
#include "Animation.hpp"
#include "utils.hpp"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

std::pair<Point, Vector> TimedTranslation::getLocation(float elapsed_time) {
    int point_count = this->curvePoints.size();

    float gt = elapsed_time / this->time;
    float t = gt * point_count;

    size_t index = std::floor(t);
    t = t - index;

    return catmollRomPosition(this->curvePoints, t);
}

void TimedTranslation::renderCatmullRomCurve() {
    float gt = 0.0;
    const float NUM_STEPS = 100;
    const float gt_step = this->time / NUM_STEPS;

    glBegin(GL_LINE_LOOP);

    for (int i = 0; i < NUM_STEPS; i++) {
        auto pos_deriv = this->getLocation(gt);
        Point pos = std::get<0>(pos_deriv);
        glVertex3f(pos.x, pos.y, pos.z);
        gt += gt_step;
    }

    glEnd();
}

TimedTranslation::TimedTranslation(float time, std::vector<Point> curvePoints,
                                   bool aligned)
    : time(time), curvePoints(std::move(curvePoints)), aligned(aligned) {}

std::array<float, 16> TimedTranslation::rotationMatrix(Vector x, Vector y,
                                                       Vector z) {
    return std::array<float, 16>{{
        x.x,
        x.y,
        x.z,
        0,
        y.x,
        y.y,
        y.z,
        0,
        z.x,
        z.y,
        z.z,
        0,
        0,
        0,
        0,
        1,
    }};
}

void TimedTranslation::update(float deltaTime) {
    if (this->time > 0) {

        // renderCatmullRomCurve();

        float time = deltaTime / this->time;

        std::pair<Point, Vector> position_dir =
            catmollRomPosition(this->curvePoints, time);
        Point pos = position_dir.first;
        Vector dir = position_dir.second;

        glTranslatef(pos.x, pos.y, pos.z);

        if (this->aligned) {
            Vector x = dir.normalize();
            Vector z = x.cross(Vector(0.0f, 1.0f, 0.0f)).normalize();
            Vector y = z.cross(x).normalize();
            glMultMatrixf(rotationMatrix(x, y, z).data());
        }
    }
}

TimedRotation::TimedRotation(float time, float x, float y, float z)
    : time(time), x(x), y(y), z(z) {}

void TimedRotation::update(float deltaTime) {
    if (this->time > 0) {
        float angle = (deltaTime / this->time) * 360;
        glRotatef(angle, x, y, z);
    }
}