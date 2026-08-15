/**
 * @file main.cpp
 * @brief Main entry point for the shape generator program.
 *
 */

#include "shapes.hpp"

#include <fstream>
#include <iostream>

/**
 * @brief Displays usage information for the command-line tool.
 *
 * @param command The name of the executable.
 */
static void usage(const char *command) {
    std::cerr << "Usage: " << command << " <type> [parameters...] <filename>\n";
    std::cerr << "> types and parameters:\n";
    std::cerr << "\tplane <length> <divisions> <filename>\n";
    std::cerr << "\tbox <dimension> <divisions> <filename>\n";
    std::cerr << "\tsphere <radius> <slices> <stacks> <filename>\n";
    std::cerr << "\tcone <radius> <height> <slices> <stacks> <filename>\n";
    std::cerr << "\tcylinder <radius> <height> <slices> <filename>\n";
    std::cerr
        << "\tring <major radius> <minor radius> <sides> <rings> <filename>\n";
    std::cerr << "\tpatch <bezier patch> <tesselation> <filename>\n";
}

/**
 * @brief The main function of the shape generator.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return int Returns 0 on success, non-zero on failure.
 */
int main(int argc, char **argv) {
    if (argc < 4) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    std::string type = argv[1];

    if (type == "plane") {
        if (argc != 5) {
            usage(argv[0]);
            return 1;
        }

        float length = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        std::string filename = argv[4];

        std::cout << "Generating plane: "
                  << "[length = " << length << ", divisions = " << divisions
                  << ", file = '" << filename << "']\n";

        if (!generatePlane(length, divisions, filename)) {
            return EXIT_FAILURE;
        }
    } else if (type == "box") {
        if (argc != 5) {
            usage(argv[0]);
            return 1;
        }

        float dimension = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        std::string filename = argv[4];

        std::cout << "Generating box: "
                  << "[dimension = " << dimension
                  << ", divisions = " << divisions << ", file = '" << filename
                  << "']\n";

        if (!generateBox(dimension, divisions, filename)) {
            return EXIT_FAILURE;
        }
    } else if (type == "sphere") {
        if (argc != 6) {
            usage(argv[0]);
            return 1;
        }

        float radius = std::stof(argv[2]);
        int slices = std::stoi(argv[3]);
        int stacks = std::stoi(argv[4]);
        std::string filename = argv[5];

        std::cout << "Generating sphere: "
                  << "[radius = " << radius << ", slices = " << slices
                  << ", stacks = " << stacks << ", file = '" << filename
                  << "']\n";

        if (!generateSphere(radius, slices, stacks, filename)) {
            return EXIT_FAILURE;
        }
    } else if (type == "cone") {
        if (argc != 7) {
            usage(argv[0]);
            return 1;
        }

        float radius = std::stof(argv[2]);
        float height = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stacks = std::stoi(argv[5]);
        std::string filename = argv[6];

        std::cout << "Generating cone: "
                  << "[radius = " << radius << ", height = " << height
                  << ", slices = " << slices << ", stacks = " << stacks
                  << ", file = '" << filename << "']\n";

        if (!generateCone(radius, height, slices, stacks, filename)) {
            return EXIT_FAILURE;
        }
    } else if (type == "cylinder") {
        if (argc != 6) {
            usage(argv[0]);
            return 1;
        }

        float radius = std::stof(argv[2]);
        float height = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        std::string filename = argv[5];

        std::cout << "Generating cylinder: "
                  << "[radius = " << radius << ", height = " << height
                  << ", slices = " << slices << ", file = '" << filename
                  << "']\n";

        if (!generateCylinder(radius, height, slices, filename)) {
            return EXIT_FAILURE;
        }
    } else if (type == "ring") {
        if (argc != 7) {
            usage(argv[0]);
            return 1;
        }

        float majorRadius = std::stof(argv[2]);
        float minorRadius = std::stof(argv[3]);
        int sides = std::stoi(argv[4]);
        int rings = std::stoi(argv[5]);
        std::string filename = argv[6];

        std::cout << "Generating ring: "
                  << "[major radius = " << majorRadius
                  << ", minor radius = " << minorRadius << ", sides = " << sides
                  << ", rings = " << rings << ", file = '" << filename
                  << "']\n";

        if (!generateRing(majorRadius, minorRadius, sides, rings, filename)) {
            return EXIT_FAILURE;
        }
    } else if (type == "patch") {
        if (argc != 5) {
            usage(argv[0]);
            return 1;
        }

        std::string bezierPatch = argv[2];
        int tesselation = std::stoi(argv[3]);
        std::string filename = argv[4];

        std::cout << "Generating patch: "
                  << "[bezier patch = " << bezierPatch
                  << ", tesselation = " << tesselation << ", file = '"
                  << filename << "']\n";

        if (!generatePatch(bezierPatch, tesselation, filename)) {
            return EXIT_FAILURE;
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    return 0;
}
