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
#include <opencv2/opencv.hpp>

class View {
    /* Raw input image */
    cv::Mat mImg;

    /* Depth image */
    cv::Mat mDisparityMap;
    cv::Mat mDepthMap;

    /* Calibration information */
    cv::Mat mIntrinsicMat;
    cv::Mat mExtrinsicMat;

    /* Features extracted and matched */
    cv::Mat mDescriptor;
    std::vector<cv::KeyPoint> mFeaturePoints;
    std::vector<cv::Point3f> mKeyPoints;

    /* Assistance variables */
    std::vector<int> mRandVec;

protected:
    cv::Mat &getImg() {
        return mImg;
    }

    cv::Mat &getDescriptor() {
        return mDescriptor;
    }

    std::vector<cv::KeyPoint> &getFeaturePoints() {
        return mFeaturePoints;
    }

    std::vector<cv::Point3f> &getKeyPoints() {
        return mKeyPoints;
    }

    void pushKeyPointWithIndex(int i) {
        cv::Point2f *pt = &mFeaturePoints[i].pt;
        mKeyPoints.push_back(cv::Point3f(pt->x, pt->y, 1.0f));
    }

public:
    void matchFeaturePoints(View &r);

    void restoreMotion(View &r);

    View(cv::Mat mat) {
        mImg = mat;
    }

    void extractFeaturePoints();

    void setIntrinsicMat(cv::Mat mat) {
        mIntrinsicMat = mat;
    }

    cv::Mat &getIntrinsicMat() {
        return mIntrinsicMat;
    }

    void setExtrinsicMat(cv::Mat mat) {
        mExtrinsicMat = mat;
    }

    cv::Mat &getExtrinsicMat() {
        return mExtrinsicMat;
    }

private:
    bool getRandVec(int length, int base);
};

#endif /* defined __VIEW_H__ */
