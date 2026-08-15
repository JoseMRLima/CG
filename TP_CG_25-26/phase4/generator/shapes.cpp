/**
 * @file shapes.cpp
 * @brief Implementation of functions for generating 3D shape models.
 *
 */

#define _USE_MATH_DEFINES
#include <math.h>

#include "Model.hpp"
#include "Point.hpp"
#include "Vector.hpp"

#include <fstream>
#include <iostream>

static Vector toVector(const Point &p) { return {p.x, p.y, p.z}; }

/**
 * @brief Builds a plane by generating triangles.
 *
 * @param origin The starting point of the plane.
 * @param u The vector for the width direction.
 * @param v The vector for the height direction.
 * @param model The Model object to add the triangles to.
 * @param divisions The number of divisions for the plane grid.
 * @param increment The size of each division.
 */
static void buildPlane(const Point &origin, const Vector &u, const Vector &v,
                       Model &model, int divisions, float increment,
                       float totalLength) {
    // Plane normal is the same for every vertex.
    const Vector normal = u.cross(v).normalize();

    for (int i = 0; i < divisions; i++) {
        const float uDist = static_cast<float>(i) * increment;

        for (int j = 0; j < divisions; j++) {
            const float vDist = static_cast<float>(j) * increment;

            // positions
            Point pbl = origin + (u * uDist) + (v * vDist);
            Point pbr = pbl + (u * increment);
            Point ptl = pbl + (v * increment);
            Point ptr = pbr + (v * increment);

            // UVs — normalise distances into [0,1]
            UV uvbl = {uDist / totalLength, vDist / totalLength};
            UV uvbr = {(uDist + increment) / totalLength, vDist / totalLength};
            UV uvtl = {uDist / totalLength, (vDist + increment) / totalLength};
            UV uvtr = {(uDist + increment) / totalLength,
                       (vDist + increment) / totalLength};

            model.addTriangule({pbl, pbr, ptl}, {normal, normal, normal},
                               {uvbl, uvbr, uvtl});

            model.addTriangule({pbr, ptr, ptl}, {normal, normal, normal},
                               {uvbr, uvtr, uvtl});
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

    buildPlane(origin, u, v, model, divisions, increment, length);

    return model.toFile(filename);
}

bool generateBox(float dimension, int divisions, std::string filename) {
    Model model;
    const float increment = dimension / static_cast<float>(divisions);
    const float halfDim = dimension / 2.0f;

    // face on +z axis
    buildPlane({-halfDim, -halfDim, halfDim}, {1.0f, 0.0f, 0.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment, dimension);

    // face on -z axis
    buildPlane({halfDim, -halfDim, -halfDim}, {-1.0f, 0.0f, 0.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment, dimension);

    // face on +x axis
    buildPlane({halfDim, -halfDim, halfDim}, {0.0f, 0.0f, -1.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment, dimension);

    // face on -x axis
    buildPlane({-halfDim, -halfDim, -halfDim}, {0.0f, 0.0f, 1.0f},
               {0.0f, 1.0f, 0.0f}, model, divisions, increment, dimension);

    // face on +y axis
    buildPlane({-halfDim, halfDim, halfDim}, {1.0f, 0.0f, 0.0f},
               {0.0f, 0.0f, -1.0f}, model, divisions, increment, dimension);

    // face on -y axis
    buildPlane({-halfDim, -halfDim, -halfDim}, {1.0f, 0.0f, 0.0f},
               {0.0f, 0.0f, 1.0f}, model, divisions, increment, dimension);

    return model.toFile(filename);
}

bool generateSphere(float radius, int slices, int stacks,
                    std::string filename) {
    Model model;

    for (int i = 0; i < slices; i++) {
        float theta1 =
            static_cast<float>(i) * M_PI / static_cast<float>(slices);
        float theta2 =
            static_cast<float>(i + 1) * M_PI / static_cast<float>(slices);

        for (int j = 0; j < stacks; j++) {
            float phi1 = static_cast<float>(j) * 2.0f * M_PI /
                         static_cast<float>(stacks);
            float phi2 = static_cast<float>(j + 1) * 2.0f * M_PI /
                         static_cast<float>(stacks);

            // positions
            Point p1(radius * sin(theta1) * sin(phi1), radius * cos(theta1),
                     radius * sin(theta1) * cos(phi1));
            Point p2(radius * sin(theta1) * sin(phi2), radius * cos(theta1),
                     radius * sin(theta1) * cos(phi2));
            Point p3(radius * sin(theta2) * sin(phi1), radius * cos(theta2),
                     radius * sin(theta2) * cos(phi1));
            Point p4(radius * sin(theta2) * sin(phi2), radius * cos(theta2),
                     radius * sin(theta2) * cos(phi2));

            // normals
            Vector n1 = toVector(p1).normalize();
            Vector n2 = toVector(p2).normalize();
            Vector n3 = toVector(p3).normalize();
            Vector n4 = toVector(p4).normalize();

            // UVs
            UV uv1 = {phi1 / (2.0f * M_PI), theta1 / M_PI};
            UV uv2 = {phi2 / (2.0f * M_PI), theta1 / M_PI};
            UV uv3 = {phi1 / (2.0f * M_PI), theta2 / M_PI};
            UV uv4 = {phi2 / (2.0f * M_PI), theta2 / M_PI};

            model.addTriangule({p1, p4, p2}, {n1, n4, n2}, {uv1, uv4, uv2});
            model.addTriangule({p1, p3, p4}, {n1, n3, n4}, {uv1, uv3, uv4});
        }
    }

    return model.toFile(filename);
}

/**
 * @brief Calculates a point on a cone's surface.
 *
 * @param i The current stack index.
 * @param j The current slice index.
 * @param radius The radius of the cone's base.
 * @param height The height of the cone.
 * @param slices The total number of slices.
 * @param stacks The total number of stacks.
 * @return Point The calculated point on the cone's surface.
 */
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

    float slantLen = std::sqrt(radius * radius + height * height);
    float sinAlpha = radius / slantLen;
    float cosAlpha = height / slantLen;

    // base cap
    const Vector baseNormal = {0.0f, -1.0f, 0.0f};
    Point baseCenter = {0.0f, 0.0f, 0.0f};
    UV baseCenterUV = {0.5f, 0.5f};

    for (int j = 0; j < slices; j++) {
        float theta0 =
            2.0f * M_PI * static_cast<float>(j) / static_cast<float>(slices);
        float theta1 = 2.0f * M_PI * static_cast<float>(j + 1) /
                       static_cast<float>(slices);

        Point p0 = {radius * cos(theta0), 0.0f, radius * sin(theta0)};
        Point p1 = {radius * cos(theta1), 0.0f, radius * sin(theta1)};

        UV uv0 = {0.5f + 0.5f * cos(theta0), 0.5f + 0.5f * sin(theta0)};
        UV uv1 = {0.5f + 0.5f * cos(theta1), 0.5f + 0.5f * sin(theta1)};

        model.addTriangule({baseCenter, p0, p1},
                           {baseNormal, baseNormal, baseNormal},
                           {baseCenterUV, uv0, uv1});
    }

    // lateral surface
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            Point vbl = conePoint(i, j, radius, height, slices, stacks);
            Point vbr = conePoint(i, j + 1, radius, height, slices, stacks);
            Point vtl = conePoint(i + 1, j, radius, height, slices, stacks);
            Point vtr = conePoint(i + 1, j + 1, radius, height, slices, stacks);

            float thetaBL = 2.0f * M_PI * static_cast<float>(j) /
                            static_cast<float>(slices);
            float thetaBR = 2.0f * M_PI * static_cast<float>(j + 1) /
                            static_cast<float>(slices);

            float rBottom =
                (1.0f - static_cast<float>(i) / static_cast<float>(stacks)) *
                radius;
            float rTop = (1.0f - static_cast<float>(i + 1) /
                                     static_cast<float>(stacks)) *
                         radius;

            Vector nbl = Vector{sin(thetaBL), rBottom / height, cos(thetaBL)}
                             .normalize();
            Vector nbr = Vector{sin(thetaBR), rBottom / height, cos(thetaBR)}
                             .normalize();
            Vector ntl =
                Vector{sin(thetaBL), rTop / height, cos(thetaBL)}.normalize();
            Vector ntr =
                Vector{sin(thetaBR), rTop / height, cos(thetaBR)}.normalize();

            float vFracB = static_cast<float>(i) / static_cast<float>(stacks);
            float vFracT =
                static_cast<float>(i + 1) / static_cast<float>(stacks);

            UV uvbl = {thetaBL / (2.0f * M_PI), vFracB};
            UV uvbr = {thetaBR / (2.0f * M_PI), vFracB};
            UV uvtl = {thetaBL / (2.0f * M_PI), vFracT};
            UV uvtr = {thetaBR / (2.0f * M_PI), vFracT};

            model.addTriangule({vbl, vtl, vtr}, {nbl, ntl, ntr},
                               {uvbl, uvtl, uvtr});
            model.addTriangule({vbl, vtr, vbr}, {nbl, ntr, nbr},
                               {uvbl, uvtr, uvbr});
        }
    }

    return model.toFile(filename);
}

bool generateCylinder(float radius, float height, int slices,
                      std::string filename) {
    Model model;
    float sliceStep = 2.0f * M_PI / static_cast<float>(slices);
    float half = height / 2.0f;

    for (int i = 0; i < slices; i++) {
        float alpha = static_cast<float>(i) * sliceStep;
        float nextAlpha = static_cast<float>(i + 1) * sliceStep;

        float x1 = radius * cos(alpha);
        float z1 = radius * sin(alpha);
        float x2 = radius * cos(nextAlpha);
        float z2 = radius * sin(nextAlpha);

        // top cap
        {
            const Vector topNormal = {0.0f, 1.0f, 0.0f};
            UV uvCentre = {0.5f, 0.5f};
            UV uv2 = {0.5f + 0.5f * cos(nextAlpha),
                      0.5f + 0.5f * sin(nextAlpha)};
            UV uv1 = {0.5f + 0.5f * cos(alpha), 0.5f + 0.5f * sin(alpha)};

            model.addTriangule(
                {{0.0f, half, 0.0f}, {x2, half, z2}, {x1, half, z1}},
                {topNormal, topNormal, topNormal}, {uvCentre, uv2, uv1});
        }

        // bottom cap
        {
            const Vector botNormal = {0.0f, -1.0f, 0.0f};
            UV uvCentre = {0.5f, 0.5f};
            UV uv1 = {0.5f + 0.5f * cos(alpha), 0.5f + 0.5f * sin(alpha)};
            UV uv2 = {0.5f + 0.5f * cos(nextAlpha),
                      0.5f + 0.5f * sin(nextAlpha)};

            model.addTriangule(
                {{0.0f, -half, 0.0f}, {x1, -half, z1}, {x2, -half, z2}},
                {botNormal, botNormal, botNormal}, {uvCentre, uv1, uv2});
        }

        // side quads
        Vector n1 = {cos(alpha), 0.0f, sin(alpha)};
        Vector n2 = {cos(nextAlpha), 0.0f, sin(nextAlpha)};

        UV uvBotLeft = {alpha / (2.0f * M_PI), 0.0f};
        UV uvBotRight = {nextAlpha / (2.0f * M_PI), 0.0f};
        UV uvTopLeft = {alpha / (2.0f * M_PI), 1.0f};
        UV uvTopRight = {nextAlpha / (2.0f * M_PI), 1.0f};

        // triangle 1 (bottom-right, bottom-left, top-right)
        model.addTriangule({{x2, -half, z2}, {x1, -half, z1}, {x2, half, z2}},
                           {n2, n1, n2}, {uvBotRight, uvBotLeft, uvTopRight});

        // triangle 2 (top-right, bottom-left, top-left)
        model.addTriangule({{x2, half, z2}, {x1, -half, z1}, {x1, half, z1}},
                           {n2, n1, n1}, {uvTopRight, uvBotLeft, uvTopLeft});
    }

    return model.toFile(filename);
}

bool generateRing(float majorRadius, float minorRadius, int sides, int rings,
                  std::string filename) {
    Model model;

    for (int i = 0; i < rings; ++i) {
        float theta1 =
            static_cast<float>(i) * 2.0f * M_PI / static_cast<float>(rings);
        float theta2 =
            static_cast<float>(i + 1) * 2.0f * M_PI / static_cast<float>(rings);

        for (int j = 0; j < sides; ++j) {
            float phi1 =
                static_cast<float>(j) * 2.0f * M_PI / static_cast<float>(sides);
            float phi2 = static_cast<float>(j + 1) * 2.0f * M_PI /
                         static_cast<float>(sides);

            // positions
            Point p1((majorRadius + minorRadius * cos(phi1)) * cos(theta1),
                     minorRadius * sin(phi1),
                     (majorRadius + minorRadius * cos(phi1)) * sin(theta1));
            Point p2((majorRadius + minorRadius * cos(phi2)) * cos(theta1),
                     minorRadius * sin(phi2),
                     (majorRadius + minorRadius * cos(phi2)) * sin(theta1));
            Point p3((majorRadius + minorRadius * cos(phi1)) * cos(theta2),
                     minorRadius * sin(phi1),
                     (majorRadius + minorRadius * cos(phi1)) * sin(theta2));
            Point p4((majorRadius + minorRadius * cos(phi2)) * cos(theta2),
                     minorRadius * sin(phi2),
                     (majorRadius + minorRadius * cos(phi2)) * sin(theta2));

            // normals
            Vector n1 = Vector{cos(phi1) * cos(theta1), sin(phi1),
                               cos(phi1) * sin(theta1)}
                            .normalize();
            Vector n2 = Vector{cos(phi2) * cos(theta1), sin(phi2),
                               cos(phi2) * sin(theta1)}
                            .normalize();
            Vector n3 = Vector{cos(phi1) * cos(theta2), sin(phi1),
                               cos(phi1) * sin(theta2)}
                            .normalize();
            Vector n4 = Vector{cos(phi2) * cos(theta2), sin(phi2),
                               cos(phi2) * sin(theta2)}
                            .normalize();

            // UVs
            UV uv1 = {theta1 / (2.0f * M_PI), phi1 / (2.0f * M_PI)};
            UV uv2 = {theta1 / (2.0f * M_PI), phi2 / (2.0f * M_PI)};
            UV uv3 = {theta2 / (2.0f * M_PI), phi1 / (2.0f * M_PI)};
            UV uv4 = {theta2 / (2.0f * M_PI), phi2 / (2.0f * M_PI)};

            model.addTriangule({p1, p2, p4}, {n1, n2, n4}, {uv1, uv2, uv4});
            model.addTriangule({p1, p4, p3}, {n1, n4, n3}, {uv1, uv4, uv3});
        }
    }

    return model.toFile(filename);
}

/**
 * @brief Defines the structure for storing Bézier patch data.
 *
 */
struct BezierData {
    std::vector<std::vector<size_t>> patchIndices;
    std::vector<Point> controlPoints;
};

/**
 * @brief Parses a .patch file to extract Bézier patch data.
 *
 * @param filePath The path to the .patch file.
 * @return BezierData The parsed Bézier patch data.
 */
static BezierData parsePatchFile(const std::string &filePath) {
    BezierData data;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open patch file: " << filePath << "\n";
        return data;
    }

    // read the number of patches
    size_t num_patches;
    if (!(file >> num_patches))
        return data;

    data.patchIndices.resize(num_patches, std::vector<size_t>(16));

    // read the 16 control point indices for each patch
    for (size_t i = 0; i < num_patches; i++) {
        for (size_t j = 0; j < 16; j++) {
            size_t idx;
            file >> idx;
            file.ignore(); // skip commas or whitespace
            data.patchIndices[i][j] = idx;
        }
    }

    // read the number of control points
    size_t numberOfPoints;
    if (!(file >> numberOfPoints))
        return data;

    // read each control point
    for (size_t i = 0; i < numberOfPoints; i++) {
        float x, y, z;
        file >> x;
        file.ignore();
        file >> y;
        file.ignore();
        file >> z;
        file.ignore(); // skip commas and whitespace

        data.controlPoints.push_back(Point(x, y, z));
    }

    file.close();
    return data;
}

/**
 * @brief Calculates the Bernstein polynomial value.
 *
 * @param i The index of the Bernstein basis function.
 * @param t The parameter value (between 0 and 1).
 * @return float The value of the Bernstein polynomial.
 */
static float bernstein(int i, float t) {
    switch (i) {
        case 0:
            return (1 - t) * (1 - t) * (1 - t);
        case 1:
            return 3 * t * (1 - t) * (1 - t);
        case 2:
            return 3 * t * t * (1 - t);
        case 3:
            return t * t * t;
        default:
            return 0;
    }
}

static float bernsteinDerivative(int i, float t) {
    switch (i) {
        case 0:
            return -3 * (1 - t) * (1 - t);
        case 1:
            return 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
        case 2:
            return 6 * t * (1 - t) - 3 * t * t;
        case 3:
            return 3 * t * t;
        default:
            return 0;
    }
}

/**
 * @brief Calculates a point on a Bézier patch surface.
 *
 * @param controlPoints The 16 control points defining the patch.
 * @param u The parameter value in the u direction.
 * @param v The parameter value in the v direction.
 * @return Point The calculated point on the patch surface.
 */
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

/**
 * @brief Tangent in the u direction: dP/du.
 *
 * @param cp control points of the patch
 * @param u the parameter in the u direction
 * @param v the parameter in the v direction
 * @return Vector vector representing the tangent in the u direction at (u, v)
 */
static Vector bezierTangentU(const std::vector<Point> &cp, float u, float v) {
    Vector t = {0, 0, 0};

    for (int i = 0; i < 4; i++) {
        float dbu = bernsteinDerivative(i, u);

        for (int j = 0; j < 4; j++) {
            float b = dbu * bernstein(j, v);

            t.x += cp[i * 4 + j].x * b;
            t.y += cp[i * 4 + j].y * b;
            t.z += cp[i * 4 + j].z * b;
        }
    }

    return t;
}

/**
 * @brief Tangent in the v direction: dP/dv.
 *
 * @param cp control points of the patch
 * @param u the parameter in the u direction
 * @param v the parameter in the v direction
 * @return Vector vector representing the tangent in the v direction at (u, v)
 */
static Vector bezierTangentV(const std::vector<Point> &cp, float u, float v) {
    Vector t = {0, 0, 0};

    for (int i = 0; i < 4; i++) {
        float bu = bernstein(i, u);

        for (int j = 0; j < 4; j++) {
            float b = bu * bernsteinDerivative(j, v);

            t.x += cp[i * 4 + j].x * b;
            t.y += cp[i * 4 + j].y * b;
            t.z += cp[i * 4 + j].z * b;
        }
    }

    return t;
}

static Vector bezierNormal(const std::vector<Point> &cp, float u, float v) {
    Vector tu = bezierTangentU(cp, u, v);
    Vector tv = bezierTangentV(cp, u, v);
    Vector n = tv.cross(tu);
    float len = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    if (len < 1e-6f)
        return {0.0f, 1.0f, 0.0f};
    return n.normalize();
}

bool generatePatch(std::string bezier_patch, int tessellation,
                   std::string filename) {
    Model model;
    BezierData patchContent = parsePatchFile(bezier_patch);

    for (const auto &indices : patchContent.patchIndices) {
        std::vector<Point> cp;
        for (size_t idx : indices)
            cp.push_back(patchContent.controlPoints[idx]);

        for (int ui = 0; ui < tessellation; ui++) {
            for (int vi = 0; vi < tessellation; vi++) {
                float u1 =
                    static_cast<float>(ui) / static_cast<float>(tessellation);
                float v1 =
                    static_cast<float>(vi) / static_cast<float>(tessellation);
                float u2 = static_cast<float>(ui + 1) /
                           static_cast<float>(tessellation);
                float v2 = static_cast<float>(vi + 1) /
                           static_cast<float>(tessellation);

                Point p1 = bezierPatch(cp, u1, v1);
                Point p2 = bezierPatch(cp, u2, v1);
                Point p3 = bezierPatch(cp, u1, v2);
                Point p4 = bezierPatch(cp, u2, v2);

                Vector n1 = bezierNormal(cp, u1, v1);
                Vector n2 = bezierNormal(cp, u2, v1);
                Vector n3 = bezierNormal(cp, u1, v2);
                Vector n4 = bezierNormal(cp, u2, v2);

                UV uv1 = {u1, v1};
                UV uv2 = {u2, v1};
                UV uv3 = {u1, v2};
                UV uv4 = {u2, v2};

                model.addTriangule({p1, p3, p2}, {n1, n3, n2}, {uv1, uv3, uv2});
                model.addTriangule({p2, p3, p4}, {n2, n3, n4}, {uv2, uv3, uv4});
            }
        }
    }

    return model.toFile(filename);
}