#pragma once
#include <vector>
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"
#include "../../shapes/Triangle.h"
#include "../../shapes/Mesh.h"

class SceneManager {
public:
    // Singleton pattern
    static SceneManager& getInstance();
    
    // Delete copy constructor and assignment operator
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    
    ~SceneManager();
    
    void addShape(Shape* shape);
    void deleteShape(Shape* shape);
    const std::vector<Shape*>& getShapes() const;
    void buildScene();
    
    inline size_t getNumShapes() const { return shapes.size(); }
    Shape* getShapesBuffer() const; // Returns a buffer suitable for GPU consumption

private:
    SceneManager(); // Private constructor
    std::vector<Shape*> shapes;
    void clearShapes();    
};