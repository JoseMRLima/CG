/**
 * @file Animation.hpp
 * @brief Defines an abstract base class for animations and its concrete
 * implementations.
 *
 */

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "Point.hpp"
#include <array>
#include <utility>
#include <vector>

/**
 * @brief Abstract base class for all animations.
 */
class Animation {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Animation() = default;
    /**
     * @brief Updates the animation state based on the elapsed time.
     * @param deltaTime The total elapsed time since the start of the program.
     */
    virtual void update(float deltaTime) = 0;
};

/**
 * @brief Represents a translation animation along a Catmull-Rom curve over a
 * specific duration.
 */
class TimedTranslation : public Animation {
  private:
    float time; ///< The total time the translation should take.
    std::vector<Point>
        curvePoints; ///< Control points for the Catmull-Rom curve.
    bool aligned; ///< If true, aligns the object's orientation with the curve's
                  ///< tangent.

    /**
     * @brief Calculates the position and derivative (tangent) on the
     * Catmull-Rom curve for a given time.
     * @param elapsed_time The time elapsed along the curve's duration.
     * @return A pair containing the position (Point) and the tangent (Vector).
     */
    std::pair<Point, Vector> getLocation(float elapsed_time);
    /**
     * @brief Renders the Catmull-Rom curve for visualization.
     */
    void renderCatmullRomCurve();
    /**
     * @brief Creates a rotation matrix from three basis vectors.
     * @param x The x-axis vector.
     * @param y The y-axis vector.
     * @param z The z-axis vector.
     * @return A 16-element array representing the 4x4 rotation matrix.
     */
    std::array<float, 16> rotationMatrix(Vector x, Vector y, Vector z);

  public:
    /**
     * @brief Constructs a TimedTranslation object.
     * @param time The duration of the translation.
     * @param curvePoints The control points defining the path.
     * @param aligned Whether to align the object with the path direction.
     */
    TimedTranslation(float time, std::vector<Point> curvePoints, bool aligned);
    /**
     * @brief Applies the translation and optional alignment based on the
     * elapsed time.
     * @param deltaTime The total elapsed time.
     */
    void update(float deltaTime) override;
};

/**
 * @brief Represents a continuous rotation animation over a specific duration.
 */
class TimedRotation : public Animation {
  private:
    float time, x, y, z; ///< Duration for a full 360-degree rotation and the
                         ///< axis of rotation.

  public:
    /**
     * @brief Constructs a TimedRotation object.
     * @param time The time in seconds for one full rotation.
     * @param x The x-component of the rotation axis.
     * @param y The y-component of the rotation axis.
     * @param z The z-component of the rotation axis.
     */
    TimedRotation(float time, float x, float y, float z);
    /**
     * @brief Applies the rotation based on the elapsed time.
     * @param deltaTime The total elapsed time.
     */
    void update(float deltaTime) override;
};

#endif