/**
 * @file World.cpp
 * @brief Definition of the World class, which encapsulates the entire 3D scene.
 *
 */

#include "World.hpp"

void World::draw() {
    for (int i = 0; i < static_cast<int>(lights.size()) && i < 8; ++i)
        lights[i].setup(i);

    group.draw();
}

void World::setWindow(const Window &w) { window = w; }

void World::setCamera(const Camera &c) { camera = c; }

void World::setup() {
    Light::enableLighting();
    group.setup();
}

void World::addLight(const Light &l) {
    if (lights.size() >= 8)
        std::cerr << "World::addLight - OpenGL supports at most 8 lights; "
                     "this light will be ignored\n";
    else
        lights.push_back(l);
}
