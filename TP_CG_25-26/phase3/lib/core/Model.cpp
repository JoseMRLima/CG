/* Model.cpp */

#include "Model.hpp"
#include "utils.hpp"
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

    for (Point &p : vertices) {
        p.toFile(file);
    }

    file.close();

    return true;
}

void Model::draw() const {
    if (ibo.empty())
        return;

    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glDrawElements(GL_TRIANGLES, (GLsizei)this->ibo.size(), GL_UNSIGNED_INT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void Model::setup(){
    generateBuffers(this->vertices, this->vbo, this->ibo);

    // generate and bind vertex buffer
    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->vbo.size(),
                 this->vbo.data(), GL_STATIC_DRAW);

    // generate and bind index buffer
    glGenBuffers(1, &this->_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * this->ibo.size(), this->ibo.data(),
                 GL_STATIC_DRAW);
}
