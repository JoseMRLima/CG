/**
 * @file Transformation.cpp
 * @brief Implementation of the Transformation classes.
 *
 */

#include "Transformation.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Translate::Translate(float x, float y, float z) : x(x), y(y), z(z) {}

void Translate::apply() const { glTranslatef(x, y, z); }

Rotate::Rotate(float angle, float x, float y, float z)
    : angle(angle), x(x), y(y), z(z) {}

void Rotate::apply() const { glRotatef(angle, x, y, z); }

Scale::Scale(float x, float y, float z) : x(x), y(y), z(z) {}

void Scale::apply() const { glScalef(x, y, z); }
