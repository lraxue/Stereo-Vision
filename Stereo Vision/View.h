//
//  View.h
//  Stereo Vision
//
//  Created by Neo on 14-12-3.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#ifndef __VIEW_H__
#define __VIEW_H__

#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

class View {
/* Members */
private:
    /* Raw input image */
    cv::Mat mImg;
    cv::Mat rectifiedImg;

    /* Depth image */
    cv::Mat mDisparityMap;
    cv::Mat mDepthMap;

    /* Calibration information */
    cv::Mat mIntrinsicMat;
    cv::Mat mExtrinsicMat;
    cv::Mat mProjectMat; // mIntrinsic * mExtrinsic
    cv::Mat mRectifyMat;

    /* Features extracted and matched */
    cv::Mat mDescriptor;
    std::vector<cv::KeyPoint> mFeaturePoints;
    std::vector<cv::Point3f> mMatchedPoints;

    /* Assistance variables */
    std::vector<int> mRandVec;

/* Methods */
public:
    void extractFeaturePoints();

    void matchFeaturePoints(View &r);

    void restoreMotion(View &r);

    void rectify(View &r);

    View(cv::Mat img) {
        mImg = img;
        mIntrinsicMat = cv::Mat(3, 3, CV_32F);
        mExtrinsicMat = cv::Mat(3, 4, CV_32F);
    }
    cv::Mat& img() { return mImg; }

    cv::Mat& projectMat() { return mProjectMat; }
    cv::Mat& projectMat(cv::Mat mat) { return (mProjectMat = mat); }

    cv::Mat& rectifyMat() { return mRectifyMat; }
    cv::Mat& rectifyMat(cv::Mat mat) { return (mRectifyMat = mat); }

    cv::Mat& intrinsicMat() { return mIntrinsicMat; }
    cv::Mat& intrinsicMat(cv::Mat mat) { return (mIntrinsicMat = mat); }

    cv::Mat& extrinsicMat() { return mExtrinsicMat; }
    cv::Mat& extrinsicMat(cv::Mat mat) { return (mExtrinsicMat = mat); }
    cv::Mat extrinsicMat(cv::Mat R, cv::Mat t) {
        cv::Mat roiR = cv::Mat(mExtrinsicMat, cv::Rect(0, 0, 3, 3));
        R.copyTo(roiR);
        cv::Mat roiT = mExtrinsicMat(cv::Rect(3, 0, 1, 3));
        t.copyTo(roiT);
        return mExtrinsicMat;
    }

    cv::Mat &getDescriptor() {
        return mDescriptor;
    }

    std::vector<cv::KeyPoint>& featurePoints() {
        return mFeaturePoints;
    }

    std::vector<cv::Point3f>& matchedPoints() {
        return mMatchedPoints;
    }

    void pushMatchedPointWithIndex(int i) {
        cv::Point2f *pt = &mFeaturePoints[i].pt;
        mMatchedPoints.push_back(cv::Point3f(pt->x, pt->y, 1.0f));
    }

private:
    bool genRandIndex(int length, int base);
};

#endif /* defined __VIEW_H__ */
