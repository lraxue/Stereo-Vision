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

    void StereoView::drawEpipolarLines(std::vector<cv::Scalar> &palette) {
        for (int i = 0; i < mEpiLines.size(); ++i) {
            cv::Point3f *pt = &mEpiLines[i];
            line(mEpiImg,
                    cv::Point(0, -pt->z / pt->y),
                    cv::Point(img().cols, -(pt->z + pt->x * img().cols) / pt->y),
                    palette[i % PALETTE_SIZE]);
        }
    }

    void StereoView::drawMatchedPoints(cv::Mat &img, std::vector<cv::Scalar> &palette) {
        for (int i = 0; i < mMatchedPoints.size(); ++i) {
            cv::Point2f *pt = &mMatchedPoints[i];
            circle(img,
                    cv::Point(pt->x, pt->y), 3, palette[i % PALETTE_SIZE]);
        }
    }

    void StereoView::rectify(cv::Mat &H, std::vector<cv::Scalar> &palette) {
        /* Calculate the real transform matrix adaptive for displaying */
        const cv::Mat V =
                (cv::Mat_<double>(3, 4) <<
                0, 0, img().cols, img().cols,
                0, img().rows, 0, img().rows,
                1, 1, 1, 1);

        cv::Mat X = (H.row(0) * V) / (H.row(2) * V),
                Y = (H.row(1) * V) / (H.row(2) * V),
                T;
        cv::Size size = cv::Size(img().cols, img().rows);

        double minX, minY, maxX, maxY;
        cv::minMaxLoc(X, &minX, &maxX);
        cv::minMaxLoc(Y, &minY, &maxY);
        /* Attach offset for better displaying */
        T = H * (cv::Mat_<double>(3, 3) << 1, 0, -minX, 0, 1, -minY, 0, 0, 1);

        /* Transform the matched feature points to the new plane */
        std::for_each(matchedPoints().begin(), matchedPoints().end(), [H](cv::Point2f &pt) {
            cv::Mat ptHomo = (cv::Mat_<double>(3, 1) << pt.x, pt.y, 1);
            cv::Mat trans = (H.rowRange(0, 2) * ptHomo) / (H.row(2) * ptHomo);
            pt.x = trans.at<double>(0, 0);
            pt.y = trans.at<double>(1, 0);
        });

        /* Reproject matrix */
        mQ = H * R();

        /* Rectify */
        cv::warpPerspective(mEpiImg, mRectifiedImg, H, size);
        //drawMatchedPoints(mRectifiedImg, palette);
    }
}