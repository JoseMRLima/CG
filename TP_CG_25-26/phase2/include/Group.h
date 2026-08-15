#pragma once

#include "Model.h"
#include "Transformation.h"

#include <memory>
#include <vector>

class Group {
  private:
    std::vector<Model> models;
    std::vector<Group> groups;
    std::vector<std::unique_ptr<Transformation>> transformations;

  public:
    Group() = default;
    Group(Group &&) noexcept = default;
    Group &operator=(Group &&) noexcept = default;

    void addTransformation(std::unique_ptr<Transformation> t);
    
    void addModel(const Model &m);
    void addGroup(Group &&g);

    void draw() const;
};