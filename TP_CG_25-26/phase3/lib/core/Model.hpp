/* Model.h */

#pragma once

#include "Point.hpp"

#include <string>
#include <vector>

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Model {
  private:
    std::vector<Point> vertices;
    std::vector<float> vbo;
    std::vector<unsigned int> ibo;
    GLuint _vbo;
    GLuint _ibo;

  public:
    Model() = default;

    bool loadFromFile(const std::string &filename);
    void addTriangule(std::vector<Point> ps);
    bool toFile(std::string &filename);
    void draw() const;
    void setup();
};
