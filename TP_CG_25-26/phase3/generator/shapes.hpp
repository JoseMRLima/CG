
#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>

bool generatePlane(float length, int divisions, std::string filename);
bool generateBox(float dimension, int divisions, std::string filename);
bool generateSphere(float radius, int slices, int stacks, std::string filename);
bool generateCone(float radius, float height, int slices, int stacks,
                  std::string filename);
bool generateCylinder(float radius, float height, int slices,
                      std::string filename);
bool generateRing(float majorRadius, float minorRadius, int sides, int rings,
                  std::string filename);

bool generatePatch(std::string bezier_patch, int tessellation,
                   std::string filename);

#endif
