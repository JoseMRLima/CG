#pragma once

#include "tinyxml2.h"

#include "Vector.hpp"

#include <fstream>
#include <iostream>

struct Point {
    float x, y, z;

    Point();
    Point(float x_, float y_, float z_);

    Point operator+(const Vector &v) const;
    Point operator-(const Vector &v) const;
    Point operator*(float scalar) const;
    bool operator<(const Point& other) const;

    static Point fromXML(const tinyxml2::XMLElement *elem);
    void toFile(std::ofstream &file);
};

std::ostream &operator<<(std::ostream &os, const Point &p);