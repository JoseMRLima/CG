#include "Group.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void Group::updateAnimations() {
    float deltaTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    for (auto &a : animations) {
        a->update(deltaTime);
    }
}

void Group::addAnimation(std::unique_ptr<Animation> a) {
    if (a)
        animations.push_back(std::move(a));
}

void Group::addTransformation(std::unique_ptr<Transformation> t) {
    if (t)
        transformations.push_back(std::move(t));
}

void Group::addModel(const Model &m) { models.push_back(m); }

void Group::addGroup(Group &&g) { groups.push_back(std::move(g)); }

void Group::draw() {
    glPushMatrix();

    updateAnimations();

    for (const auto &t : transformations) {
        t->apply();
    }

    for (const auto &m : models) {
        m.draw();
    }

    for (auto &g : groups) {
        g.draw();
    }

    glPopMatrix();
}

void Group::setup() {
    for (auto &m : models) {
        m.setup();
    }

    for (auto &g : groups) {
        g.setup();
    }
}
