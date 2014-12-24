//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include "PointCloud.h"
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

namespace sv {
    void onPointcloudDraw(void *rotate) {
        double* rot = (double *) rotate;
        //  Clear screen and Z-buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // Reset transformations
        glLoadIdentity();

        // Other Transformations
        // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
        // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included

        // Rotate when user changes rotate_x and rotate_y
        glRotatef( rot[0], 1.0, 0.0, 0.0 );
        glRotatef( rot[1], 0.0, 1.0, 0.0 );

        // Other Transformations
        // glScalef( 2.0, 2.0, 0.0 );          // Not included

        //Multi-colored side - FRONT
        glBegin(GL_POLYGON);

        glColor3f( 1.0, 0.0, 0.0 );     glVertex3f(  0.5, -0.5, -0.5 );      // P1 is red
        glColor3f( 0.0, 1.0, 0.0 );     glVertex3f(  0.5,  0.5, -0.5 );      // P2 is green
        glColor3f( 0.0, 0.0, 1.0 );     glVertex3f( -0.5,  0.5, -0.5 );      // P3 is blue
        glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5, -0.5, -0.5 );      // P4 is purple

        glEnd();

        // White side - BACK
        glBegin(GL_POLYGON);
        glColor3f(   1.0,  1.0, 1.0 );
        glVertex3f(  0.5, -0.5, 0.5 );
        glVertex3f(  0.5,  0.5, 0.5 );
        glVertex3f( -0.5,  0.5, 0.5 );
        glVertex3f( -0.5, -0.5, 0.5 );
        glEnd();

        // Purple side - RIGHT
        glBegin(GL_POLYGON);
        glColor3f(  1.0,  0.0,  1.0 );
        glVertex3f( 0.5, -0.5, -0.5 );
        glVertex3f( 0.5,  0.5, -0.5 );
        glVertex3f( 0.5,  0.5,  0.5 );
        glVertex3f( 0.5, -0.5,  0.5 );
        glEnd();

        // Green side - LEFT
        glBegin(GL_POLYGON);
        glColor3f(   0.0,  1.0,  0.0 );
        glVertex3f( -0.5, -0.5,  0.5 );
        glVertex3f( -0.5,  0.5,  0.5 );
        glVertex3f( -0.5,  0.5, -0.5 );
        glVertex3f( -0.5, -0.5, -0.5 );
        glEnd();

        // Blue side - TOP
        glBegin(GL_POLYGON);
        glColor3f(   0.0,  0.0,  1.0 );
        glVertex3f(  0.5,  0.5,  0.5 );
        glVertex3f(  0.5,  0.5, -0.5 );
        glVertex3f( -0.5,  0.5, -0.5 );
        glVertex3f( -0.5,  0.5,  0.5 );
        glEnd();

        // Red side - BOTTOM
        glBegin(GL_POLYGON);
        glColor3f(   1.0,  0.0,  0.0 );
        glVertex3f(  0.5, -0.5, -0.5 );
        glVertex3f(  0.5, -0.5,  0.5 );
        glVertex3f( -0.5, -0.5,  0.5 );
        glVertex3f( -0.5, -0.5, -0.5 );
        glEnd();

        glFlush();
        glutSwapBuffers();
    }
}