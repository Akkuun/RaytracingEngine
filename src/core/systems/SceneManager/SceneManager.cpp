#include "SceneManager.h"
#include <algorithm>

// Singleton getInstance method
SceneManager &SceneManager::getInstance()
{
    static SceneManager instance;
    return instance;
}

SceneManager::SceneManager()
{
    buildScene(); // TODO create 2nd version with buildScene(path) to get all the shape from a single file
}

SceneManager::~SceneManager()
{
    // Clean up allocated shapes
    for (Shape *shape : shapes)
    {
        delete shape;
    }
    shapes.clear();
}

void SceneManager::addShape(Shape *shape)
{
    shapes.push_back(shape);
}
void SceneManager::deleteShape(Shape *shape)
{
    shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
}

void SceneManager::clearShapes()
{
    // Clear materials vector first to avoid dangling pointers
    materials.clear();
    
    // Now delete shapes (which will also delete their materials)
    for (Shape *shape : shapes)
    {
        delete shape;
    }
    shapes.clear();
}

// build the scene ie. Cornell Box
void SceneManager::buildScene()
{
    clearShapes();

    // Sphere 1 - Diffuse Material  PINK
    addShape(new Sphere(
        0.15f,
         vec3(0.25f, -0.2f, -0.25f),
          vec3(0.9f, 0.9f, 0.9f),
           "Boule 1",
            new Material(vec3(1.f, 0.3f, 1.f))));

    // Sphere 2 - Texture Material
    addShape(new Sphere(0.1f, vec3(-0.25f, -0.25f, -0.25f), vec3(0.95f, 0.95f, 0.95f), "Boule 2", new Material("../assets/textures/earth.ppm")));

    // Floor - white
    addShape(new Square(
        vec3(0.0f, -0.35f, 0.0f),  // pos
        vec3(1.5f, 0.0f, 0.0f),   // u_vec
        vec3(0.0f, 0.0f, 1.5f), // v_vec
        vec3(0.0f, 1.0f, 0.0f),   // normal
        vec3(0.9f, 0.9f, 0.9f),   // color
        "Floor",                   // name
        new Material("../assets/textures/white_pool_tiles.ppm")
        ));

    // Ceiling - white (no name)
    addShape(new Square(
        vec3(0.0f, 0.35f, 0.0f),  // pos
        vec3(1.5f, 0.0f, 0.0f),   // u_vec
        vec3(0.0f, 0.0f, 1.5f), // v_vec
        vec3(0.0f, -1.0f, 0.0f),  // normal
        vec3(0.9f, 0.9f, 0.9f),   // color
        "Ceiling",                 // name
        new Material("../assets/textures/metal.ppm")
    ));

    // Left wall - red
    addShape(new Square(
        vec3(-0.75f, 0.0f, 0.0f), // pos
        vec3(0.0f, 1.5f, 0.0f),   // u_vec
        vec3(0.0f, 0.0f, 1.5f), // v_vec
        vec3(1.0f, 0.0f, 0.0f),   // normal
        vec3(0.9f, 0.1f, 0.1f),   // color - red
        "Right Wall",               // name
        new Material("../assets/textures/brickwall.ppm")
    ));

    // Right wall - green
    addShape(new Square(
        vec3(0.75f, 0.0f, 0.0f),  // pos
        vec3(0.0f, 1.5f, 0.0f),   // u_vec
        vec3(0.0f, 0.0f, 1.5f), // v_vec
        vec3(-1.0f, 0.0f, 0.0f),  // normal
        vec3(0.1f, 0.9f, 0.1f),   // color - green
        "Left Wall",              // name
        new Material("../assets/textures/brickwall.ppm")
    ));

    // Back wall - white
    addShape(new Square(
        vec3(0.0f, 0.0f, 0.0f), // pos
        vec3(1.5f, 0.0f, 0.0f),   // u_vec
        vec3(0.0f, 1.5f, 0.0f),   // v_vec
        vec3(0.0f, 0.0f, -1.0f),   // normal
        vec3(0.9f, 0.9f, 0.9f),   // color
        "Back Wall",               // name
        new Material("../assets/textures/white_pool_tiles.ppm")
        ));

    // Triangle - blue
    addShape(new Triangle(
        vec3(-0.1f, 0.2f, -0.3f), // vertex A
        vec3(0.0f, 0.3f, -0.3f),   // vertex C
        vec3(-0.0f, 0.2f, -0.3f),   // vertex B
        vec3(0.9f, 0.9f, 0.1f),   // color - yellow
        "Triangle"                // name
    ));

    Mesh *mesh = new Mesh("../assets/models3D/tripod.off");
    mesh->scale(vec3(0.4f));
    // mesh->translate(vec3(0.0f, 0.0f, 2.0f));
    mesh->rotate(vec3(180.0f * 0.0174533f, 0.0f, 0.0f));
    mesh->generateCpuTriangles();
    addShape(mesh);
}
// return the array of shapes suitable for kernel code
Shape *SceneManager::getShapesBuffer() const
{
    if (shapes.empty())
        return nullptr;

    // Allocate a contiguous buffer for all shapes
    Shape *shapesArray = new Shape[shapes.size()];
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        shapesArray[i] = *(shapes[i]);
    }
    return shapesArray;
}

Shape *SceneManager::getShapeByID(const int &shapeID) const
{
    if (shapeID < 0 || static_cast<size_t>(shapeID) >= shapes.size())
    {
        return nullptr; // Invalid ID
    }
    return shapes[shapeID];
}


// push all the unique materials used to avoid duplications in GPU
void SceneManager::updateUniqueMaterials()
{
    materials.clear();
    for (const auto &shape : shapes)
    {
        Material *mat = shape->getMaterial();
        if (mat != nullptr && std::find(materials.begin(), materials.end(), mat) == materials.end())
        {
            materials.push_back(mat);
        }
    }
}