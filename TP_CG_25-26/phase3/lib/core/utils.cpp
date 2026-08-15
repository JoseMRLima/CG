
#include "utils.hpp"
#include <array>
#include <cmath>
#include <map>
#include <utility>

void generateBuffers(const std::vector<Point> &points, std::vector<float> &vbo,
                     std::vector<unsigned int> &ibo) {
    // clear data from vectors
    vbo.clear();
    ibo.clear();

    // Point -> Index
    std::map<Point, unsigned int> pointToIndex;

    for (const auto &p : points) {
        auto it = pointToIndex.find(p);

        if (it == pointToIndex.end()) {
            // unique point
            unsigned int newIndex = static_cast<unsigned int>(vbo.size() / 3);
            pointToIndex[p] = newIndex;

            vbo.push_back(p.x);
            vbo.push_back(p.y);
            vbo.push_back(p.z);

            ibo.push_back(newIndex);
        } else {
            // point already exists, reuse index
            ibo.push_back(it->second);
        }
    }
}

static const std::array<std::array<float, 4>, 4> catmoll_rom_matrix{{
    {-0.5f, +1.5f, -1.5f, +0.5f},
    {+1.0f, -2.5f, +2.0f, -0.5f},
    {-0.5f, +0.0f, +0.5f, +0.0f},
    {+0.0f, +1.0f, +0.0f, +0.0f},
}};

std::pair<Point, Vector> catmollRomPosition(std::vector<Point> curve,
                                            float global_time) {
    const std::array<std::array<float, 4>, 4> matrix = catmoll_rom_matrix;

    float t = global_time * curve.size();
    float segment = (int)floor(t);
    t -= segment;

    int first = segment + curve.size() - 1;
    Point p1 = curve[(first + 0) % curve.size()];
    Point p2 = curve[(first + 1) % curve.size()];
    Point p3 = curve[(first + 2) % curve.size()];
    Point p4 = curve[(first + 3) % curve.size()];

    const std::array<std::array<float, 4>, 3> p{{
        {p1.x, p2.x, p3.x, p4.x},
        {p1.y, p2.y, p3.y, p4.y},
        {p1.z, p2.z, p3.z, p4.z},
    }};

    const std::array<float, 4> timeP = {t * t * t, t * t, t, 1};

    const std::array<float, 4> timeDP = {3 * t * t, 2 * t, 1, 0};

    std::array<float, 3> pv{};
    std::array<float, 3> dv{};

    for (size_t i = 0; i < 3; ++i) {
        std::array<float, 4> a{};

        for (size_t j = 0; j < 4; ++j)
            for (size_t k = 0; k < 4; ++k)
                a[j] += p[i][k] * matrix[j][k];

        for (size_t j = 0; j < 4; j++) {
            pv[i] += timeP[j] * a[j];
            dv[i] += timeDP[j] * a[j];
        }
    }

    return {
        Point(pv[0], pv[1], pv[2]),
        Vector(dv[0], dv[1], dv[2]),
    };
}
