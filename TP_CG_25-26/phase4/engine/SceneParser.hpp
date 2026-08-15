/**
 * @file SceneParser.hpp
 * @brief Defines the SceneParser class for loading 3D world descriptions from
 * XML files.
 *
 */

#ifndef SCENE_PARSER_HPP
#define SCENE_PARSER_HPP

#include "World.hpp"
#include <string>
#include <tinyxml2.h>

/**
 * @class SceneParser
 * @brief A utility class to parse an XML file and build a World object.
 *
 * This class provides static methods to read an XML file that describes a 3D
 * scene, including window settings, camera properties, and a scene graph of
 * groups, models, and transformations.
 */
class SceneParser {
  public:
    /**
     * @brief Loads a world description from an XML file.
     * @param filename The path to the XML file.
     * @param world The World object to populate with the parsed data.
     * @return True if the file was loaded and parsed successfully, false
     * otherwise.
     */
    static bool loadWorld(const std::string &filename, World &world);

  private:
    /**
     * @brief Parses the window settings from an XML element.
     * @param elem The XML element containing window data.
     * @param world The World object to update.
     * @return True on success, false if the element is null.
     */
    static bool parseWindow(tinyxml2::XMLElement *elem, World &world);

    /**
     * @brief Parses the camera settings from an XML element.
     * @param elem The XML element containing camera data.
     * @param world The World object to update.
     * @return True on success, false if the element is null.
     */
    static bool parseCamera(tinyxml2::XMLElement *elem, World &world);

    /**
     * @brief Recursively parses a group and its contents from an XML element.
     * @param elem The XML element containing group data.
     * @param group The Group object to populate.
     * @return True on success, false if the element is null.
     */
    static bool parseGroup(tinyxml2::XMLElement *elem, Group &group);

    /**
     * @brief Parses model file references from an XML element.
     * @param elem The XML element containing model data.
     * @param group The Group to which the loaded models will be added.
     * @return Always returns true.
     */
    static bool parseModels(tinyxml2::XMLElement *elem, Group &group);

    /**
     * @brief Parses transformations (translate, rotate, scale) from an XML
     * element.
     * @param elem The XML element containing transformation data.
     * @param group The Group to which the transformations will be added.
     */
    static bool parseTransform(tinyxml2::XMLElement *elem, Group &group);

    /**
     * @brief Parses light definitions from an XML element and adds them to the
     world.
     *
     * @param elem The XML element containing light data.
     * @param world The World object to update.
     * @return True on success, false if the element is null.
     */
    static bool parseLights(tinyxml2::XMLElement *elem, World &world);

    static void parseColorComponent(const tinyxml2::XMLElement *elem,
                                    float out[4]);
};

#endif