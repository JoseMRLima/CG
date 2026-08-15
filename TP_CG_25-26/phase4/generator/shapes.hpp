/**
 * @file shapes.hpp
 * @brief Function declarations for generating 3D shape models.
 * 
 */

#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>

/**
 * @brief Generates a plane model and saves it to a file.
 *
 * @param length The side length of the plane.
 * @param divisions The number of divisions along each side.
 * @param filename The name of the file to save the model to.
 * @return true if the model was generated and saved successfully, false
 * otherwise.
 */
bool generatePlane(float length, int divisions, std::string filename);

/**
 * @brief Generates a box model and saves it to a file.
 *
 * @param dimension The side length of the box.
 * @param divisions The number of divisions along each edge.
 * @param filename The name of the file to save the model to.
 * @return true if the model was generated and saved successfully, false
 * otherwise.
 */
bool generateBox(float dimension, int divisions, std::string filename);

/**
 * @brief Generates a sphere model and saves it to a file.
 *
 * @param radius The radius of the sphere.
 * @param slices The number of longitudinal divisions.
 * @param stacks The number of latitudinal divisions.
 * @param filename The name of the file to save the model to.
 * @return true if the model was generated and saved successfully, false
 * otherwise.
 */
bool generateSphere(float radius, int slices, int stacks, std::string filename);

/**
 * @brief Generates a cone model and saves it to a file.
 *
 * @param radius The radius of the base of the cone.
 * @param height The height of the cone.
 * @param slices The number of divisions around the base.
 * @param stacks The number of divisions along the height.
 * @param filename The name of the file to save the model to.
 * @return true if the model was generated and saved successfully, false
 * otherwise.
 */
bool generateCone(float radius, float height, int slices, int stacks,
                  std::string filename);

/**
 * @brief Generates a cylinder model and saves it to a file.
 *
 * @param radius The radius of the cylinder.
 * @param height The height of the cylinder.
 * @param slices The number of divisions around the cylinder.
 * @param filename The name of the file to save the model to.
 * @return true if the model was generated and saved successfully, false
 * otherwise.
 */
bool generateCylinder(float radius, float height, int slices,
                      std::string filename);

/**
 * @brief Generates a ring (torus) model and saves it to a file.
 *
 * @param majorRadius The distance from the center of the tube to the center of
 * the torus.
 * @param minorRadius The radius of the tube.
 * @param sides The number of segments for the tube cross-section.
 * @param rings The number of segments around the torus.
 * @param filename The name of the file to save the model to.
 * @return true if the model was generated and saved successfully, false
 * otherwise.
 */
bool generateRing(float majorRadius, float minorRadius, int sides, int rings,
                  std::string filename);

/**
 * @brief Generates a model from a Bézier patch file and saves it.
 *
 * @param bezier_patch The path to the Bézier patch file.
 * @param tessellation The number of divisions for tessellation.
 * @param filename The name of the file to save the model to.
 * @return true if the model was generated and saved successfully, false
 * otherwise.
 */
bool generatePatch(std::string bezier_patch, int tessellation,
                   std::string filename);

#endif
