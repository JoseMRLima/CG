#include "Group.h"
#include <utility>

void Group::addModel(const Model &m) { models.push_back(m); }

void Group::draw() const {
    for (const auto &m : models) {
        m.draw();
    }
}