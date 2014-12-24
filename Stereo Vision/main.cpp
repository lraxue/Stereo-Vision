//
//  main.cpp
//  Stereo Vision
//
//  Created by Neo on 14-11-28.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

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
    Mat imgLeft = imread(dirPath + "B00.jpg", CV_64FC3),
        imgRight = imread(dirPath + "B01.jpg", CV_64FC3);

    sv::MonoView l = sv::MonoView(imgLeft), r = sv::MonoView(imgRight);

    l.extractFeaturePoints();
    r.extractFeaturePoints();

    sv::StereoPair pair = sv::StereoPair(&l, &r);
    pair.matchFeaturePoints();
    pair.restoreMotion();
    pair.rectify();
    pair.sparseMapping();
#endif

#ifdef GLU
    sv::PointCloud *pointCloud = &pair.pointCloud();

    sv::initGl("PointCloud", pointCloud);
    for (;;)
    {
        int key = waitKey(20);
        if ((key & 0xff) == 27)
            break;
        switch (key) {
            case 123:
                pointCloud->rotateX(-5); break;
            case 124:
                pointCloud->rotateX(5); break;
            case 125:
                pointCloud->rotateY(-5); break;
            case 126:
                pointCloud->rotateY(5); break;
        }
        updateWindow("PointCloud");
    }

#endif

    return 0;
}