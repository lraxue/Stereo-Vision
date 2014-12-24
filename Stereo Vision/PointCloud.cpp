//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include "PointCloud.h"
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

namespace sv {
    void initGl(std::string window, void *data) {
        cv::namedWindow(window, cv::WINDOW_OPENGL);
        cv::resizeWindow(window, 1200, 800);
        cv::setOpenGlContext(window);

        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glEnable(GL_DEPTH_TEST);
        //glClearColor(0, 0xff, 0xff, 0.6);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);

        cv::setOpenGlDrawCallback(window, sv::onDraw, data);
    }

    void onDraw(void *data) {
        PointCloud *pointCloud = (PointCloud *)data;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();

        glRotatef( pointCloud->rotateX(), 1.0, 0.0, 0.0 );
        glRotatef( pointCloud->rotateY(), 0.0, 1.0, 0.0 );

        // Other Transformations
        glScalef( 1.0f / 400, 1.0f / 400, 1.0f / 20 );          // Not included

        glPointSize(5);
        glColor3f(1.0, 0, 0);
        glBegin(GL_POINTS);
        for (int i = 0; i < pointCloud->size(); ++i) {
            glVertex3f((*pointCloud)(i).x, (*pointCloud)(i).y, (*pointCloud)(i).z);
        }
        glEnd();

        glFlush();
        glutSwapBuffers();
    }
}