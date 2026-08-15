/**
 * @file Light.hpp
 * @brief Defines the Light class for representing OpenGL light sources.
 *
 */

#ifndef LIGHT_HPP
#define LIGHT_HPP

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>

/**
 * @brief Enumerates the supported light source types.
 */
enum class LightType { Point, Directional, Spot };

/**
 * @brief Represents a single OpenGL light source.
 */
struct Light {
    LightType type;

    // position
    float posX = 0.0f, posY = 0.0f, posZ = 0.0f;

    // direction
    float dirX = 0.0f, dirY = 0.0f, dirZ = 0.0f;

    // spotlight cone half-angle in degrees
    float cutoff = 45.0f;

    // spotlight concentration exponent (0 = uniform, higher = more focused)
    float exponent = 0.0f;

    /**
     * @brief Enables OpenGL lighting globally.
     * Call once at engine startup, before any setup() calls.
     */
    static void enableLighting();

    /**
     * @brief Activates this light on the given OpenGL light index.
     *
     * Must be called in world space (before any model transforms are applied)
     * so that positions and directions are interpreted correctly by OpenGL.
     *
     * @param index OpenGL light index in [0, 7]
     */
    void setup(int index) const;

    /**
     * @brief Parses a LightType from its XML string representation.
     * @param str One of "point", "directional", or "spot".
     * @return The corresponding LightType. Defaults to Point on unknown input.
     */
    static LightType typeFromString(const std::string &str);
};

#endif