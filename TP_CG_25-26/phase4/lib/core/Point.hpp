/**
 * @file Point.hpp
 * @brief Defines a 3D Point class with basic vector operations.
 */

#ifndef POINT_HPP
#define POINT_HPP

#include "Vector.hpp"
#include <fstream>
#include <iostream>

/**
 * @brief Represents a point in 3D space.
 */
struct Point {
    float x, y, z;

    /**
     * @brief Default constructor. Initializes point to (0,0,0).
     */
    Point();

    /**
     * @brief Constructs a Point with specified coordinates.
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     * @param z The z-coordinate.
     */
    Point(float x, float y, float z);

    /**
     * @brief Translates the point by a vector.
     * @param v The vector to add.
     * @return The new translated point.
     */
    Point operator+(const Vector &v) const;

    /**
     * @brief Translates the point by the inverse of a vector.
     * @param v The vector to subtract.
     * @return The new translated point.
     */
    Point operator-(const Vector &v) const;

    /**
     * @brief Scales the point's coordinates by a scalar.
     * @param scalar The scalar value to multiply by.
     * @return The new scaled point.
     */
    Point operator*(float scalar) const;

    /**
     * @brief Compares two points for sorting purposes.
     * @param other The other point to compare with.
     * @return True if this point is less than the other.
     */
    bool operator<(const Point &other) const;

    /**
     * @brief Writes the point's coordinates to a file.
     * @param file The output file stream.
     */
    void toFile(std::ofstream &file) const;
};

/**
 * @brief Overloads the stream insertion operator for easy printing of Point
 * objects.
 * @param os The output stream.
 * @param p The point to print.
 * @return The output stream with the point's data.
 */
std::ostream &operator<<(std::ostream &os, const Point &p);

#endif