//
// Created by Neo on 14-12-18.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include "StereoPair.h"

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
                l->pushMatchedPoint(bestMatch.queryIdx);
                r->pushMatchedPoint(bestMatch.trainIdx);
            }
        }
        cout << "Good Matches: " << goodMatchedPoints.size() << endl;
        Mat matchImg;
        drawMatches(l->img(), l->featurePoints(),
                r->img(), r->featurePoints(),
                goodMatchedPoints, matchImg,
                Scalar::all(-1), Scalar::all(-1), vector<char>(),
                DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        imwrite(resPath + "match.png", matchImg);
    }

    void StereoPair::restoreMotion() {
        Mat status;
        mFundamentalMat =
                findFundamentalMat(l->matchedPoints(), r->matchedPoints(), cv::FM_RANSAC, 3, 0.99, status);

        l->drawEpipolarLines(mFundamentalMat, LEFT, palette);
        l->drawMatchedPoints(palette, l->epipolarImg());
        imwrite(resPath + "epi-L.png", l->epipolarImg());

        r->drawEpipolarLines(mFundamentalMat, RIGHT, palette);
        r->drawMatchedPoints(palette, r->epipolarImg());
        imwrite(resPath + "epi-R.png", r->epipolarImg());

        mEssentialMat = r->intrinsicMat().t() * mFundamentalMat * l->intrinsicMat();
        SVD svd(mEssentialMat);
        Mat W = (Mat_<double>(3, 3) << 0, 1, 0, -1, 0, 0, 0, 0, 0);
        Mat R = svd.u * W * svd.vt, T = svd.u.col(2);

        l->R(Mat::eye(3, 3, CV_64F));
        if (R.at<double>(2, 2) < 0) {
            R = -R;
        }
        r->R(R);
    }

    void StereoPair::rectify() {
        Mat H1, H2;
        Size size = Size(l->img().cols * 1.2, l->img().rows * 1.2);
        stereoRectifyUncalibrated(l->matchedPoints(), r->matchedPoints(),
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

        /* Reproject matrix */
        l->Q(T1 * l->R());
        r->Q(T2 * r->R());

        warpPerspective(l->epipolarImg(), l->rectifiedImg(), T1, sizeL);
        warpPerspective(r->epipolarImg(), r->rectifiedImg(), T2, sizeR);
/*
        Point2f curPoint;
        for (int i = 0; i < l->matchedPoints().size(); ++i) {
            curPoint.x = 0, curPoint.y = l->matchedPoints()[i].y;
            int offsetlX = (int) transformPoint(T1, curPoint).x;
            curPoint.x = 0, curPoint.y = r->matchedPoints()[i].y;
            int offsetrX = (int) transformPoint(T2, curPoint).x;

            transformPoint(T1, l->matchedPoints()[i]);
            transformPoint(T2, r->matchedPoints()[i]);
            mSparseDisparity.push_back((l->matchedPoints()[i].x - offsetlX) - (r->matchedPoints()[i].x - offsetrX));
        }
*/
        //l->drawMatchedPoints(palette, l->rectifiedImg());
        //r->drawMatchedPoints(palette, r->rectifiedImg());

        imwrite(resPath + "rectify-L.png", l->rectifiedImg());
        imwrite(resPath + "rectify-R.png", r->rectifiedImg());
    }

    void StereoPair::disparity() {

    }
}