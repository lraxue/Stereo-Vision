//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include "PointCloud.h"
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

namespace sv {
    void onOpenGlDraw(void *param) {
        //  Clear screen and Z-buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // Reset transformations
        glLoadIdentity();

        glBegin(GL_POINTS);
            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(1.0, 1.0, 1.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 0.5, 0.0);
        glEnd();

        glFlush();
        glutSwapBuffers();
    }
}