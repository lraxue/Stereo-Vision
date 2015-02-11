//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __PointCloud_H_
#define __PointCloud_H_

#include "sv.h"

namespace sv {
    void onMouseClick(GLFWwindow *window, int button, int action, int mods);
    void onMouseDrag(GLFWwindow *window);
    void onMouseScroll(GLFWwindow *window, double xOffset, double yOffset);

    void initGl(const std::string windowName, void *data);

    void render(GLFWwindow *window, void *data);
    void drawPoints(PointCloud *data);

    class PointCloud {
    private:
        std::vector<cv::Point3f> mPointCloud;
    public:
        PointCloud() { }

        size_t size() {
            return mPointCloud.size();
        }

        cv::Point3f &operator[](int i) {
            return mPointCloud[i];
        }

        void pushPoint(cv::Point3f pt) {
            mPointCloud.push_back(pt);
        }

        void clear() {
            mPointCloud.clear();
        }

        void reScale();
    };
}


#endif //__PointCloud_H_
