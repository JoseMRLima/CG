#pragma once

#include <string>

#include "Group.h"
#include "Point.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

struct Window {
    int width = 800;
    int height = 600;
};

struct Camera {
    Point position;
    Point lookAt = {0.0f, 0.0f, 0.0f};
    Point up = {0.0f, 1.0f, 0.0f};
    int tracking = 0;
    int startX = 0, startY = 0;
    int alpha = 0, beta = 45, r = 50;

    float fov = 60.0f;
    float near = 1.0f;
    float far = 1000.0f;
};

class World {
  private:
    Window window;
    Camera camera;
    Group group;

  public:
    void draw() const;

    void setWindow(const Window &w);
    void setCamera(const Camera &c);

    Group &getGroup() { return group; }
    const Window &getWindow() const { return window; }
    Camera &getCamera() { return camera; }
};
