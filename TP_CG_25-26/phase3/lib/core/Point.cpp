#include "Point.hpp"

Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}

Point::Point() : Point(0.0f, 0.0f, 0.0f) {}

Point Point::fromXML(const tinyxml2::XMLElement *elem) {
    Point p;

    if (!elem)
        return p;

    elem->QueryFloatAttribute("x", &p.x);
    elem->QueryFloatAttribute("y", &p.y);
    elem->QueryFloatAttribute("z", &p.z);

    return p;
}

Point Point::operator+(const Vector &v) const {
    return {x + v.x, y + v.y, z + v.z};
}

Point Point::operator-(const Vector &v) const {
    return {x - v.x, y - v.y, z - v.z};
}

Point Point::operator*(const float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
}

bool Point::operator<(const Point &other) const {
    if (x != other.x)
        return x < other.x;
    if (y != other.y)
        return y < other.y;
    return z < other.z;
}

void Point::toFile(std::ofstream &file) {
    file << x << " " << y << " " << z << "\n";
}

std::ostream &operator<<(std::ostream &os, const Point &p) {
    os << "Point(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}