/* engine.cpp */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include "World.h"

#include <iostream>
#include <string>
#include <vector>

static float alpha = 0.5f;
static float beta = 0.5f;
static float radius = 10.0f;

static World w;

static bool fixedCam = true;

static void usage(char *command) {
    std::cerr << "Usage: " << command << " <config.xml>\n";
    std::cerr << "> config.xml: configuration file with window, camera "
                 "settings, lights, transformations and models\n";
}

void changeSize(int width, int height) {
    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if (height == 0)
        height = 1;

    // compute window's aspect ratio
    float ratio = width * 1.0 / height;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, width, height);

    // Set perspective
    Camera cam = w.getCamera();
    gluPerspective(cam.fov, ratio, cam.near, cam.far);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();

    if (fixedCam) {
        Camera cam = w.getCamera();
        gluLookAt(cam.position.x, cam.position.y, cam.position.z, cam.lookAt.x,
                  cam.lookAt.y, cam.lookAt.z, cam.up.x, cam.up.y, cam.up.z);
    } else {
        float camX = radius * cos(beta) * sin(alpha);
        float camY = radius * sin(beta);
        float camZ = radius * cos(beta) * cos(alpha);

        gluLookAt(camX, camY, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }

    // draw the x y z axis
    glBegin(GL_LINES);

    // X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);

    // Y Axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    // Z Axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);

    glEnd();

    // change to lines
    glPolygonMode(GL_FRONT, GL_LINE);

    // change color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    // draw world
    w.draw();

    // End of frame
    glutSwapBuffers();
}

void processSpecialKeys(int key, int xx, int yy) {
    float angleStep = 0.05f;

    switch (key) {
        case GLUT_KEY_LEFT:
            alpha -= angleStep;
            break;

        case GLUT_KEY_RIGHT:
            alpha += angleStep;
            break;

        case GLUT_KEY_UP:
            beta += angleStep;
            if (beta > (M_PI / 2))
                beta = M_PI / 2;
            break;

        case GLUT_KEY_DOWN:
            beta -= angleStep;
            if (beta < -(M_PI / 2))
                beta = -(M_PI / 2);
            break;
    }

    glutPostRedisplay();
}

void processKeys(unsigned char c, int xx, int yy) {
    float zoomStep = 0.5f;

    if (c == 'w')
        radius -= zoomStep;
    else if (c == 's')
        radius += zoomStep;

    if (radius < 1.0f)
        radius = 1.0f;

    // lock/unlock camera
    if (c == 'l')
        fixedCam = !fixedCam;

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    if (!w.loadFromFile(argv[1])) {
        std::cerr << "Error reading config file.\n";
        return EXIT_FAILURE;
    }

    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    Window wind = w.getWindow();
    glutInitWindowSize(wind.width, wind.height);
    glutCreateWindow("Solar System");

    // Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
