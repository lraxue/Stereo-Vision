//
// Created by Neo on 14-12-18.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __StereoPair_H_
#define __StereoPair_H_

#include "sv.h"
#include "MonoView.h"
#include "StereoView.h"

namespace sv {
    class StereoPair {
    private:
        StereoView *l;
        StereoView *r;

        cv::Mat mF;
        cv::Mat mEssentialMat;

        cv::Mat mDisparity;
        std::vector<cv::Scalar> palette;
        std::vector<int> mSparseDisparity;

    public:
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

        void matchFeaturePoints();
        void restoreMotion();
        void rectify();
        void disparity();
    };
}


#endif //__StereoPair_H_
