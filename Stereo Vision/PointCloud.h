//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __PointCloud_H_
#define __PointCloud_H_

#include "sv.h"

namespace sv {
    void initGl(const std::string window, void *data);
    void onMouseEvent(int event, int x, int y, int z, void *data);
    void onDraw(void *data);

    class PointCloud {
    private:
        std::vector<cv::Point3f> mPointCloud;
        float rotate[2] = {0, 0};
    public:
        PointCloud() {}
        size_t size() {
            return mPointCloud.size();
        }
        cv::Point3f& operator() (int i) {
            return mPointCloud[i];
        }
        void pushPoint(cv::Point3f pt) {
            mPointCloud.push_back(pt);
        }
        void clear() {
            mPointCloud.clear();
        }
        float rotateX(float r = 0.0f) {
            return rotate[0] += r;
        }
        float rotateY(float r = 0.0f) {
            return rotate[1] += r;
        }
    };
}


#endif //__PointCloud_H_
