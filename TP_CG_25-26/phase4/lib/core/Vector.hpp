/**
 * @file Vector.hpp
 * @brief Defines a 3D Vector class with basic mathematical operations.
 *
 */

#ifndef VECTOR_HPP
#define VECTOR_HPP

/**
 * @brief Represents a vector in 3D space.
 */
struct Vector {
  public:
    float x, y, z;

    /**
     * @brief Constructs a Vector with specified components.
     * @param x The x-component.
     * @param y The y-component.
     * @param z The z-component.
     */
    Vector(float x, float y, float z);

    /**
     * @brief Scales the vector by a scalar value.
     * @param scalar The scalar to multiply the vector by.
     * @return The new scaled vector.
     */
    Vector operator*(float scalar) const;

    /**
     * @brief Computes the normalized version of the vector.
     * @return The normalized vector (unit vector).
     */
    Vector normalize() const;

    /**
     * @brief Computes the cross product with another vector.
     * @param other The other vector.
     * @return The vector resulting from the cross product.
     */
    Vector cross(const Vector &other) const;
};

#endif