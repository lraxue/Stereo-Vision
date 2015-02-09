//
// Created by Neo on 14-12-19.
// Copyright (c) 2014 PKU. All rights reserved.
//

#include <OpenGL/OpenGL.h>
#include "PointCloud.h"

namespace sv {
    const static int WIDTH = 640, HEIGHT = 480;
    const static float RATIO = float(WIDTH) / float(HEIGHT), R = 1.0f;

    static double prevMouseX = 0, prevMouseY = 0;
    static float elevation = 0, azimuth = 0;
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
        // Y-Axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 2.0, 0.0);
        // Z-Axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 2.0);
        glEnd();
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

    void initGl(std::string windowName, void *data) {
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
        render(window, data);
    }

    void render(GLFWwindow *window, void *data) {
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window)) {
            if (isDragging) onMouseDrag(window);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();
            glTranslatef(0, 0, -5);

            gluLookAt(R * cos(elevation) * sin(azimuth),
                    R * sin(elevation),
                    R * cos(elevation) * cos(azimuth),
                    0, 0, 0, 0, cos(elevation) > 0 ? 1 : -1, 0);

            drawAxis();
            drawCube();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }
}