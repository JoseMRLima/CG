
#include "World.h"

void World::draw() const { group.draw(); }

void World::setWindow(const Window &w) { window = w; }

void World::setCamera(const Camera &c) { camera = c; }
