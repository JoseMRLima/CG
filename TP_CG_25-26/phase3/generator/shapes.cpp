/* shapes.cpp */

#define _USE_MATH_DEFINES
#include <math.h>

#include "Model.hpp"
#include "Point.hpp"
#include "Vector.hpp"

#include <fstream>
#include <iostream>

static void buildPlane(const Point &origin, const Vector &u, const Vector &v,
                       Model &model, int divisions, float increment) {
    for (int i = 0; i < divisions; i++) {
        const float uDist = static_cast<float>(i) * increment;

        for (int j = 0; j < divisions; j++) {
            const float vDist = static_cast<float>(j) * increment;

            // bottom left
            Point pbl = origin + (u * uDist) + (v * vDist);
            // bottom right
            Point pbr = pbl + (u * increment);
            // top left
            Point ptl = pbl + (v * increment);
            // top right
            Point ptr = pbr + (v * increment);

            // add triangles to model
            model.addTriangule({pbl, pbr, ptl});
            model.addTriangule({pbr, ptr, ptl});
        }
    }
}

bool generatePlane(float length, int divisions, std::string filename) {
    // size of each small square
    const float increment = length / static_cast<float>(divisions);
    // starting coordinate so the plane is centered
    const float start = length / 2.0f;

    Point origin(-start, 0.0f, start);
    Vector u(1.0f, 0.0f, 0.0f);
    Vector v(0.0f, 0.0f, -1.0f);

    Model model;

    buildPlane(origin, u, v, model, divisions, increment);

    return model.toFile(filename);
}

bool generateBox(float dimension, int divisions, std::string filename) {
    Model model;
    const float increment = dimension / static_cast<float>(divisions);
    const float halfDim = dimension / 2.0f;

    // face on +z axis
    buildPlane({-halfDim, -halfDim, halfDim}, {1.0f, 0.0f, 0.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment);

    // face on -z axis
    buildPlane({halfDim, -halfDim, -halfDim}, {-1.0f, 0.0f, 0.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment);

    // face on +x axis
    buildPlane({halfDim, -halfDim, halfDim}, {0.0f, 0.0f, -1.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment);

    // face on -x axis
    buildPlane({-halfDim, -halfDim, -halfDim}, {0.0f, 0.0f, 1.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment);

    // face on +y axis
    buildPlane({-halfDim, halfDim, halfDim}, {1.0f, 0.0f, 0.0f},
               {0.0f, 0.0f, -1.0f}, model, divisions, increment);

    // face on -y axis
    buildPlane({-halfDim, -halfDim, -halfDim}, {1.0f, 0.0f, 0.0f},
               {0.0f, 0.0f, 1.0f}, model, divisions, increment);

    return model.toFile(filename);
}

bool generateSphere(float radius, int slices, int stacks,
                    std::string filename) {
    Model model;
    for (int i = 0; i < slices; i++) {
        float theta1 = i * M_PI / static_cast<float>(slices);
        float theta2 = (i + 1) * M_PI / static_cast<float>(slices);

        for (int j = 0; j < stacks; j++) {
            float phi1 = j * 2.0f * M_PI / static_cast<float>(stacks);
            float phi2 = (j + 1) * 2.0f * M_PI / static_cast<float>(stacks);

            float x1 = radius * sin(theta1) * sin(phi1);
            float y1 = radius * cos(theta1);
            float z1 = radius * sin(theta1) * cos(phi1);

            float x2 = radius * sin(theta1) * sin(phi2);
            float y2 = radius * cos(theta1);
            float z2 = radius * sin(theta1) * cos(phi2);

            float x3 = radius * sin(theta2) * sin(phi1);
            float y3 = radius * cos(theta2);
            float z3 = radius * sin(theta2) * cos(phi1);

            float x4 = radius * sin(theta2) * sin(phi2);
            float y4 = radius * cos(theta2);
            float z4 = radius * sin(theta2) * cos(phi2);

            Point p1(x1, y1, z1);
            Point p2(x2, y2, z2);
            Point p3(x3, y3, z3);
            Point p4(x4, y4, z4);

            model.addTriangule({p1, p4, p2});
            model.addTriangule({p1, p3, p4});
        }
    }

    return model.toFile(filename);
}

static Point conePoint(int i, int j, float radius, float height, int slices,
                       int stacks) {
    float aux = static_cast<float>(i) / static_cast<float>(stacks);

    float currHeight = aux * height;
    float currRadius = (1.0f - aux) * radius;
    float theta =
        2.0f * M_PI * static_cast<float>(j) / static_cast<float>(slices);

    return {currRadius * cos(theta), currHeight, currRadius * sin(theta)};
}

bool generateCone(float radius, float height, int slices, int stacks,
                  std::string filename) {
    Model model;
    Point baseCenter = {0.0f, 0.0f, 0.0f};

    for (int j = 0; j < slices; j++) {
        float theta0 =
            2.0f * M_PI * static_cast<float>(j) / static_cast<float>(slices);
        float theta1 = 2.0f * M_PI * static_cast<float>(j + 1) /
                       static_cast<float>(slices);

        Point p0 = {radius * cos(theta0), 0.0f, radius * sin(theta0)};
        Point p1 = {radius * cos(theta1), 0.0f, radius * sin(theta1)};

        // add triangle to model
        model.addTriangule({baseCenter, p0, p1});
    }

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            // bottom left
            Point vbl = conePoint(i, j, radius, height, slices, stacks);
            // bottom right
            Point vbr = conePoint(i, j + 1, radius, height, slices, stacks);
            // top left
            Point vtl = conePoint(i + 1, j, radius, height, slices, stacks);
            // top right
            Point vtr = conePoint(i + 1, j + 1, radius, height, slices, stacks);

            // add triangles to model
            model.addTriangule({vbl, vtl, vtr});
            model.addTriangule({vbl, vtr, vbr});
        }
    }

    return model.toFile(filename);
}

bool generateCylinder(float radius, float height, int slices,
                      std::string filename) {
    Model model;
    float sliceStep = 2.0f * M_PI / slices;
    float half = height / 2.0f;

    for (int i = 0; i < slices; i++) {
        float alpha = i * sliceStep;
        float nextAlpha = (i + 1) * sliceStep;

        float x1 = radius * cos(alpha);
        float z1 = radius * sin(alpha);

        float x2 = radius * cos(nextAlpha);
        float z2 = radius * sin(nextAlpha);

        // top
        Point p1(0.0f, half, 0.0f);
        Point p2(x2, half, z2);
        Point p3(x1, half, z1);
        model.addTriangule({p1, p2, p3});

        // bottom
        p1 = {0.0f, -half, 0.0f};
        p2 = {x1, -half, z1};
        p3 = {x2, -half, z2};
        model.addTriangule({p1, p2, p3});

        // sides
        p1 = {x2, -half, z2};
        p2 = {x1, -half, z1};
        p3 = {x2, half, z2};
        model.addTriangule({p1, p2, p3});

        p1 = {x2, half, z2};
        p2 = {x1, -half, z1};
        p3 = {x1, half, z1};
        model.addTriangule({p1, p2, p3});
    }

    return model.toFile(filename);
}

bool generateRing(float majorRadius, float minorRadius, int sides, int rings,
                  std::string filename) {
    Model model;

    for (int i = 0; i < rings; ++i) {
        float theta1 = static_cast<float>(i) * 2.0f * static_cast<float>(M_PI) /
                       static_cast<float>(rings);
        float theta2 = static_cast<float>(i + 1) * 2.0f *
                       static_cast<float>(M_PI) / static_cast<float>(rings);

        for (int j = 0; j < sides; ++j) {
            float phi1 = static_cast<float>(j) * 2.0f *
                         static_cast<float>(M_PI) / static_cast<float>(sides);
            float phi2 = static_cast<float>(j + 1) * 2.0f *
                         static_cast<float>(M_PI) / static_cast<float>(sides);

            // Vertices
            float x1 = (majorRadius + minorRadius * cos(phi1)) * cos(theta1);
            float y1 = minorRadius * sin(phi1);
            float z1 = (majorRadius + minorRadius * cos(phi1)) * sin(theta1);

            float x2 = (majorRadius + minorRadius * cos(phi2)) * cos(theta1);
            float y2 = minorRadius * sin(phi2);
            float z2 = (majorRadius + minorRadius * cos(phi2)) * sin(theta1);

            float x3 = (majorRadius + minorRadius * cos(phi1)) * cos(theta2);
            float y3 = minorRadius * sin(phi1);
            float z3 = (majorRadius + minorRadius * cos(phi1)) * sin(theta2);

            float x4 = (majorRadius + minorRadius * cos(phi2)) * cos(theta2);
            float y4 = minorRadius * sin(phi2);
            float z4 = (majorRadius + minorRadius * cos(phi2)) * sin(theta2);

            // Push vertices in counter-clockwise order
            Point p1(x1, y1, z1);
            Point p2(x2, y2, z2);
            Point p3(x4, y4, z4);
            model.addTriangule({p1, p2, p3});

            p1 = {x1, y1, z1};
            p2 = {x4, y4, z4};
            p3 = {x3, y3, z3};
            model.addTriangule({p1, p2, p3});
        }
    }

    return model.toFile(filename);
}

struct BezierData {
    std::vector<std::vector<size_t>> patchIndices;
    std::vector<Point> controlPoints;
};

static BezierData parsePatchFile(const std::string &filePath) {
    BezierData data;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open patch file: " << filePath << "\n";
        return data;
    }

    // 1. Read the number of patches in the file
    size_t num_patches;
    if (!(file >> num_patches))
        return data;

    data.patchIndices.resize(num_patches, std::vector<size_t>(16));

    // 2. Read the Topology: 16 control point indices for each patch
    // These indices point to the coordinates list we will read later.
    for (size_t i = 0; i < num_patches; i++) {
        for (size_t j = 0; j < 16; j++) {
            size_t idx;
            file >> idx;
            file.ignore(); // Skips commas or whitespace between indices
            data.patchIndices[i][j] = idx;
        }
    }

    // 3. Read the total number of control points (coordinates)
    size_t numberOfPoints;
    if (!(file >> numberOfPoints))
        return data;

    // 4. Read the Geometry: The actual X, Y, Z coordinates
    for (size_t i = 0; i < numberOfPoints; i++) {
        float x, y, z;
        file >> x;
        file.ignore();
        file >> y;
        file.ignore();
        file >> z;
        file.ignore(); // Skips commas/whitespace

        data.controlPoints.push_back(Point(x, y, z));
    }

    file.close();
    return data;
}

static float bernstein(int i, float t) {
    float result = 0;
    switch (i) {
        case 0:
            result = (1 - t) * (1 - t) * (1 - t);
            break;
        case 1:
            result = 3 * t * (1 - t) * (1 - t);
            break;
        case 2:
            result = 3 * t * t * (1 - t);
            break;
        case 3:
            result = t * t * t;
            break;
        default:
            result = 0;
            break;
    }
    return result;
}

static Point bezierPatch(const std::vector<Point> &controlPoints, float u,
                         float v) {
    Point p(0, 0, 0);
    float aux = 0;

    for (int i = 0; i < 4; i++) {
        aux = bernstein(i, u);
        for (int j = 0; j < 4; j++) {
            float b = aux * bernstein(j, v);
            p.x += controlPoints[i * 4 + j].x * b;
            p.y += controlPoints[i * 4 + j].y * b;
            p.z += controlPoints[i * 4 + j].z * b;
        }
    }

    return p;
}

bool generatePatch(std::string bezier_patch, int tessellation,
                   std::string filename) {
    Model model;
    BezierData patchContent = parsePatchFile(bezier_patch);

    for (const auto &indices : patchContent.patchIndices) {
        std::vector<Point> patchControlPoints;
        for (size_t idx : indices) {
            patchControlPoints.push_back(patchContent.controlPoints[idx]);
        }

        for (int u = 0; u < tessellation; u++) {
            for (int v = 0; v < tessellation; v++) {
                float u1 = (float)u / tessellation;
                float v1 = (float)v / tessellation;
                float u2 = (float)(u + 1) / tessellation;
                float v2 = (float)(v + 1) / tessellation;

                Point p1 = bezierPatch(patchControlPoints, u1, v1);
                Point p2 = bezierPatch(patchControlPoints, u2, v1);
                Point p3 = bezierPatch(patchControlPoints, u1, v2);
                Point p4 = bezierPatch(patchControlPoints, u2, v2);

                model.addTriangule({p1, p3, p2});
                model.addTriangule({p2, p3, p4});
            }
        }
    }

    return model.toFile(filename);
}