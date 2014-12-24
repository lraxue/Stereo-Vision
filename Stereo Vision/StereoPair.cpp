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
                r->pushMatchedPoint(bestMatch.trainIdx);
                l->pushMatchedPoint(bestMatch.queryIdx);
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
                findFundamentalMat(l->matchedPoints(), r->matchedPoints());

        computeCorrespondEpilines(l->matchedPoints(), 1, mFundamentalMat, r->epipolarLines());
        computeCorrespondEpilines(r->matchedPoints(), 2, mFundamentalMat, l->epipolarLines());

        l->drawEpipolarLines(palette);
        l->drawMatchedPoints(l->epipolarImg(), palette);
        imwrite(resPath + "epi-L.png", l->epipolarImg());

        r->drawEpipolarLines(palette);
        r->drawMatchedPoints(r->epipolarImg(), palette);
        imwrite(resPath + "epi-R.png", r->epipolarImg());

        mEssentialMat = r->intrinsicMat().t() * mFundamentalMat * l->intrinsicMat();
        SVD svd(mEssentialMat);
        Mat W = (Mat_<double>(3, 3) << 0, 1, 0, -1, 0, 0, 0, 0, 0);
        Mat R = svd.u * W * svd.vt, T = svd.u.col(2);

        l->R(Mat::eye(3, 3, CV_64F));
        /* Orientation */
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
                H1, H2, 0);

        l->rectify(H1, palette);
        r->rectify(H2, palette);
    }

    void StereoPair::sparseMapping() {
        Mat reproj = r->Q().inv();
        reproj = reproj / norm(reproj);

        size_t length = l->matchedPoints().size();
        mPointCloud.clear();

        /* Rescale depth */
        float zMin = 1e10f, zMax = -1e10f;
        for (int i = 0; i < length; ++i) {
            float disparity = abs(l->matchedPoints()[i].x - r->matchedPoints()[i].x);
            float z = 1 / disparity;
            Point3f pt = Point3f(l->matchedPoints()[i].x, l->matchedPoints()[i].y, z);
            pointCloud().pushPoint(pt);
            zMin = zMin < z ? zMin : z;
            zMax = zMax > z ? zMax : z;
        }

        float scale = zMax - zMin;
        for (int i = 0; i < length; ++i) {
            float z = pointCloud()(i).z;
            z = (z - zMin) / scale * 1000;
            pointCloud()(i).z = z;

            double color = (1 - sqrt(cbrt(z / 1000))) * 255;
            circle(l->rectifiedImg(), l->matchedPoints()[i], 2, cv::Scalar(color, color, color), 4);
            circle(r->rectifiedImg(), r->matchedPoints()[i], 2, cv::Scalar(color, color, color), 4);

            cout << pointCloud()(i) << endl;
        }

        imwrite(resPath + "rectify-L.png", l->rectifiedImg());
        imwrite(resPath + "rectify-R.png", r->rectifiedImg());
    }
}