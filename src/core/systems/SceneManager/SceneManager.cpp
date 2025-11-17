#include "SceneManager.h"
#include <algorithm>

// Singleton getInstance method
SceneManager& SceneManager::getInstance() {
    static SceneManager instance;
    return instance;
}

SceneManager::SceneManager(){
    buildScene(); // TODO create 2nd version with buildScene(path) to get all the shape from a single file
}

SceneManager::~SceneManager() {
    // Clean up allocated shapes
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
}

void SceneManager::addShape(Shape* shape) {
    shapes.push_back(shape);
}
void SceneManager::deleteShape(Shape* shape) {
    shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
}
const std::vector<Shape*>& SceneManager::getShapes() const {
    return shapes;
}

void SceneManager::clearShapes() {
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
}

// build the scene ie. Cornell Box
void SceneManager::buildScene() {
    clearShapes();
    
    // Sphere 1 - white sphere
    addShape(new Sphere(0.15f, vec3(0.25f, -0.2f, -1.25f), vec3(0.9f, 0.9f, 0.9f),"Boule 1"));

    // Sphere 2 - white sphere
    addShape(new Sphere(0.1f, vec3(-0.25f, -0.25f, -2.25f), vec3(0.95f, 0.95f, 0.95f),"Boule 2"));

    // Floor - white
    addShape(new Square(
        vec3(0.0f, -0.5f, 0.0f),        // pos
        vec3(1.5f, 0.0f, 0.0f),          // u_vec
        vec3(0.0f, 0.0f, -80.0f),        // v_vec
        vec3(0.0f, 1.0f, 0.0f),          // normal
        vec3(0.9f, 0.9f, 0.9f),           // color
        "Floor"                          // name
    ));
    
    // Ceiling - white (no name)
    addShape(new Square(
        vec3(0.0f, 0.35f, 3.0f),         // pos
        vec3(1.5f, 0.0f, 0.0f),          // u_vec
        vec3(0.0f, 0.0f, -80.0f),        // v_vec
        vec3(0.0f, -1.0f, 0.0f),         // normal
        vec3(0.9f, 0.9f, 0.9f)          // color
    ));
    
    // Left wall - red
    addShape(new Square(
        vec3(-0.75f, 0.0f, 0.0f),        // pos
        vec3(0.0f, 1.5f, 0.0f),          // u_vec
        vec3(0.0f, 0.0f, -80.0f),        // v_vec
        vec3(1.0f, 0.0f, 0.0f),          // normal
        vec3(0.9f, 0.1f, 0.1f),         // color - red
        "Left Wall"                     // name
    ));

    // Right wall - green
    addShape(new Square(
        vec3(0.75f, 0.0f, 0.0f),         // pos
        vec3(0.0f, 1.5f, 0.0f),          // u_vec
        vec3(0.0f, 0.0f, -80.0f),        // v_vec
        vec3(-1.0f, 0.0f, 0.0f),         // normal
        vec3(0.1f, 0.9f, 0.1f),         // color - green
        "Right Wall"                    // name
    ));

    // Back wall - white
    addShape(new Square(
        vec3(0.0f, 0.0f, -60.0f),        // pos
        vec3(5.0f, 0.0f, 0.0f),          // u_vec
        vec3(0.0f, 1.5f, 0.0f),          // v_vec
        vec3(0.0f, 0.0f, 1.0f),          // normal
        vec3(0.9f, 0.9f, 0.9f),         // color
        "Back Wall"                     // name
    ));

    // Triangle - blue
    addShape(new Triangle(
        vec3(-0.4f, 0.0f, -1.5f),        // vertex A
        vec3(0.2f, 0.0f, 1.0f),         // vertex B
        vec3(0.0f, 0.3f, 0.0f),         // vertex C
        vec3(0.9f, 0.9f, 0.1f),         // color - yellow
        "Triangle"                     // name
    ));

    Mesh* mesh = new Mesh("../assets/tripod.off");
    //mesh->scale(vec3(0.4f));
    mesh->translate(vec3(0.0f, 0.0f, -10.0f));
    mesh->rotate(vec3(180.0f * 0.0174533f, 0.0f, 0.0f));
    mesh->generateCpuTriangles();
    addShape(mesh);
}
// return the array of shapes suitable for kernel code
Shape* SceneManager::getShapesBuffer() const{
    if (shapes.empty()) return nullptr;

    // Allocate a contiguous buffer for all shapes
    Shape* shapesArray = new Shape[shapes.size()];
    for (size_t i = 0; i < shapes.size(); ++i) {
        shapesArray[i] = *(shapes[i]);
    }
    return shapesArray;
}
