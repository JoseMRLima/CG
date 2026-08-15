#ifndef UTILS_H
#define UTILS_H

#include "Point.hpp"
#include "Vector.hpp"
#include <vector>

void generateBuffers(const std::vector<Point> &points, std::vector<float> &vbo,
                     std::vector<unsigned int> &ibo);

std::pair<Point, Vector> catmollRomPosition(std::vector<Point> curve,
                                           float global_time);

#endif