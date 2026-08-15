/* Model.cpp */

#include "Model.h"
#include <fstream>
#include <iostream>

bool Model::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return false;
    }

    vertices.clear();

    size_t count;
    file >> count;

    if (!file) {
        std::cerr << "Invalid file format (missing vertex count)\n";
        return false;
    }

    for (size_t i = 0; i < count; ++i) {
        Point v;
        file >> v.x >> v.y >> v.z;

        if (!file) {
            std::cerr << "Invalid vertex data at index " << i << "\n";
            return false;
        }

        vertices.push_back(v);
    }

    return true;
}

void Model::addTriangule(std::vector<Point> ps) {
    for (const Point &p : ps) {
        vertices.push_back(p);
    }
}

bool Model::toFile(std::string &filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return false;
    }

    // write the number of vertices
    file << vertices.size() << "\n";

    for (Point &p: vertices) {
        p.toFile(file);
    }

    file.close();

    return true;
}

// TODO: change it
void Model::draw(GLenum mode) const {
    if (vertices.empty())
        return;

    glBegin(mode);

    for (const auto &v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }

    glEnd();
}
