/* Vector.h */

#pragma once

struct Vector {
  public:
    float x, y, z;

    Vector(float x_, float y_, float z_);

    Vector operator*(float scalar) const;
    Vector operator*(int scalar) const;

    Vector normalize() const;
    Vector cross(const Vector &other) const;
};
