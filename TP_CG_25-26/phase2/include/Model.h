/* Model.h */

#pragma once

#include "Point.h"

#include <string>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Model {
  private:
    std::vector<Point> vertices;

  public:
    Model() = default;

    bool loadFromFile(const std::string &filename);
    void addTriangule(std::vector<Point> ps);
    bool toFile(std::string &filename);
    void draw(GLenum mode = GL_TRIANGLES) const;
};
