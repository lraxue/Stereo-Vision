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
            pushKeyPointWithIndex(matchedPoints[i].queryIdx);
            r.pushKeyPointWithIndex(matchedPoints[i].trainIdx);
        }
    }

    Mat imgMatch;
    drawMatches(mImg, mFeaturePoints,
            r.getImg(), r.getFeaturePoints(),
            filteredMatchedPoints,
            imgMatch);
    namedWindow("Test");
    imshow("Test", imgMatch);
    waitKey(-1);
}

void View::restoreMotion(View &r) {
    /* Eight Points Method */
    const int pointNum = 8;
    const int row = pointNum, col = 9;

    /* Get random points */
    if (not getRandVec(pointNum, (int) mKeyPoints.size()))
        return;

    vector<Point3f> lVec, rVec;
    for (int i = 0; i < pointNum; i++) {
        lVec.push_back(mKeyPoints[mRandVec[i]]);
        rVec.push_back(r.getKeyPoints()[mRandVec[i]]);
    }

    /* Get Point-pair matrices */
    // TODO: RANSAC
    Mat A = Mat(row, col, CV_32FC1);
    for (int i = 0; i < row; i++) {
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
    SVD svd(mIntrinsicMat.t() * FoundationMat * r.getIntrinsicMat());

    /* Extract [R, t] */
    Mat W = (Mat_<float>(3, 3) << 0, -1, 0, 1, 0, 0, 0, 0, 1),
            R = svd.u * W * svd.vt,
            t = svd.u.col(2);

    cout << R << endl;
    cout << t << endl;

}

/* Private Methods */
/* For RANSAC */
bool View::getRandVec(int length, int base) {
    mRandVec.clear();
    if (base < length)
        return false;

    while (length) {
        int randIndex = rand() % base;
        if (mRandVec.end()
                == find(mRandVec.begin(), mRandVec.end(), randIndex)) {
            length--;
            mRandVec.push_back(randIndex);
        }
    }
    return true;
}