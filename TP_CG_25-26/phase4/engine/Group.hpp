/**
 * @file Group.hpp
 * @brief Defines the Group class, a container for models and other groups in a
 * scene graph.
 *
 */

#ifndef GROUP_HPP
#define GROUP_HPP

#include "Animation.hpp"
#include "Model.hpp"
#include "Transformation.hpp"
#include <memory>
#include <vector>

/**
 * @brief Represents a group of models and subgroups, forming a node in a scene
 * graph.
 */
class Group {
  private:
    std::vector<Model> models; ///< A list of models belonging to this group.
    std::vector<Group> groups; ///< A list of subgroups.
    std::vector<std::unique_ptr<Animation>>
        animations; ///< A list of animations to be applied.
    std::vector<std::unique_ptr<Transformation>>
        transformations; ///< A list of transformations to be applied.

  public:
    /**
     * @brief Default constructor.
     */
    Group() = default;
    /**
     * @brief Default move constructor.
     */
    Group(Group &&) noexcept = default;
    /**
     * @brief Default move assignment operator.
     */
    Group &operator=(Group &&) noexcept = default;

    /**
     * @brief Adds an animation to the group.
     * @param a A unique pointer to an Animation object.
     */
    void addAnimation(std::unique_ptr<Animation> a);
    /**
     * @brief Adds a transformation to the group.
     * @param t A unique pointer to a Transformation object.
     */
    void addTransformation(std::unique_ptr<Transformation> t);

    /**
     * @brief Adds a model to the group.
     * @param m The model to be added.
     */
    void addModel(const Model &m);
    /**
     * @brief Adds a subgroup to this group.
     * @param g The group to be added, moved into the list.
     */
    void addGroup(Group &&g);

    /**
     * @brief Draws all models and subgroups, applying transformations and
     * animations.
     */
    void draw();
    /**
     * @brief Sets up the VBOs for all models in this group and its subgroups.
     */
    void setup();

  private:
    /**
     * @brief Updates all animations based on the elapsed time.
     */
    void updateAnimations();
};

#endif