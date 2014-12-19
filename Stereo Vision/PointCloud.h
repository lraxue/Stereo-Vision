//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __PointCloud_H_
#define __PointCloud_H_
#include "sv.h"

namespace sv {
    void displayPointCloud();
    void drawPointCloud();

    class PointCloud {
    private:
        std::vector<cv::Point3f> mPointCloud;
    public:
        PointCloud();
        void pushPoint(cv::Point3f pt) {
            mPointCloud.push_back(pt);
        }
    };
}


#endif //__PointCloud_H_
