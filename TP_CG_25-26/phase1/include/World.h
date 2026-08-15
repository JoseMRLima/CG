#pragma once

#include "Group.h"
#include "Point.h"

#include <string>

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
    Point lookAt;
    Point up;

    float fov = 60.0f;
    float near = 1.0f;
    float far = 1000.0f;
};

class World {
  private:
    Window window;
    Camera camera;
    Group group;

    bool parseWindow(tinyxml2::XMLElement *windowElem);
    bool parseCamera(tinyxml2::XMLElement *cameraElem);
    bool parseGroup(tinyxml2::XMLElement *groupElem);
    bool parseModels(tinyxml2::XMLElement *modelsElem);

  public:
    bool loadFromFile(const std::string &filename);
    void draw() const;

    const Window &getWindow() const { return window; }
    const Camera &getCamera() const { return camera; }
};
