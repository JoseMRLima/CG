#include "Group.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void Group::addTransformation(std::unique_ptr<Transformation> t) {
    if (t)
        transformations.push_back(std::move(t));
}

void Group::addModel(const Model &m) { models.push_back(m); }

void Group::addGroup(Group &&g) { groups.push_back(std::move(g)); }

void Group::draw() const {
    glPushMatrix();

    for (const auto &t : transformations) {
        t->apply();
    }
    
    for (const auto &m : models) {
        m.draw();
    }

    for (const auto &g : groups) {
        g.draw();
    }

    glPopMatrix();
}