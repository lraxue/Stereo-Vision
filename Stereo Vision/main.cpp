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
    //pair.disparity();
#endif

#ifdef GL
    namedWindow("OpenGL-Test", WINDOW_OPENGL);
    resizeWindow("OpenGL-Test", 640, 480);
    setOpenGlContext("OpenGL-Test");

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glEnable(GL_DEPTH_TEST);

    setOpenGlDrawCallback("OpenGL-Test", sv::onOpenGlDraw, NULL);
    updateWindow("OpenGL-Test");
    waitKey(-1);
#endif

    return 0;
}