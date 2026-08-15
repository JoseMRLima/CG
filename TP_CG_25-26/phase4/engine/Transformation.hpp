/**
 * @file Transformation.hpp
 * @brief Defines an abstract base class for transformations and its concrete
 * implementations.
 *
 */

#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

/**
 * @brief Abstract base class for all geometric transformations.
 */
class Transformation {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Transformation() = default;
    /**
     * @brief Applies the transformation. This is a pure virtual function.
     */
    virtual void apply() const = 0;
};

/**
 * @brief Represents a translation transformation.
 */
class Translate : public Transformation {
    float x, y, z;

  public:
    /**
     * @brief Constructs a Translate object.
     * @param x Translation factor on the x-axis.
     * @param y Translation factor on the y-axis.
     * @param z Translation factor on the z-axis.
     */
    Translate(float x, float y, float z);
    /**
     * @brief Applies the translation.
     */
    void apply() const override;
};

/**
 * @brief Represents a rotation transformation.
 */
class Rotate : public Transformation {
    float angle, x, y, z;

  public:
    /**
     * @brief Constructs a Rotate object.
     * @param angle The angle of rotation in degrees.
     * @param x The x-component of the rotation axis.
     * @param y The y-component of the rotation axis.
     * @param z The z-component of the rotation axis.
     */
    Rotate(float angle, float x, float y, float z);
    /**
     * @brief Applies the rotation.
     */
    void apply() const override;
};

/**
 * @brief Represents a scaling transformation.
 */
class Scale : public Transformation {
    float x, y, z;

  public:
    /**
     * @brief Constructs a Scale object.
     * @param x Scaling factor on the x-axis.
     * @param y Scaling factor on the y-axis.
     * @param z Scaling factor on the z-axis.
     */
    Scale(float x, float y, float z);
    /**
     * @brief Applies the scaling.
     */
    void apply() const override;
};

#endif