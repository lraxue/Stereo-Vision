//
// Created by Neo on 14-12-18.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include "StereoPair.h"
#include "View.h"

using namespace cv;
using namespace std;

namespace sv {
    void StereoPair::matchFeaturePoints() {
        FlannBasedMatcher matcher;
        vector<vector<DMatch> > knnMatches;
        vector<DMatch> goodMatchedPoints;

        matcher.knnMatch(l->descriptor(), r->descriptor(), knnMatches, 2);
        cout << "Matches: " << knnMatches.size() << endl;

        for (int i = 0; i < knnMatches.size(); ++i) {
            DMatch bestMatch = knnMatches[i][0];
            DMatch goodMatch = knnMatches[i][1];
            if (bestMatch.distance < goodMatch.distance * KNN_THRESHOLD) {
                goodMatchedPoints.push_back(bestMatch);
                lMatchedPoints.push_back(l->featurePoints()[(bestMatch.queryIdx)].pt);
                rMatchedPoints.push_back(r->featurePoints()[(bestMatch.trainIdx)].pt);
            }
        }
        cout << "Good Matches: " << goodMatchedPoints.size() << endl;
        Mat matchImg;
        drawMatches(l->img(), l->featurePoints(), r->img(), r->featurePoints(),
                goodMatchedPoints, matchImg,
                Scalar::all(-1), Scalar::all(-1), vector<char>(),
                DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        imwrite(resPath + "match.png", matchImg);
    }

    void StereoPair::restoreMotion() {
        Mat status;
        mF = findFundamentalMat(lMatchedPoints, rMatchedPoints, cv::FM_RANSAC, 3, 0.99, status);
        mF.convertTo(mFundamentalMat, CV_64F);

        computeCorrespondEpilines(lMatchedPoints, 1, mF, lEpiLines);
        l->img().copyTo(lEpiImg);
        drawEpiLines(lEpiLines, lEpiImg);
        //drawMatchedPoints(lMatchedPoints, lEpiImg);
        imwrite(resPath + "epi-L.png", lEpiImg);

        computeCorrespondEpilines(rMatchedPoints, 2, mF, rEpiLines);
        r->img().copyTo(rEpiImg);
        drawEpiLines(rEpiLines, rEpiImg);
        //drawMatchedPoints(rMatchedPoints, rEpiImg);
        imwrite(resPath + "epi-R.png", rEpiImg);

/*
        Mat E = r->intrinsicMat().t() * mFundamentalMat * l->intrinsicMat();
        SVD svd(E);
        Mat W = (Mat_<double>(3, 3) << 0, 1, 0, -1, 0, 0, 0, 0, 0);
        Mat R = svd.u * W * svd.vt, T = svd.u.col(2);

        lR = Mat::eye(3, 3, CV_64F);
        lT = (Mat_<double>(3, 1) << (0, 0, 0));

        rR = R / norm(R);
        rT = T / norm(T);
        cout << "R: " << rR << endl;
        cout << "T: " << rT << endl;
*/
    }

    void StereoPair::rectify() {
        Mat H1, H2;
        Size size = Size(l->img().cols * 1.2, l->img().rows * 1.2);
        stereoRectifyUncalibrated(lMatchedPoints, rMatchedPoints,
                mFundamentalMat,
                size,
                H2, H1, 0);


        const Mat V = (Mat_<double>(3, 4) <<
                 0, 0, l->img().cols, l->img().cols,
                 0, l->img().rows, 0, l->img().rows,
                 1, 1, 1, 1);

        auto transformQuad = [](const Mat &V, const Mat &H, Mat &T, Size &size) {
            Mat X = (H.row(0) * V) / (H.row(2) * V),
                Y = (H.row(1) * V) / (H.row(2) * V);
            double minX, maxX, minY, maxY;
            minMaxLoc(X, &minX, &maxX),
            minMaxLoc(Y, &minY, &maxY);

            T = H * (Mat_<double>(3, 3) << 1, 0, -minX, 0, 1, -minY, 0, 0, 1);
            size.width = ceil(maxX - minX);
            size.height = ceil(maxY - minY);
        };

        auto transformPoint = [](const Mat &T, Point2f &pt) {
            Mat ptHomo = (Mat_<double>(3, 1) << pt.x, pt.y, 1);
            Mat trans = (T.rowRange(0, 2) * ptHomo) / (T.row(2) * ptHomo);
            pt.x = trans.at<double>(0, 0);
            pt.y = trans.at<double>(1, 0);
            return pt;
        };

        Mat T1, T2;
        Size sizeL, sizeR;


        transformQuad(V, H1, T1, sizeL);
        transformQuad(V, H2, T2, sizeR);

        warpPerspective(lEpiImg, lRectified, T1, sizeL);
        warpPerspective(rEpiImg, rRectified, T2, sizeR);

        Point2f curPoint;
        for (int i = 0; i < lMatchedPoints.size(); ++i) {
            curPoint.x = 0, curPoint.y = lMatchedPoints[i].y;
            int offsetlX = (int) transformPoint(T1, curPoint).x;
            curPoint.x = 0, curPoint.y = rMatchedPoints[i].y;
            int offsetrX = (int) transformPoint(T2, curPoint).x;

            transformPoint(T1, lMatchedPoints[i]);
            transformPoint(T2, rMatchedPoints[i]);
            mSparseDisparity.push_back((lMatchedPoints[i].x - offsetlX) - (rMatchedPoints[i].x - offsetrX));
        }

        drawMatchedPoints(lMatchedPoints, lRectified);
        drawMatchedPoints(rMatchedPoints, rRectified);

        imwrite(resPath + "rectify-L.png", lRectified);
        imwrite(resPath + "rectify-R.png", rRectified);
    }

    void StereoPair::disparity() {

    }

    void StereoPair::drawEpiLines(vector<Point3f>& lines, Mat &img) {
        for (int i = 0; i < lines.size(); ++i) {
            Point3f *pt = &lines[i];
            line(img,
                    Point(0,- pt->z/pt->y),
                    Point(l->img().cols,-(pt->z + pt->x* l->img().cols)/pt->y),
                    palette[i % PALETTE_SIZE]);
        }
    }

    void StereoPair::drawMatchedPoints(vector<Point2f>& points, Mat &img) {
        for (int i = 0; i < points.size(); ++i) {
            Point2f *pt = &points[i];
            circle(img, Point(pt->x, pt->y), 3, palette[i % PALETTE_SIZE]);
        }
    }
}