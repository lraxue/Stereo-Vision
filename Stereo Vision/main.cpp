//
//  main.cpp
//  Stereo Vision
//
//  Created by Neo on 14-11-28.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#include "sv.h"
#include "MonoView.h"
#include "StereoView.h"
#include "StereoPair.h"
#include "PointCloud.h"

using namespace std;
using namespace cv;

#define GLU

int main() {
    const string dirPath = "/Users/Neo/code/Visual/Stereo-Vision/Assets/";

#ifndef GL
    Mat imgLeft = imread(dirPath + "B01.jpg", CV_64FC3),
        imgRight = imread(dirPath + "B02.jpg", CV_64FC3);

    sv::MonoView l = sv::MonoView(imgLeft), r = sv::MonoView(imgRight);

    l.extractFeaturePoints();
    r.extractFeaturePoints();

    sv::StereoPair pair = sv::StereoPair(&l, &r);
    pair.matchFeaturePoints();
    pair.restoreMotion();
    pair.rectify();
    //pair.sparseMapping();
    pair.denseMapping();
#endif

#ifdef GLU
    sv::PointCloud *pointCloud = &pair.pointCloud();
    sv::initGl("Point Cloud", pointCloud);
    return 0;
#endif

}