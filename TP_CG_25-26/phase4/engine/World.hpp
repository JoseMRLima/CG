/**
 * @file World.hpp
 * @brief Defines the World class, which encapsulates the entire 3D scene.
 *
 */

#ifndef WORLD_HPP
#define WORLD_HPP

#include "Group.hpp"
#include "Light.hpp"
#include "Point.hpp"
#include <string>

/**
 * @brief Represents the properties of the application window.
 */
struct Window {
    int width = 800;  ///< The width of the window.
    int height = 600; ///< The height of the window.
};

/**
 * @brief Represents the camera's properties and state for viewing the scene.
 */
struct Camera {
    Point position; ///< The position of the camera in 3D space.
    Point lookAt = {0.0f, 0.0f, 0.0f}; ///< The point the camera is looking at.
    Point up = {0.0f, 1.0f, 0.0f}; ///< The up vector for camera orientation.

    int tracking = 0; ///< Flag to indicate if mouse tracking is active.
    int startX = 0, startY = 0; ///< Initial mouse coordinates for tracking.
    int alpha = 0, beta = 45;   ///< Spherical coordinates.
    float r = 50;               ///< Radius for spherical camera positioning.

    float fov = 60.0f;   ///< Field of view for the perspective projection.
    float near = 1.0f;   ///< Near clipping plane distance.
    float far = 1000.0f; ///< Far clipping plane distance.
};

/**
 * @brief Manages all components of the 3D scene, including window, camera, and
 * models.
 */
class World {
  private:
    Window window; ///< The window settings for the scene.
    Camera camera; ///< The camera used to view the scene.
    Group group;   ///< The root group containing all models in the scene.
    std::vector<Light> lights; ///< A list of lights in the scene.

  public:
    /**
     * @brief Renders the entire scene by drawing the root group.
     */
    void draw();

    /**
     * @brief Sets the window properties for the scene.
     * @param w The new window settings.
     */
    void setWindow(const Window &w);

    /**
     * @brief Sets the camera properties for the scene.
     * @param c The new camera settings.
     */
    void setCamera(const Camera &c);

    /**
     * @brief Gets a reference to the root group of models.
     * @return A reference to the Group object.
     */
    Group &getGroup() { return group; }

    /**
     * @brief Gets a constant reference to the window settings.
     * @return A constant reference to the Window object.
     */
    const Window &getWindow() const { return window; }

    /**
     * @brief Gets a reference to the camera.
     * @return A reference to the Camera object.
     */
    Camera &getCamera() { return camera; }

    /**
     * @brief Initializes the scene's models for rendering.
     */
    void setup();

    /**
     * @brief Adds a light source to the scene.
     * @param l The light to add.
     */
    void addLight(const Light &l);
};

#endif