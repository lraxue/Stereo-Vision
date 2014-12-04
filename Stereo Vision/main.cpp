//
//  main.cpp
//  Stereo Vision
//
//  Created by Neo on 14-11-28.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "View.h"
#include "calib.h"

using namespace std;
using namespace cv;

int main() {
    const string dirPath = "/Users/Neo/Desktop/stereo-vision/",
            imgLeftPath = dirPath + "im0.png",
            imgRightPath = dirPath + "im1.png";

    Mat imgLeft = imread(imgLeftPath),
            imgRight = imread(imgRightPath);

    cv::resize(imgLeft, imgLeft, Size(imgLeft.size[1] / 4, imgLeft.size[0] / 4));
    cv::resize(imgRight, imgRight, Size(imgRight.size[1] / 4, imgRight.size[0] / 4));

    View l = View(imgLeft), r = View(imgRight);

    l.setIntrinsicMat(Mat(3, 3, CV_32F, intrisicMat0));
    r.setIntrinsicMat(Mat(3, 3, CV_32F, intrisicMat1));

    l.extractFeaturePoints();
    r.extractFeaturePoints();

    l.matchFeaturePoints(r);
    l.restoreMotion(r);

    return 0;
}