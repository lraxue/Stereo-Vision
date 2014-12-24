//
//  MonoView.h
//  Stereo Vision
//
//  Created by Neo on 14-12-3.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#ifndef __VIEW_H__
#define __VIEW_H__

#include "sv.h"

namespace sv {
    class MonoView {

        /* Members */
    private:
        /* Raw input image */
        cv::Mat mImg;

        /* Calibration information */
        cv::Mat mIntrinsicMat;

        /* Features extracted and matched */
        cv::Mat mDescriptor;
        std::vector<cv::KeyPoint> mFeaturePoints;

        /* Methods */
    public:
        void extractFeaturePoints();

        MonoView() {
        }

        MonoView(cv::Mat img) {
            mImg = img;
            mIntrinsicMat = cv::Mat::eye(3, 3, CV_64F);
        }

        cv::Mat &img() {
            return mImg;
        }

        cv::Mat &intrinsicMat() {
            return mIntrinsicMat;
        }

        cv::Mat &intrinsicMat(cv::Mat mat) {
            return (mIntrinsicMat = mat);
        }

        cv::Mat &descriptor() {
            return mDescriptor;
        }

        std::vector<cv::KeyPoint> &featurePoints() {
            return mFeaturePoints;
        }
    };
}

#endif /* defined __VIEW_H__ */
