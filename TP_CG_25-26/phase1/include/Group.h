#pragma once

#include "Model.h"

#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Group {
  private:
    std::vector<Model> models;

  public:
    void addModel(const Model &m);
    void draw() const;
};
