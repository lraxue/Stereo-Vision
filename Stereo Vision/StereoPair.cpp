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
        
        cout << mFundamentalMat << endl;

        computeCorrespondEpilines(l->matchedPoints(), 1, mFundamentalMat, r->epipolarLines());
        computeCorrespondEpilines(r->matchedPoints(), 2, mFundamentalMat, l->epipolarLines());

        l->drawEpipolarLines(palette);
        l->drawMatchedPoints(l->epipolarImg(), palette);
        imwrite(resPath + "epi-L.png", l->epipolarImg());

        r->drawEpipolarLines(palette);
        r->drawMatchedPoints(r->epipolarImg(), palette);
        imwrite(resPath + "epi-R.png", r->epipolarImg());

        mEssentialMat = mFundamentalMat;
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

        /* Correct the offset depth */
        float dMin = 1e10f;
        for (int i = 0; i < length; ++i) {
            float d = (l->matchedPoints()[i].x - r->matchedPoints()[i].x);
            cout << d << endl;
            mSparseDisparity.push_back(d);
            dMin = dMin < d ? dMin : d;
        }

        /* Reverse the disparity for Depth */
        float zMin = 1e10f, zMax = -1e10f;
        for (int i = 0; i < length; ++i) {
            float d = mSparseDisparity[i];
            float z = 1 / ((d - dMin) + 80);
            mSparseDisparity[i] = z;

            zMin = zMin < z ? zMin : z;
            zMax = zMax > z ? zMax : z;
        }

        /* Rescale the color map */
        const int colorTabLen = 256;
        const float colorGap = 1.0f / colorTabLen;
        int colorMap[colorTabLen] = {0};

        float scale = zMax - zMin;
        for (int i = 0; i < length; ++i) {
            float z = (mSparseDisparity[i] - zMin) / scale;
            mSparseDisparity[i] = z;
            colorMap[(int)floor(z / colorGap)] ++;

            Point3f pt = Point3f(l->matchedPoints()[i].x, l->matchedPoints()[i].y, z);
            pointCloud().pushPoint(pt);
        }

        /* Histogram equalization map */
        float sum = 0;
        for (int j = 0; j < colorTabLen; ++j) {
            float color = colorMap[j];
            colorMap[j] = (1 - sum) * colorTabLen;
            sum += color / length;
        }

        /* Draw the final map */
        for (int i = 0; i < length; ++i) {
            int color = colorMap[(int)floor(mSparseDisparity[i] / colorGap)];
            circle(l->rectifiedImg(), l->matchedPoints()[i], 2, cv::Scalar(color, color, color), 3);
            circle(r->rectifiedImg(), r->matchedPoints()[i], 2, cv::Scalar(color, color, color), 3);
        }

        imwrite(resPath + "rectify-L.png", l->rectifiedImg());
        imwrite(resPath + "rectify-R.png", r->rectifiedImg());
    }
    
    void StereoPair::denseMapping() {
        FastFeatureDetector fast(10);
        SiftDescriptorExtractor extractor;
        cv::Mat lDescriptor, rDescriptor;
        
        vector<KeyPoint> lKeyPoints, rKeyPoints;
        
        fast.detect(l->img(), lKeyPoints);
        fast.detect(r->img(), rKeyPoints);

        extractor.compute(l->img(), lKeyPoints, lDescriptor);
        extractor.compute(r->img(), rKeyPoints, rDescriptor);
        
//        drawKeypoints(l->img(), lKeyPoints, l->img(), Scalar(0, 0, 0));
//        drawKeypoints(r->img(), rKeyPoints, r->img(), Scalar(0, 0, 0));

        FlannBasedMatcher matcher;
        vector<vector<DMatch> > knnMatches;
        vector<DMatch> goodMatchedPoints;
        
        matcher.knnMatch(lDescriptor, rDescriptor, knnMatches, 2);
        cout << "Matches: " << knnMatches.size() << endl;
        
        for (int i = 0; i < knnMatches.size(); ++i) {
            DMatch bestMatch = knnMatches[i][0];

            Point2f lPoint = lKeyPoints[bestMatch.queryIdx].pt;
            Point2f rPoint = rKeyPoints[bestMatch.trainIdx].pt;
            Mat epiLine = mFundamentalMat * (Mat_<double>(3, 1) << lPoint.x, lPoint.y, 1);
            if (norm(epiLine.t() * (Mat_<double>(3, 1) << rPoint.x, rPoint.y, 1)) < 5) {
                goodMatchedPoints.push_back(bestMatch);
                l->matchedPoints().push_back(lPoint);
                r->matchedPoints().push_back(rPoint);
            }
        }
        cout << "Good Matches: " << goodMatchedPoints.size() << endl;
        Mat matchImg;
        drawMatches(l->img(), lKeyPoints,
                    r->img(), rKeyPoints,
                    goodMatchedPoints, matchImg,
                    Scalar::all(-1), Scalar::all(-1), vector<char>(),
                    DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        
        imwrite(resPath + "match-dense.png", matchImg);
        
        sparseMapping();
        /*
        int count = 0;
        for (int i = 0; i < lKeyPoints.size(); ++i) {
            Mat epiLine = mFundamentalMat * (Mat_<double>(3, 1) << lKeyPoints[i].pt.x, lKeyPoints[i].pt.y, 1);

            for (int j = 0; j < rKeyPoints.size(); ++j) {
                if (norm(epiLine.t() * (Mat_<double>(3, 1) << rKeyPoints[j].pt.x, rKeyPoints[j].pt.y, 1)) < 5) {
                    count ++;
                }
            }
        }
        cout << count << endl;*/

    }
}