#pragma once
#include <vector>
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"
#include "../../shapes/Triangle.h"
#include "../../shapes/Mesh.h"

class SceneManager
{
public:
    // Singleton pattern
    static SceneManager &getInstance();

    // Delete copy constructor and assignment operator
    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;

    ~SceneManager();

    void addShape(Shape *shape);
    void deleteShape(Shape *shape);
    inline const std::vector<Shape *> &getShapes() { return shapes; }
    inline const std::vector<Material *> &getMaterials() const { return materials; }
    void updateUniqueMaterials();
    void buildScene();
    void buildScene(const std::string &path);
    void defaultScene();

    inline size_t getNumShapes() const { return shapes.size(); }
    size_t getNumMaterials() const { return materials.size(); }
    Shape *getShapesBuffer() const; // Returns a buffer suitable for GPU consumption
    Shape *getShapeByID(const int &shapeID) const;

private:
    SceneManager(); // Private constructor
    std::vector<Shape *> shapes;
    std::vector<Material *> materials;
    void clearShapes();
};