//
//  View.cpp
//  Stereo Vision
//
//  Created by Neo on 14-12-3.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#include "View.h"
#include <algorithm>
#include <opencv2/flann/flann.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;

#define DEBUG_
/* Public interfaces */
void View::extractFeaturePoints() {
    SiftFeatureDetector detector;
    SiftDescriptorExtractor extractor;
    detector.detect(mImg, mFeaturePoints);
    extractor.compute(mImg, mFeaturePoints, mDescriptor);
}

void View::matchFeaturePoints(View &r) {
    FlannBasedMatcher matcher;
    vector<DMatch> matchedPoints, filteredMatchedPoints;
    matcher.match(mDescriptor,
            r.getDescriptor(),
            matchedPoints);

    /* TODO: Update this naive match filter */
    float minDist = INT32_MAX, maxDist = INT32_MIN;
    for (int i = 0; i < mDescriptor.rows; ++i) {
        float dist = matchedPoints[i].distance;
        minDist = dist < minDist ? dist : minDist;
        maxDist = dist > maxDist ? dist : maxDist;
    }

    for (int i = 0; i < mDescriptor.rows; ++i) {
        if (matchedPoints[i].distance < 4 * minDist) {
            filteredMatchedPoints.push_back(matchedPoints[i]);
            pushMatchedPointWithIndex(matchedPoints[i].queryIdx);
            r.pushMatchedPointWithIndex(matchedPoints[i].trainIdx);
        }
    }

    Mat imgMatch;
    drawMatches(mImg, mFeaturePoints,
            r.img(), r.featurePoints(),
            filteredMatchedPoints,
            imgMatch);
}

void View::restoreMotion(View &r) {
    /* Eight Points Method */
    const int pointNum = 12;
    const int row = pointNum, col = 9;

    /* Get random points */
    if (not genRandIndex(pointNum, (int) matchedPoints().size()))
        return;

    vector<Point3f> lVec, rVec;
    for (int i = 0; i < pointNum; ++i) {
        lVec.push_back(matchedPoints()[mRandVec[i]]);
        rVec.push_back(r.matchedPoints()[mRandVec[i]]);
    }

    /* Get Point-pair matrices */
    // TODO: RANSAC
    Mat A = Mat(row, col, CV_32FC1);
    for (int i = 0; i < row; ++i) {
        float x1 = lVec[i].x, y1 = lVec[i].y,
                x2 = rVec[i].x, y2 = rVec[i].y;

        float *rowVec = A.ptr<float>(i);
        rowVec[0] = x1 * x2;
        rowVec[1] = x1 * y2;
        rowVec[2] = x1;
        rowVec[3] = y1 * x2;
        rowVec[4] = y1 * y2;
        rowVec[5] = y1;
        rowVec[6] = x2;
        rowVec[7] = y2;
        rowVec[8] = 1.0f;
    }

    /* Use min-square method to restore Foundation Matrix */
    Mat e, v;
    eigen(A.t() * A, e, v);
    Mat FoundationMat = (v.row(v.rows - 1)).reshape(0, 3);

    /* Decompose to retrieve Essential Matrix */
    /**
    * Not necessary to explicitly write
    * Mat EssentialMat = svd.u * (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 0) * svd.vt;
    */
    SVD svd(intrinsicMat().t() * FoundationMat * r.intrinsicMat());

    /* Extract [R, t] */
    Mat W = (Mat_<float>(3, 3) << 0, -1, 0, 1, 0, 0, 0, 0, 1),
            R = svd.u * W * svd.vt,
            t = svd.u.col(2);

    extrinsicMat(Mat::eye(3, 3, CV_32F), (Mat_<float>(3, 1) << 0, 0, 0));
    projectMat(mIntrinsicMat * mExtrinsicMat);

    r.extrinsicMat(R, t);
    r.projectMat(r.intrinsicMat() * r.extrinsicMat());
    cout << r.extrinsicMat() << endl;
    cout << r.projectMat() << endl;
}

void View::rectify(View &r) {
    Mat c1 = - mProjectMat.colRange(0, 3).inv() * (mProjectMat.col(3)),
    c2 = - r.projectMat().colRange(0, 3).inv() * (r.projectMat().col(3));
    Mat v1 = c1 - c2;
    Mat v2 = mExtrinsicMat.colRange(0, 3).row(2).t().cross(v1);
    Mat v3 = v1.cross(v2);
    Mat R = Mat(3, 3, CV_32F);
    R.row(0) = v1.t() / norm(v1);
    R.row(1) = v2.t() / norm(v2);
    R.row(2) = v3.t() / norm(v3);
    Mat A = (mIntrinsicMat + r.mIntrinsicMat) / 2;
    A.at<float>(0, 1) = 0;

    intrinsicMat(A);
    extrinsicMat(R, -R * c1);
    Mat newProjectMatL = mIntrinsicMat * mExtrinsicMat;
    rectifyMat(newProjectMatL.colRange(0, 3) * (mProjectMat.colRange(0, 3).inv()));
    projectMat(newProjectMatL);

    r.intrinsicMat(A);
    r.extrinsicMat(R, -R * c2);
    Mat newProjectMatR = r.intrinsicMat() * r.extrinsicMat();
    r.rectifyMat(newProjectMatR.colRange(0, 3) * (r.projectMat().colRange(0, 3).inv()));
    r.projectMat(newProjectMatR);

#ifdef DEBUG_
    Mat outputImg;
    cout << rectifyMat() << endl;
    Mat t = (Mat_<float>(3, 3) << 0.03, 0.03, 0, 0.85, -0.54, 0, 0, 0, 1);
    warpPerspective(getImg(), outputImg, rectifyMat(), Size(getImg().rows, getImg().cols));
    namedWindow("Test");
    imshow("Test", outputImg);
    waitKey(-1);
#endif
}

/* Private Methods */

/* For RANSAC */
bool View::genRandIndex(int length, int base) {
    mRandVec.clear();
    if (base < length)
        return false;

    while (length) {
        int randIndex = rand() % base;
        if (mRandVec.end()
                == find(mRandVec.begin(), mRandVec.end(), randIndex)) {
            --length;
            mRandVec.push_back(randIndex);
        }
    }
    return true;
}