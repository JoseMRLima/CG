/**
 * @file main.cpp
 * @brief Main entry point for the 3D scene rendering application.
 *
 */

#ifndef __APPLE__
#include <GL/glew.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include "SceneParser.hpp"
#include "World.hpp"

#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Global World object that holds the entire scene graph and settings.
 *
 */
static World w;

/**
 * @brief Time base for calculating frames per second (FPS).
 *
 */
static int timebase;

/**
 * @brief Prints the command-line usage instructions.
 *
 * @param command The name of the executable.
 */
static void usage(char *command) {
    std::cerr << "Usage: " << command << " <config.xml>\n";
    std::cerr << "> config.xml: configuration file with window, camera "
                 "settings, lights, transformations and models\n";
}

/**
 * @brief GLUT callback function for window reshape events.
 *
 * @param width The new window width.
 * @param height The new window height.
 */
static void changeSize(int width, int height) {
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
    Camera &cam = w.getCamera();
    gluPerspective(cam.fov, ratio, cam.near, cam.far);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Calculates and prints the frames per second (FPS).
 *
 */
static void frameCounter() {
    static int frame = 0;
    static float time = 0;
    static float fps = 0;

    frame++;
    time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        fps = frame * 1000.0 / (time - timebase);
        timebase = time;
        frame = 0;
    }

    static int i = 0;
    i++;
    if (i % 100 == 0)
        std::cout << "FPS: " << std::to_string(fps) << std::endl;
}

/**
 * @brief Draws the X, Y, and Z axes for orientation.
 *
 */
static void drawAxis(void) {
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
}

/**
 * @brief GLUT display callback function to render the entire scene.
 *
 */
static void renderScene(void) {
    // clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    Camera &cam = w.getCamera();
    gluLookAt(cam.position.x, cam.position.y, cam.position.z, cam.lookAt.x,
              cam.lookAt.y, cam.lookAt.z, cam.up.x, cam.up.y, cam.up.z);

    // draw xyz axis (disable lighting so glColor3f works on the axes)
    glDisable(GL_LIGHTING);
    drawAxis();
    glEnable(GL_LIGHTING);

    // change to lines
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // ensure lighting state is always correct each frame
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // draw world
    w.draw();

    frameCounter();

    // end of frame
    glutSwapBuffers();
}

/**
 * @brief GLUT callback for mouse button events. Handles camera tracking.
 *
 * @param button The mouse button that was pressed or released.
 * @param state The state of the button (GLUT_DOWN or GLUT_UP).
 * @param xx The x-coordinate of the mouse cursor.
 * @param yy The y-coordinate of the mouse cursor.
 */
static void processMouseButtons(int button, int state, int xx, int yy) {
    Camera &cam = w.getCamera();

    if (state == GLUT_DOWN) {
        cam.startX = xx;
        cam.startY = yy;
        if (button == GLUT_LEFT_BUTTON)
            cam.tracking = 1;
        else if (button == GLUT_RIGHT_BUTTON)
            cam.tracking = 2;
        else
            cam.tracking = 0;
    } else if (state == GLUT_UP) {
        if (cam.tracking == 1) {
            cam.alpha += (xx - cam.startX);
            cam.beta += (yy - cam.startY);
        } else if (cam.tracking == 2) {
            cam.r -= yy - cam.startY;
            if (cam.r < 3)
                cam.r = 3.0;
        }
        cam.tracking = 0;
    }
}

/**
 * @brief GLUT callback for mouse motion events. Updates camera position.
 *
 * @param xx The x-coordinate of the mouse cursor.
 * @param yy The y-coordinate of the mouse cursor.
 */
static void processMouseMotion(int xx, int yy) {
    Camera &cam = w.getCamera();

    int deltaX, deltaY;
    int alphaAux, betaAux;
    int rAux;

    if (!cam.tracking)
        return;

    deltaX = xx - cam.startX;
    deltaY = yy - cam.startY;

    if (cam.tracking == 1) {
        alphaAux = cam.alpha + deltaX;
        betaAux = cam.beta + deltaY;

        if (betaAux > 85.0)
            betaAux = 85.0;
        else if (betaAux < -85.0)
            betaAux = -85.0;

        rAux = cam.r;
    } else if (cam.tracking == 2) {
        alphaAux = cam.alpha;
        betaAux = cam.beta;
        rAux = cam.r - deltaY;
        if (rAux < 3)
            rAux = 3;
    }
    cam.position.x =
        rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    cam.position.z =
        rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    cam.position.y = rAux * sin(betaAux * 3.14 / 180.0);
}

/**
 * @brief GLUT callback for special keyboard key events.
 *
 * @param key The special key code.
 * @param xx The x-coordinate of the mouse cursor.
 * @param yy The y-coordinate of the mouse cursor.
 */
static void processSpecialKeys(int key, int xx, int yy) {
    (void)key;
    (void)xx;
    (void)yy;
    // process special keys
}

/**
 * @brief GLUT callback for standard keyboard key events.
 *
 * @param c The ASCII code of the key.
 * @param xx The x-coordinate of the mouse cursor.
 * @param yy The y-coordinate of the mouse cursor.
 */
static void processKeys(unsigned char c, int xx, int yy) {
    (void)c;
    (void)xx;
    (void)yy;
    // process keys
}

/**
 * @brief Main entry point for the application.
 *
 * Initializes GLUT, parses the scene file, sets up callbacks and OpenGL, and
 * enters the main loop.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Exit status.
 */
int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    if (!SceneParser::loadWorld(argv[1], w)) {
        std::cerr << "Error reading config file.\n";
        return EXIT_FAILURE;
    }

    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    Window wind = w.getWindow();
    glutInitWindowSize(wind.width, wind.height);
    glutCreateWindow("Solar System");

    glewInit();

    // setup world VBOs
    w.setup();

    // Required callback registry
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_RESCALE_NORMAL);

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
