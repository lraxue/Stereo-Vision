//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include "PointCloud.h"
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

namespace sv {
    void displayPointCloud() {
        glClear(GL_COLOR_BUFFER_BIT);
        glutInit(NULL, 0);
        glutCreateWindow("Point Cloud");
        glutDisplayFunc(drawPointCloud);
        glutMainLoop();
    }

    void drawPointCloud() {
    }
}