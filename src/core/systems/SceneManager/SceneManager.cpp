#include "SceneManager.h"
#include <algorithm>
void SceneManager::addShape(Shape* shape) {
    shapes.push_back(shape);
}
void SceneManager::deleteShape(Shape* shape) {
    shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
}
const std::vector<Shape*>& SceneManager::getShapes() const {
    return shapes;
}
