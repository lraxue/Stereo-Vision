//
//  main.cpp
//  Stereo Vision
//
//  Created by Neo on 14-11-28.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "sv.h"
#include "View.h"
#include "StereoPair.h"
#include "Consts.h"

using namespace std;
using namespace cv;

int main() {
    const string dirPath = "/Users/Neo/code/Visual/Stereo-Vision/Assets/",
            imgLeftPath = dirPath + "im0.png",
            imgRightPath = dirPath + "im1.png";

    Mat imgLeft = imread(imgLeftPath, CV_32FC3),
        imgRight = imread(imgRightPath, CV_32FC3);

    resize(imgLeft, imgLeft, Size(imgLeft.size[1] / 4 , imgLeft.size[0] / 4));
    resize(imgRight, imgRight, Size(imgRight.size[1] / 4, imgRight.size[0] / 4));


    sv::View l = sv::View(imgLeft), r = sv::View(imgRight);

    l.intrinsicMat(Mat(3, 3, CV_64F, intrisicMat0) / 4);
    r.intrinsicMat(Mat(3, 3, CV_64F, intrisicMat1) / 4);

    l.extractFeaturePoints();
    r.extractFeaturePoints();

    sv::StereoPair pair = sv::StereoPair(&l, &r);
    pair.matchFeaturePoints();
    pair.restoreMotion();
    pair.rectify();
    pair.disparity();
    return 0;
}