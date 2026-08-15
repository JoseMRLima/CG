#include "Vector.h"

Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

Vector Vector::operator*(int n) const {
    return {x * float(n), y * float(n), z * float(n)};
}

Vector Vector::operator*(float n) const { return {x * n, y * n, z * n}; }
