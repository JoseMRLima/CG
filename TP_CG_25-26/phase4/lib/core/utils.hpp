/**
 * @file utils.hpp
 * @brief Utility function declarations for 3D graphics.
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include "Point.hpp"
#include "Vector.hpp"
#include <vector>

/**
 * @brief Generates VBO and IBO from a vector of points, optimizing for unique
 * vertices.
 *
 * @param points Input vector of points.
 * @param vbo Output vector for the Vertex Buffer Object (VBO).
 * @param ibo Output vector for the Index Buffer Object (IBO).
 */
void generateBuffers(const std::vector<Point> &points, std::vector<float> &vbo,
                     std::vector<unsigned int> &ibo);

/**
 * @brief Calculates a point and tangent on a Catmull-Rom spline.
 *
 * @param curve A vector of control points for the spline.
 * @param global_time The normalized time along the entire curve (0.0 to 1.0).
 * @return A pair containing the calculated Point and the tangent Vector.
 */
std::pair<Point, Vector> catmollRomPosition(std::vector<Point> curve,
                                            float global_time);

#endif