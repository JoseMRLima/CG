/**
 * @file Model.hpp
 * @brief Defines the Model class for representing and rendering 3D objects.
 *
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Point.hpp"
#include <string>
#include <vector>

/**
 * @brief Texture coordinate pair in [0,1]².
 */
struct UV {
    float u, v;
};

/**
 * @brief Holds the OpenGL material properties for a model.
 */
struct Material {
    float diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    float specular[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float emissive[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float shininess = 0.0f;
};

/**
 * @brief Represents a 3D model, handling its vertices, buffer objects, and
 * drawing.
 */
class Model {
  private:
    std::vector<Point> vertices; ///< Stores all the vertex points of the model.
    std::vector<Vector> normals; ///< Normal of each vertex.
    std::vector<UV> uvs;         ///< Texture coordinates of each vertex.
    std::vector<float> vbo;      ///< Vertex data for the VBO.
    std::vector<unsigned int> ibo; ///< Index data for the IBO.
    unsigned int _vbo;             ///< ID for the Vertex Buffer Object.
    unsigned int _ibo;             ///< ID for the Index Buffer Object.
    std::string textureFile;       ///< Path to the texture image, or empty.
    unsigned int _texture = 0;     ///< OpenGL texture handle (0 = none).
    Material material;             ///< Material colour properties.

  public:
    /**
     * @brief Default constructor for the Model class.
     */
    Model() = default;

    /**
     * @brief Loads model vertices from a specified file.
     * @param filename The path to the file to load the model from.
     * @return True if loading is successful, false otherwise.
     */
    bool loadFromFile(const std::string &filename);

    /**
     * @brief Adds the vertices of a triangle to the model.
     * @param ps A vector containing the three points of the triangle.
     * @param ns The three vertex normals.
     * @param uvs The three vertex texture coordinates.
     */
    void addTriangule(std::vector<Point> ps, std::vector<Vector> ns,
                      std::vector<UV> tests);

    /**
     * @brief Saves the model's vertices to a specified file.
     * @param filename The path to the file where the model will be saved.
     * @return True if saving is successful, false otherwise.
     */
    bool toFile(const std::string &filename);

    /**
     * @brief Draws the model using the configured VBO and IBO.
     */
    void draw() const;

    /**
     * @brief Sets up the VBO and IBO for rendering.
     */
    void setup();

    /**
     * @brief Sets the texture file path. Call before setup().
     * @param path Path to the image file
     */
    void setTextureFile(const std::string &path) { textureFile = path; }

    /**
     * @brief Sets the material colour properties. Call before setup().
     * @param m The material to apply.
     */
    void setMaterial(const Material &m) { material = m; }

    /**
     * @brief Uploads the texture image to the GPU.
     * Must be called after an OpenGL context exists (i.e. inside setup()).
     */
    void loadTexture();
};

#endif