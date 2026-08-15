#include "SceneParser.h"
#include <iostream>

bool SceneParser::loadWorld(const std::string &filename, World &world) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML: " << filename << "\n";
        return false;
    }

    tinyxml2::XMLElement *root = doc.FirstChildElement("world");
    if (!root)
        return false;

    if (!parseWindow(root->FirstChildElement("window"), world))
        return false;
    if (!parseCamera(root->FirstChildElement("camera"), world))
        return false;

    if (!parseGroup(root->FirstChildElement("group"), world.getGroup()))
        return false;

    return true;
}

bool SceneParser::parseWindow(tinyxml2::XMLElement *elem, World &world) {
    if (!elem)
        return false;
    Window win;
    elem->QueryIntAttribute("width", &win.width);
    elem->QueryIntAttribute("height", &win.height);
    world.setWindow(win);
    return true;
}

bool SceneParser::parseCamera(tinyxml2::XMLElement *elem, World &world) {
    if (!elem)
        return false;
    Camera cam;

    if (auto p = elem->FirstChildElement("position"))
        cam.position = Point::fromXML(p);
    if (auto l = elem->FirstChildElement("lookAt"))
        cam.lookAt = Point::fromXML(l);
    if (auto u = elem->FirstChildElement("up"))
        cam.up = Point::fromXML(u);

    if (auto pr = elem->FirstChildElement("projection")) {
        pr->QueryFloatAttribute("fov", &cam.fov);
        pr->QueryFloatAttribute("near", &cam.near);
        pr->QueryFloatAttribute("far", &cam.far);
    }

    world.setCamera(cam);
    return true;
}

bool SceneParser::parseModels(tinyxml2::XMLElement *elem, Group &group) {
    if (!elem)
        return true;
    for (auto e = elem->FirstChildElement("model"); e;
         e = e->NextSiblingElement("model")) {
        const char *file = e->Attribute("file");
        if (file) {
            Model m;
            if (m.loadFromFile(file))
                group.addModel(m);
        }
    }
    return true;
}

void SceneParser::parseTransform(tinyxml2::XMLElement *elem, Group &group) {
    if (!elem)
        return;

    for (tinyxml2::XMLElement *child = elem->FirstChildElement();
         child != nullptr; child = child->NextSiblingElement()) {

        std::string name = child->Value();

        if (name == "translate") {
            float x = 0, y = 0, z = 0;
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);
            group.addTransformation(std::make_unique<Translate>(x, y, z));
        } else if (name == "rotate") {
            float angle = 0, x = 0, y = 0, z = 0;
            child->QueryFloatAttribute("angle", &angle);
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);
            group.addTransformation(std::make_unique<Rotate>(angle, x, y, z));
        } else if (name == "scale") {
            float x = 1, y = 1, z = 1;
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);
            group.addTransformation(std::make_unique<Scale>(x, y, z));
        }
    }
}

bool SceneParser::parseGroup(tinyxml2::XMLElement *elem, Group &group) {
    if (!elem)
        return false;

    parseTransform(elem->FirstChildElement("transform"), group);

    parseModels(elem->FirstChildElement("models"), group);

    for (auto childElem = elem->FirstChildElement("group");
         childElem != nullptr;
         childElem = childElem->NextSiblingElement("group")) {

        Group childGroup;
        if (parseGroup(childElem, childGroup)) {
            group.addGroup(std::move(childGroup));
        }
    }

    return true;
}