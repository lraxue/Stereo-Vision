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
    const string dirPath = "/Users/Neo/code/Visual/Stereo-Vision/Assets/",
            imgLeftPath = dirPath + "im0.png",
            imgRightPath = dirPath + "im1.png";

    Mat imgLeft = imread(imgLeftPath, CV_32FC3),
            imgRight = imread(imgRightPath, CV_32FC3);

    resize(imgLeft, imgLeft, Size(imgLeft.size[1], imgLeft.size[0]));
    resize(imgRight, imgRight, Size(imgRight.size[1], imgRight.size[0]));

    View l = View(imgLeft), r = View(imgRight);

    l.intrinsicMat(Mat(3, 3, CV_32F, intrisicMat0));
    r.intrinsicMat(Mat(3, 3, CV_32F, intrisicMat1));

    l.extractFeaturePoints();
    r.extractFeaturePoints();

    l.matchFeaturePoints(r);

    /*
    l.restoreMotion(r);
    l.rectify(r);
    */

    return 0;
}