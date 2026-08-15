#pragma once

#include "World.h"
#include <string>
#include <tinyxml2.h>

class SceneParser {
  public:
    static bool loadWorld(const std::string &filename, World &world);

  private:
    static bool parseWindow(tinyxml2::XMLElement *elem, World &world);
    static bool parseCamera(tinyxml2::XMLElement *elem, World &world);
    static bool parseGroup(tinyxml2::XMLElement *elem, Group &group);
    static bool parseModels(tinyxml2::XMLElement *elem, Group &group);
    static void parseTransform(tinyxml2::XMLElement *elem, Group &group);
};
