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

using namespace std;
using namespace cv;

int main() {
    const string dirPath = "/Users/Neo/code/Visual/Stereo-Vision/Assets/";

    Mat imgLeft = imread(dirPath + "B00.jpg", CV_64FC3),
        imgRight = imread(dirPath + "B01.jpg", CV_64FC3);

    sv::View l = sv::View(imgLeft), r = sv::View(imgRight);

    l.extractFeaturePoints();
    r.extractFeaturePoints();

    sv::StereoPair pair = sv::StereoPair(&l, &r);
    pair.matchFeaturePoints();
    pair.restoreMotion();
    pair.rectify();
    pair.disparity();
    return 0;
}