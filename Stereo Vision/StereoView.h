//
// Created by Neo on 14-12-22.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __StereoView_H_
#define __StereoView_H_
#include "sv.h"
#include "MonoView.h"

namespace sv {
    class StereoView : public MonoView {
    private:
        cv::Mat mEpiImg;
        cv::Mat mRectified;
        cv::Mat mR;
        cv::Mat mT;
        std::vector<cv::Point2f> mMatchedPoints;
        std::vector<cv::Point3f> mEpiLines;

    public:
        StereoView(MonoView *);

        void pushMatchedPoint(int index);
        void drawEpipolarLines(cv::Mat &, int , std::vector<cv::Scalar>&);
        void drawMatchedPoints(std::vector<cv::Scalar>&, cv::Mat &img);

        cv::Mat &epipolarImg() {
            return mEpiImg;
        }
        cv::Mat &rectifiedImg() {
            return mRectified;
        }

        std::vector<cv::Point2f>& matchedPoints() {
            return mMatchedPoints;
        }
        std::vector<cv::Point3f>& epipolarLines() {
            return mEpiLines;
        }

    };
}


#endif //__StereoView_H_
