
#include "World.h"
#include "tinyxml2.h"

#include <fstream>
#include <iostream>
#include <sstream>

bool World::parseWindow(tinyxml2::XMLElement *windowElem) {
    if (!windowElem) {
        std::cerr << "Missing <window>\n";
        return false;
    }

    if (windowElem->QueryIntAttribute("width", &window.width) != tinyxml2::XML_SUCCESS) {
        return false;
    }
    
    if (windowElem->QueryIntAttribute("height", &window.height) != tinyxml2::XML_SUCCESS) {
        return false;
    }

    return true;
}

bool World::parseCamera(tinyxml2::XMLElement *cameraElem) {
    if (!cameraElem) {
        std::cerr << "Missing <camera>\n";
        return false;
    }

    tinyxml2::XMLElement *pos = cameraElem->FirstChildElement("position");
    if (pos) {
        camera.position = Point::fromXML(pos);
    } else {
        return false;
    }

    tinyxml2::XMLElement *look = cameraElem->FirstChildElement("lookAt");
    if (look) {
        camera.lookAt = Point::fromXML(look);
    } else {
        return false;
    }

    tinyxml2::XMLElement *upElem = cameraElem->FirstChildElement("up");
    if (upElem) {
        camera.up = Point::fromXML(upElem);
    } else {
        camera.up = {0, 1, 0};
    }

    tinyxml2::XMLElement *proj = cameraElem->FirstChildElement("projection");
    if (proj) {
        proj->QueryFloatAttribute("fov", &camera.fov);
        proj->QueryFloatAttribute("near", &camera.near);
        proj->QueryFloatAttribute("far", &camera.far);
    } else {
        camera.fov = 60;
        camera.near = 1;
        camera.far = 1000;
    }

    return true;
}

bool World::parseModels(tinyxml2::XMLElement *modelsElem) {
    for (tinyxml2::XMLElement *modelElem =
             modelsElem->FirstChildElement("model");
         modelElem != nullptr;
         modelElem = modelElem->NextSiblingElement("model")) {

        const char *file = modelElem->Attribute("file");
        if (!file)
            continue;

        Model m;
        if (!m.loadFromFile(file)) {
            std::cerr << "Failed to load model: " << file << "\n";
            return false;
        }

        group.addModel(m);
    }

    return true;
}

bool World::parseGroup(tinyxml2::XMLElement *groupElem) {
    if (!groupElem) {
        std::cerr << "Missing <group>\n";
        return false;
    }

    tinyxml2::XMLElement *modelsElem = groupElem->FirstChildElement("models");

    if (modelsElem)
        return parseModels(modelsElem);

    return true;
}

bool World::loadFromFile(const std::string &filename) {
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << filename << "\n";
        return false;
    }

    tinyxml2::XMLElement *worldElem = doc.FirstChildElement("world");
    if (!worldElem) {
        std::cerr << "Missing <world> element\n";
        return false;
    }

    if (!parseWindow(worldElem->FirstChildElement("window")))
        return false;

    if (!parseCamera(worldElem->FirstChildElement("camera")))
        return false;

    if (!parseGroup(worldElem->FirstChildElement("group")))
        return false;

    return true;
}

void World::draw() const {
    group.draw();
}
