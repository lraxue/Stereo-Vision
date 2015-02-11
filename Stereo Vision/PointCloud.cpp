//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include <OpenGL/OpenGL.h>
#include "PointCloud.h"

namespace sv {
    const static int WIDTH = 640, HEIGHT = 480;
    const static float RATIO = float(WIDTH) / float(HEIGHT);

    static double prevMouseX = 0, prevMouseY = 0;
    static float elevation = 0, azimuth = 0, R = 1.0f;
    static bool isDragging = false;

    static void drawCube() {
        glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(1.0, 1.0, -1.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-1.0, 1.0, -1.0);
        glColor3f(0.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, 1.0);
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);

        glColor3f(1.0, 0.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(-1.0, -1.0, 1.0);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(-1.0, -1.0, -1.0);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(1.0, -1.0, -1.0);

        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glColor3f(0.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, 1.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(-1.0, -1.0, 1.0);
        glColor3f(1.0, 0.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);

        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(1.0, -1.0, -1.0);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(-1.0, -1.0, -1.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-1.0, 1.0, -1.0);
        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(1.0, 1.0, -1.0);

        glColor3f(0.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, 1.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-1.0, 1.0, -1.0);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(-1.0, -1.0, -1.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(-1.0, -1.0, 1.0);

        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(1.0, 1.0, -1.0);
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glColor3f(1.0, 0.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(1.0, -1.0, -1.0);
        glEnd();
    }

    static void drawAxis() {
        glLineWidth(3.0);
        glColor3f(1.0, 0.6, 0.0);

        glBegin(GL_LINES);
        // X-Axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(2.0, 0.0, 0.0);
        // Z-Axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 2.0);
        glEnd();

        glColor3f(0.0, 0.2, 1.0);
        glBegin(GL_LINES);
        // Y-Axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 2.0, 0.0);
        glEnd();
    }

    void drawPoints(PointCloud *data) {
        glPointSize(20.0 / R);
        glColor3f(1.0, 1.0, 1.0);

        glEnable(GL_POINT_SMOOTH);
        glBegin(GL_POINTS);
        for (int i = 0; i < data->size(); ++i) {
            glVertex3f((*data)[i].x, (*data)[i].y, (*data)[i].z);
        }
        glEnd();
        glDisable(GL_POINT_SMOOTH);
    }

    void onMouseClick(GLFWwindow *window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_RELEASE) isDragging = false;
            else if (action == GLFW_PRESS) {
                isDragging = true;
                glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
            }
        }
    }

    void onMouseDrag(GLFWwindow *window) {
        static double x, y;
        glfwGetCursorPos(window, &x, &y);
        azimuth += (prevMouseX - x) * 0.01;
        elevation += (y - prevMouseY) * 0.01;
        prevMouseX = x;
        prevMouseY = y;
    }

    void onMouseScroll(GLFWwindow *window, double xOffset, double yOffset) {
        R += yOffset * 0.1;
        R = R > 0.1 ? R : 0.1;
        R = R < 10 ? R : 10;
    }

    void initGl(std::string windowName, void *data) {
        ((PointCloud *)data)->reScale();

        GLFWwindow *window;
        /* Initialize the library */
        if (!glfwInit()) return;

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(WIDTH, HEIGHT, windowName.data(), NULL, NULL);
        if (!window) {
            glfwTerminate();
            return;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);
        std::cout << glGetString(GL_VERSION) << std::endl;

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClearDepth(1.0);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, RATIO, 1, 100.0);
        glMatrixMode(GL_MODELVIEW);

        glfwSetMouseButtonCallback(window, onMouseClick);
        glfwSetScrollCallback(window, onMouseScroll);
        render(window, data);
    }

    void render(GLFWwindow *window, void *data) {
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window)) {
            if (isDragging) onMouseDrag(window);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();
            glTranslatef(0, 0, -1);

            gluLookAt(R * cos(elevation) * sin(azimuth),
                    R * sin(elevation),
                    R * cos(elevation) * cos(azimuth),
                    0, 0, 0, 0, cos(elevation) > 0 ? 1 : -1, 0);

            drawAxis();
            //drawCube();
            drawPoints((PointCloud *)data);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }

    void PointCloud::reScale() {
        if (mPointCloud.size() == 0) return;

        cv::Point3f avg, delta;
        int n = mPointCloud.size();
        for (int i = 0; i < n; avg += mPointCloud[i], ++i);
        avg.x /= n, avg.y /= n, avg.z /= n;

        float scale = 0;
        for (int i = 0; i < n; ++i) {
            delta = avg - mPointCloud[i];
            scale = scale > fabsf(delta.x) ? scale : fabsf(delta.x);
            scale = scale > fabsf(delta.y) ? scale : fabsf(delta.y);
        }

        for (int i = 0; i < n; ++i) {
            mPointCloud[i].x = (mPointCloud[i].x - avg.x) / scale;
            /*
             -----------------> x in the photo coordinate;
             |                   ^
             |                   |
             |                   |
             |                   |
             v                   |
             y                   -----------------> x in the 3D coordinate
             */
            mPointCloud[i].y = (avg.y - mPointCloud[i].y) / scale;
            mPointCloud[i].z = (avg.z - mPointCloud[i].z) * 5;
            std::cout << mPointCloud[i] << std::endl;
        }
    }
}