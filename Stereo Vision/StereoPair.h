//
// Created by Neo on 14-12-18.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __StereoPair_H_
#define __StereoPair_H_

#include "sv.h"

namespace sv {
    class StereoPair {
    private:
        View *l;
        View *r;

        cv::Mat lEpiImg;
        cv::Mat lRectified;
        cv::Mat lR;
        cv::Mat lT;
        std::vector<cv::Point2f> lMatchedPoints;
        std::vector<cv::Point3f> lEpiLines;

        cv::Mat rEpiImg;
        cv::Mat rRectified;
        cv::Mat rR;
        cv::Mat rT;
        std::vector<cv::Point2f> rMatchedPoints;
        std::vector<cv::Point3f> rEpiLines;

        cv::Mat mFundamentalMat;
        cv::Mat mF;
        cv::Mat mEssentialMat;

        cv::Mat mDisparity;
        std::vector<cv::Scalar> palette;
        std::vector<int> mSparseDisparity;

    public:
        StereoPair(View *left, View *right) {
            l = left;
            r = right;
            for (int i = 0; i < PALETTE_SIZE; ++i) {
                palette.push_back(cv::Scalar(rand() % 255, rand() % 255, rand() % 255));
            }
        }

        void matchFeaturePoints();
        void restoreMotion();
        void rectify();
        void disparity();

        void drawEpiLines(std::vector<cv::Point3f>&, cv::Mat&);
        void drawMatchedPoints(std::vector<cv::Point2f>&, cv::Mat&);
    };
}


#endif //__StereoPair_H_
