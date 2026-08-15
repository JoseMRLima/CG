#include "Vector.hpp"

#include <cmath>

Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

Vector Vector::operator*(int n) const {
    return {x * float(n), y * float(n), z * float(n)};
}

Vector Vector::operator*(float n) const { return {x * n, y * n, z * n}; }

Vector Vector::normalize() const {
    float norm = std::sqrt(x * x + y * y + z * z);
    if (norm == 0.0f)
        return {0.0f, 0.0f, 0.0f};
    return {x / norm, y / norm, z / norm};
}

Vector Vector::cross(const Vector &other) const {
    return {y * other.z - z * other.y, z * other.x - x * other.z,
            x * other.y - y * other.x};
}