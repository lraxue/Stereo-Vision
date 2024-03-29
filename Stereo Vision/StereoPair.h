//
// Created by Neo on 14-12-18.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __StereoPair_H_
#define __StereoPair_H_

#include "sv.h"
#include "MonoView.h"
#include "StereoView.h"
#include "PointCloud.h"

namespace sv {
    class StereoPair {
    private:
        StereoView *l;
        StereoView *r;

        cv::Mat mFundamentalMat;
        cv::Mat mEssentialMat;

        cv::Mat mDisparity;
        std::vector<cv::Scalar> palette;
        PointCloud mPointCloud;

        std::vector<float> mSparseDisparity;

    public:
        const static int LEFT = 1;
        const static int RIGHT = 2;

        StereoPair(MonoView *left, MonoView *right) {
            l = new StereoView(left);
            r = new StereoView(right);
            for (int i = 0; i < PALETTE_SIZE; ++i) {
                palette.push_back(cv::Scalar(rand() % 255, rand() % 255, rand() % 255));
            }
        }

        ~StereoPair() {
            if (nullptr != l) delete l;
            if (nullptr != r) delete r;
        }

        PointCloud &pointCloud() {
            return mPointCloud;
        }
        void matchFeaturePoints();

        void restoreMotion();

        void rectify();

        void sparseMapping();

        void denseMapping();
    };
}


#endif //__StereoPair_H_
