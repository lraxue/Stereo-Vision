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
        cv::Mat mRectifiedImg;

        cv::Mat mR;
        cv::Mat mQ;
        std::vector<cv::Point2f> mMatchedPoints;
        std::vector<cv::Point3f> mEpiLines;

    public:
        StereoView(MonoView *);

        void pushMatchedPoint(int index);

        void drawEpipolarLines(std::vector<cv::Scalar> &);

        void drawMatchedPoints(cv::Mat &, std::vector<cv::Scalar> &);

        void rectify(cv::Mat &H, std::vector<cv::Scalar> &);

        cv::Mat &epipolarImg() {
            return mEpiImg;
        }

        cv::Mat &rectifiedImg() {
            return mRectifiedImg;
        }

        cv::Mat &R() {
            return mR;
        }

        cv::Mat &R(cv::Mat r) {
            return (mR = r / norm(r));
        }

        cv::Mat &Q() {
            return mQ;
        }

        cv::Mat &Q(cv::Mat q) {
            return (mQ = q / norm(q));
        }

        std::vector<cv::Point2f> &matchedPoints() {
            return mMatchedPoints;
        }

        std::vector<cv::Point3f> &epipolarLines() {
            return mEpiLines;
        }
    };
}


#endif //__StereoView_H_
