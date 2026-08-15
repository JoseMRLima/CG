#pragma once

#include "Animation.hpp"
#include "Model.hpp"
#include "Transformation.hpp"

#include <memory>
#include <vector>

class Group {
  private:
    std::vector<Model> models;
    std::vector<Group> groups;
    std::vector<std::unique_ptr<Animation>> animations;
    std::vector<std::unique_ptr<Transformation>> transformations;

  public:
    Group() = default;
    Group(Group &&) noexcept = default;
    Group &operator=(Group &&) noexcept = default;

    void addAnimation(std::unique_ptr<Animation> a);
    void addTransformation(std::unique_ptr<Transformation> t);

    void addModel(const Model &m);
    void addGroup(Group &&g);

    void draw();
    void setup();

  private:
    void updateAnimations();
};