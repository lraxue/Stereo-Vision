//
// Created by Neo on 14-12-18.
// Copyright (c) 2014 PKU. All rights reserved.
//


#ifndef __SV_H_
#define __SV_H_

#include <string>
#include <vector>
#include <iostream>
#include <iosfwd>
#include <algorithm>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2/flann/flann.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>

namespace sv {
    const static std::string resPath = "/Users/Neo/code/Visual/Stereo-Vision/Results/";
    const static int PALETTE_SIZE = 100;
    const static float KNN_THRESHOLD = 0.6f;

    class MonoView;
    class StereoView;
    class StereoPair;
    class PointCloud;
};

#endif
