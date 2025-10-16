#pragma once
#include <vector>
#include "../../shapes/Shape.h"

class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager() = default;
    void addShape(Shape* shape);
    void deleteShape(Shape* shape);
    const std::vector<Shape*>& getShapes() const;
private:
    std::vector<Shape*> shapes;
};