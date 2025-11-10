#pragma once
#include <vector>
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"

class SceneManager {
public:
    ~SceneManager();
    SceneManager();
    void addShape(Shape* shape);
    void deleteShape(Shape* shape);
    const std::vector<Shape*>& getShapes() const;
    void buildScene();
private:
    std::vector<Shape*> shapes;
    void clearShapes();
};