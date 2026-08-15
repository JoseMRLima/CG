/**
 * @file Model.cpp
 * @brief Implementation of the Model class for handling 3D models.
 *
 */

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Model.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <tuple>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool Model::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return false;
    }

    vertices.clear();
    normals.clear();
    uvs.clear();

    size_t count;
    file >> count;
    if (!file) {
        std::cerr << "Invalid file format (missing vertex count)\n";
        return false;
    }

    for (size_t i = 0; i < count; ++i) {
        Point p;
        Vector n(0, 0, 0);
        UV uv;

        file >> p.x >> p.y >> p.z >> n.x >> n.y >> n.z >> uv.u >> uv.v;

        if (!file) {
            std::cerr << "Invalid vertex data at index " << i << "\n";
            return false;
        }

        vertices.push_back(p);
        normals.push_back(n);
        uvs.push_back(uv);
    }

    return true;
}

void Model::addTriangule(std::vector<Point> ps, std::vector<Vector> ns,
                         std::vector<UV> texts) {
    for (size_t i = 0; i < ps.size(); ++i) {
        vertices.push_back(ps[i]);
        normals.push_back(ns[i]);
        uvs.push_back(texts[i]);
    }
}

bool Model::toFile(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return false;
    }

    file << vertices.size() << "\n";

    for (size_t i = 0; i < vertices.size(); ++i) {
        const Point &p = vertices[i];
        const Vector &n = normals[i];
        const UV &uv = uvs[i];

        file << p.x << " " << p.y << " " << p.z << "  " << n.x << " " << n.y
             << " " << n.z << "  " << uv.u << " " << uv.v << "\n";
    }

    file.close();
    return true;
}

void Model::draw() const {
    if (ibo.empty())
        return;

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material.specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material.emissive);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);

    const bool hasTexture = (_texture != 0);
    if (hasTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    constexpr GLsizei stride = 8 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);

    // positions
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, stride, reinterpret_cast<void *>(0));

    // normals
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, stride,
                    reinterpret_cast<void *>(3 * sizeof(float)));

    // tex coords
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, stride,
                      reinterpret_cast<void *>(6 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ibo.size()),
                   GL_UNSIGNED_INT, 0);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    if (hasTexture) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void Model::loadTexture() {
    if (textureFile.empty())
        return;

    // stb_image loads top-to-bottom; OpenGL expects bottom-to-top.
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char *data =
        stbi_load(textureFile.c_str(), &width, &height, &channels, STBI_rgb);

    if (!data) {
        std::cerr << "Model::loadTexture — failed to load: " << textureFile
                  << "\n";
        return;
    }

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

using VertexKey =
    std::tuple<float, float, float, float, float, float, float, float>;

void Model::setup() {
    vbo.clear();
    ibo.clear();

    std::map<VertexKey, unsigned int> cache;

    for (size_t i = 0; i < vertices.size(); ++i) {
        const Point &p = vertices[i];
        const Vector &n = normals[i];
        const UV &uv = uvs[i];

        VertexKey key = {p.x, p.y, p.z, n.x, n.y, n.z, uv.u, uv.v};

        auto it = cache.find(key);
        if (it != cache.end()) {
            // reuse existing vertex
            ibo.push_back(it->second);
        } else {
            // new unique vertex — append to VBO
            unsigned int idx = static_cast<unsigned int>(vbo.size() / 8);
            vbo.push_back(p.x);
            vbo.push_back(p.y);
            vbo.push_back(p.z);
            vbo.push_back(n.x);
            vbo.push_back(n.y);
            vbo.push_back(n.z);
            vbo.push_back(uv.u);
            vbo.push_back(uv.v);

            cache[key] = idx;
            ibo.push_back(idx);
        }
    }

    constexpr GLsizei stride = 8 * sizeof(float);

    // upload vertex buffer
    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(float) * vbo.size()),
                 vbo.data(), GL_STATIC_DRAW);

    // upload index buffer
    glGenBuffers(1, &this->_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(unsigned int) * ibo.size()),
                 ibo.data(), GL_STATIC_DRAW);

    loadTexture();
}
