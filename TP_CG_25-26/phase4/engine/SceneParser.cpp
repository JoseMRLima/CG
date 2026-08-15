/**
 * @file SceneParser.cpp
 * @brief Implements the SceneParser class for loading 3D world descriptions
 * from XML files.
 *
 */

#include "SceneParser.hpp"
#include <iostream>
#include <vector>

/**
 * @brief Creates a Point object from an XML element's attributes.
 *
 * This helper function reads the 'x', 'y', and 'z' attributes from a
 * tinyxml2::XMLElement to construct a Point. If an attribute is not present,
 * its value defaults to 0.
 *
 * @param elem The XML element containing the point data.
 * @return A Point object.
 */
static Point pointFromXML(const tinyxml2::XMLElement *elem) {
    float x = 0, y = 0, z = 0;

    elem->QueryFloatAttribute("x", &x);
    elem->QueryFloatAttribute("y", &y);
    elem->QueryFloatAttribute("z", &z);

    return {x, y, z};
}

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

    parseLights(root->FirstChildElement("lights"), world);

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
        cam.position = pointFromXML(p);
    if (auto l = elem->FirstChildElement("lookAt"))
        cam.lookAt = pointFromXML(l);
    if (auto u = elem->FirstChildElement("up"))
        cam.up = pointFromXML(u);

    if (auto pr = elem->FirstChildElement("projection")) {
        pr->QueryFloatAttribute("fov", &cam.fov);
        pr->QueryFloatAttribute("near", &cam.near);
        pr->QueryFloatAttribute("far", &cam.far);
    }

    world.setCamera(cam);
    return true;
}

void SceneParser::parseColorComponent(const tinyxml2::XMLElement *elem,
                                      float out[4]) {
    if (!elem)
        return;

    int r = 0, g = 0, b = 0;

    elem->QueryIntAttribute("R", &r);
    elem->QueryIntAttribute("G", &g);
    elem->QueryIntAttribute("B", &b);

    out[0] = r / 255.0f;
    out[1] = g / 255.0f;
    out[2] = b / 255.0f;
    out[3] = 1.0f;
}

bool SceneParser::parseModels(tinyxml2::XMLElement *elem, Group &group) {
    if (!elem)
        return true;

    for (auto e = elem->FirstChildElement("model"); e;
         e = e->NextSiblingElement("model")) {

        const char *file = e->Attribute("file");
        if (!file)
            continue;

        Model m;
        if (!m.loadFromFile(file))
            continue;

        if (auto tex = e->FirstChildElement("texture")) {
            const char *texFile = tex->Attribute("file");
            if (texFile)
                m.setTextureFile(texFile);
        }

        if (auto color = e->FirstChildElement("color")) {
            Material mat;

            parseColorComponent(color->FirstChildElement("diffuse"),
                                mat.diffuse);
            parseColorComponent(color->FirstChildElement("ambient"),
                                mat.ambient);
            parseColorComponent(color->FirstChildElement("specular"),
                                mat.specular);
            parseColorComponent(color->FirstChildElement("emissive"),
                                mat.emissive);

            if (auto sh = color->FirstChildElement("shininess"))
                sh->QueryFloatAttribute("value", &mat.shininess);

            m.setMaterial(mat);
        }

        group.addModel(m);
    }
    return true;
}

bool SceneParser::parseTransform(tinyxml2::XMLElement *elem, Group &group) {
    if (!elem)
        return false;

    for (tinyxml2::XMLElement *child = elem->FirstChildElement();
         child != nullptr; child = child->NextSiblingElement()) {

        std::string name = child->Value();

        if (name == "translate") {
            float time = 0.0f;
            const bool hasTime = child->QueryFloatAttribute("time", &time) ==
                                 tinyxml2::XML_SUCCESS;

            if (hasTime && time > 0.0f) {
                bool align = false;
                if (child->QueryBoolAttribute("align", &align) !=
                    tinyxml2::XML_SUCCESS) {
                    const char *alignAttr = child->Attribute("align");
                    if (alignAttr) {
                        const std::string alignStr = alignAttr;
                        align = (alignStr == "True" || alignStr == "true");
                    }
                }

                std::vector<Point> points;
                for (tinyxml2::XMLElement *pointElem =
                         child->FirstChildElement("point");
                     pointElem != nullptr;
                     pointElem = pointElem->NextSiblingElement("point")) {
                    points.push_back(pointFromXML(pointElem));
                }

                if (points.size() >= 4) {
                    group.addAnimation(std::make_unique<TimedTranslation>(
                        time, std::move(points), align));
                }
            } else {
                float x = 0, y = 0, z = 0;
                child->QueryFloatAttribute("x", &x);
                child->QueryFloatAttribute("y", &y);
                child->QueryFloatAttribute("z", &z);
                group.addTransformation(std::make_unique<Translate>(x, y, z));
            }
        } else if (name == "rotate") {
            float x = 0, y = 0, z = 0;
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);

            float time = 0.0f;
            const bool hasTime = child->QueryFloatAttribute("time", &time) ==
                                 tinyxml2::XML_SUCCESS;

            if (hasTime && time > 0.0f) {
                group.addAnimation(
                    std::make_unique<TimedRotation>(time, x, y, z));
            } else {
                float angle = 0;
                child->QueryFloatAttribute("angle", &angle);
                group.addTransformation(
                    std::make_unique<Rotate>(angle, x, y, z));
            }
        } else if (name == "scale") {
            float x = 1, y = 1, z = 1;
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);
            group.addTransformation(std::make_unique<Scale>(x, y, z));
        }
    }
    return true;
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

bool SceneParser::parseLights(tinyxml2::XMLElement *elem, World &world) {
    if (!elem)
        return true;

    for (auto e = elem->FirstChildElement("light"); e;
         e = e->NextSiblingElement("light")) {

        const char *typeAttr = e->Attribute("type");
        if (!typeAttr) {
            std::cerr
                << "SceneParser: <light> missing 'type' attribute, skipping\n";
            continue;
        }

        Light l;
        l.type = Light::typeFromString(typeAttr);

        e->QueryFloatAttribute("posx", &l.posX);
        e->QueryFloatAttribute("posy", &l.posY);
        e->QueryFloatAttribute("posz", &l.posZ);

        e->QueryFloatAttribute("dirx", &l.dirX);
        e->QueryFloatAttribute("diry", &l.dirY);
        e->QueryFloatAttribute("dirz", &l.dirZ);

        e->QueryFloatAttribute("cutoff", &l.cutoff);
        e->QueryFloatAttribute("exponent", &l.exponent);

        world.addLight(l);
    }

    return true;
}
