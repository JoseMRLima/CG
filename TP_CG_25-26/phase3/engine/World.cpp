
#include "World.hpp"

void World::draw() { group.draw(); }

void World::setWindow(const Window &w) { window = w; }

void World::setCamera(const Camera &c) { camera = c; }

void World::setup() { group.setup(); }
