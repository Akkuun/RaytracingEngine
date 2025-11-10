#pragma once
#include <vector>
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"

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
    
private:
    SceneManager(); // Private constructor
    std::vector<Shape*> shapes;
    void clearShapes();
};