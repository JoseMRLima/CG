/**
 * @file Light.cpp
 * @brief Implementation of the Light struct.
 *
 */

#include "Light.hpp"
#include <iostream>

void Light::enableLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    float globalAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
}

void Light::setup(int index) const {
    if (index < 0 || index > 7) {
        std::cerr << "Light::setup - index " << index
                  << " out of range [0,7]\n";
        return;
    }

    GLenum glLight = GL_LIGHT0 + static_cast<GLenum>(index);
    glEnable(glLight);

    switch (type) {
        case LightType::Point: {
            float pos[4] = {posX, posY, posZ, 1.0f};
            float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

            glLightfv(glLight, GL_POSITION, pos);
            glLightfv(glLight, GL_DIFFUSE, white);
            glLightfv(glLight, GL_SPECULAR, white);
            glLightf(glLight, GL_SPOT_CUTOFF, 180.0f);

            break;
        }
        case LightType::Directional: {
            float dir[4] = {dirX, dirY, dirZ, 0.0f};
            float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

            glLightfv(glLight, GL_POSITION, dir);
            glLightfv(glLight, GL_DIFFUSE, white);
            glLightfv(glLight, GL_SPECULAR, white);
            glLightf(glLight, GL_SPOT_CUTOFF, 180.0f);

            break;
        }
        case LightType::Spot: {
            float pos[4] = {posX, posY, posZ, 1.0f};
            float dir[3] = {dirX, dirY, dirZ};
            float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

            glLightfv(glLight, GL_POSITION, pos);
            glLightfv(glLight, GL_DIFFUSE, white);
            glLightfv(glLight, GL_SPECULAR, white);
            glLightfv(glLight, GL_SPOT_DIRECTION, dir);
            glLightf(glLight, GL_SPOT_CUTOFF, cutoff);
            glLightf(glLight, GL_SPOT_EXPONENT, exponent);

            break;
        }
    }
}

LightType Light::typeFromString(const std::string &str) {
    if (str == "directional")
        return LightType::Directional;

    if (str == "spot" || str == "spotlight")
        return LightType::Spot;

    if (str != "point")
        std::cerr << "Light::typeFromString — unknown type \"" << str
                  << "\", defaulting to Point\n";

    return LightType::Point;
}
