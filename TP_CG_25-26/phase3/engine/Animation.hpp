#pragma once

#include "Point.hpp"
#include <array>
#include <utility>
#include <vector>

class Animation {
  public:
    virtual ~Animation() = default;
    virtual void update(float deltaTime) = 0;
};

class TimedTranslation : public Animation {
    float time;
    std::vector<Point> curvePoints;
    bool aligned;

  public:
    TimedTranslation(float time, std::vector<Point> curvePoints, bool aligned);
    void update(float deltaTime) override;

  private:
    std::pair<Point, Vector> getLocation(float elapsed_time);
    void renderCatmullRomCurve();
    std::array<float, 16> rotationMatrix(Vector x, Vector y, Vector z);
};

class TimedRotation : public Animation {
    float time, x, y, z;

  public:
    TimedRotation(float time, float x, float y, float z);
    void update(float deltaTime) override;
};
