//
// Created by Neo on 14-12-22.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include "StereoView.h"

namespace sv {
    /* Can't convert from derived class to base class. */
    /* Have to explicitly copy contents */
    StereoView::StereoView(MonoView *monoView) {
        this->img() = monoView->img();
        this->mEpiImg = monoView->img();
        this->intrinsicMat() = monoView->intrinsicMat();
        this->descriptor() = monoView->descriptor();
        this->featurePoints() = monoView->featurePoints();
    }

    void StereoView::pushMatchedPoint(int index) {
        mMatchedPoints.push_back(featurePoints()[index].pt);
    }

    void StereoView::drawEpipolarLines(cv::Mat &F, int i, std::vector<cv::Scalar> &palette) {
        computeCorrespondEpilines(mMatchedPoints, i, F, mEpiLines);
        for (int i = 0; i < mEpiLines.size(); ++i) {
            cv::Point3f *pt = &mEpiLines[i];
            line(mEpiImg,
                    cv::Point(0, -pt->z / pt->y),
                    cv::Point(img().cols, -(pt->z + pt->x * img().cols) / pt->y),
                    palette[i % PALETTE_SIZE]);
        }
    }

    void StereoView::drawMatchedPoints(std::vector<cv::Scalar> &palette, cv::Mat &img) {
        for (int i = 0; i < mMatchedPoints.size(); ++i) {
            cv::Point2f *pt = &mMatchedPoints[i];
            circle(img,
                    cv::Point(pt->x, pt->y), 3, palette[i % PALETTE_SIZE]);
        }
    }

}