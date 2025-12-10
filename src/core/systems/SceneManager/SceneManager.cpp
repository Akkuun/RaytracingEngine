#include "SceneManager.h"
#include <algorithm>
#include "../FileManager/FileManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

// Singleton getInstance method
SceneManager &SceneManager::getInstance()
{
    static SceneManager instance;
    return instance;
}

SceneManager::SceneManager()
{
    buildScene(); // Initialize with default scene or loaded scene
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

    if (FileManager::getInstance().getIsNewProjectSelected())
    {
        //defaultScene();
        cornellScene();
        return;
    }
    else
    {
        std::cout << "load file from path: " << FileManager::getInstance().getActualProjectPath() << std::endl;
        // create scene from file
        buildScene(FileManager::getInstance().getActualProjectPath());
    }
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

void SceneManager::buildScene(const std::string &path)
{
    // read the JSON data of the scene file located at 'path' with nlohmann::json
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open scene file: " << path << std::endl;
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;

    // print shapes data from json for debugging
    for (const auto &shapeJson : jsonData["shapes"])
    {
        int shapeType = shapeJson["type"];
        bool hasMaterial = shapeJson.contains("material");
        vec3 position = vec3(shapeJson["position"][0], shapeJson["position"][1], shapeJson["position"][2]);
        vec3 rotation = vec3(shapeJson["rotation"][0], shapeJson["rotation"][1], shapeJson["rotation"][2]);
        vec3 scale = vec3(shapeJson["scale"][0], shapeJson["scale"][1], shapeJson["scale"][2]);

        Shape *shape = nullptr;
        Material *material = nullptr;
        if (hasMaterial)
        {
            material = new Material(shapeJson["material"]);
        }
        if (shapeType == ShapeType::SPHERE)
        {

            shape = new Sphere(
                shapeJson["radius"],
                vec3(shapeJson["center"][0], shapeJson["center"][1], shapeJson["center"][2]),
                shapeJson["name"], material);
        }
        else if (shapeType == ShapeType::SQUARE)
        {
            shape = new Square(
                vec3(shapeJson["position"][0], shapeJson["position"][1], shapeJson["position"][2]),
                vec3(shapeJson["u_vec"][0], shapeJson["u_vec"][1], shapeJson["u_vec"][2]),
                vec3(shapeJson["v_vec"][0], shapeJson["v_vec"][1], shapeJson["v_vec"][2]),
                vec3(shapeJson["normal"][0], shapeJson["normal"][1], shapeJson["normal"][2]),
                shapeJson["name"], material);
        }
        else if (shapeType == ShapeType::TRIANGLE)
        {
            shape = new Triangle(
                shapeJson["name"],
                vec3(shapeJson["vertexA"][0], shapeJson["vertexA"][1], shapeJson["vertexA"][2]),
                vec3(shapeJson["vertexC"][0], shapeJson["vertexC"][1], shapeJson["vertexC"][2]),
                vec3(shapeJson["vertexB"][0], shapeJson["vertexB"][1], shapeJson["vertexB"][2]));
        }
        else if (shapeType == ShapeType::MESH)
        {
            Mesh *mesh = new Mesh(shapeJson["file_path"]);
            // specify transformations for mesh
            mesh->scale(scale);
            mesh->rotate(rotation);
            mesh->translate(position);
            mesh->generateCpuTriangles();
            shape = mesh;
        }
        else
        {
            std::cerr << "Unknown shape type: " << shapeType << std::endl;
        }
        addShape(shape);
        shape->setPosition(position);
        shape->setRotation(rotation);
        shape->setScale(scale);
    }
}

void SceneManager::defaultScene()
{
    clearShapes();

    // Floor - white
    addShape(new Square(
        vec3(0.0f, -0.35f, 0.0f), // pos
        vec3(1.5f, 0.0f, 0.0f),   // u_vec
        vec3(0.0f, 0.0f, 1.5f),   // v_vec
        vec3(0.0f, 1.0f, 0.0f),   // normal
        "Floor"                   // name
        ));

    // Sphere 1 on top of the floor  with nothing special
    addShape(new Sphere(
        0.15f,
        vec3(0.25f, -0.2f, -0.25f),
        "Boule 1"));
}

void SceneManager::cornellScene()
{
    clearShapes();

    Material *pinkMat = new Material(vec3(1.f, 0.3f, 1.f));
    pinkMat->setTransparency(1.0f);
    pinkMat->setIndexMedium(1.5f);

    // Sphere 1 - Diffuse Material  PINK
    addShape(new Sphere(
        0.15f,
        vec3(0.25f, -0.2f, -0.25f),
        "Boule 1",
        pinkMat));

    Material *earthMat = new Material(std::string("../assets/textures/earth.ppm"));
    earthMat->setMetalness(1.0f);

    // Sphere 2 - Texture Material
    addShape(new Sphere(
        0.1f,                                         // radius
        vec3(-0.25f, -0.25f, -0.25f),                 // center
        "Boule 2",                                    // name
        earthMat                                     // Earth texture
    ));

    Material* poolMat = new Material(std::string("../assets/textures/white_pool_tiles.ppm"));
    poolMat->setNormalsFromPath(std::string("../assets/normals/pool_tiles_n.ppm"));
    poolMat->setMetalness(0.75f);

    // Floor - white
    addShape(new Square(
        vec3(0.0f, -0.35f, 0.0f), // pos
        vec3(1.5f, 0.0f, 0.0f),   // u_vec
        vec3(0.0f, 0.0f, 1.5f),   // v_vec
        vec3(0.0f, 1.0f, 0.0f),   // normal
        "Floor",                  // name
        poolMat));
    
    Material* metalMat = new Material(std::string("../assets/textures/rustediron.ppm"));
    metalMat->setNormalsFromPath(std::string("../assets/normals/rustediron_n.ppm"));
    metalMat->setMetallicFromPath(std::string("../assets/metallics/rustediron_m.ppm"));

        // Ceiling - white (no name)
    addShape(new Square(
        vec3(0.0f, 0.35f, 0.0f), // pos
        vec3(1.5f, 0.0f, 0.0f),  // u_vec
        vec3(0.0f, 0.0f, 1.5f),  // v_vec
        vec3(0.0f, -1.0f, 0.0f), // normal
        "Ceiling",              
        metalMat
    ));
            
    Material *brickwallMat = new Material(std::string("../assets/textures/brickwall.ppm"));
    brickwallMat->setNormalsFromPath(std::string("../assets/normals/brickwall_n.ppm"));

    // Left wall - red
    addShape(new Square(
        vec3(-0.75f, 0.0f, 0.0f), // pos
        vec3(0.0f, 1.5f, 0.0f),   // u_vec
        vec3(0.0f, 0.0f, 1.5f),   // v_vec
        vec3(1.0f, 0.0f, 0.0f),   // normal
        "Right Wall",             // name
        brickwallMat));

    // Create a second brickwall material for the other wall (to avoid double-free)
    Material *brickwallMat2 = new Material(std::string("../assets/textures/brickwall.ppm"));
    brickwallMat2->setNormalsFromPath(std::string("../assets/normals/brickwall_n.ppm"));

    // Right wall - green
    addShape(new Square(
        vec3(0.75f, 0.0f, 0.0f), // pos
        vec3(0.0f, 1.5f, 0.0f),  // u_vec
        vec3(0.0f, 0.0f, 1.5f),  // v_vec
        vec3(-1.0f, 0.0f, 0.0f), // normal
        "Left Wall",             // name
        brickwallMat2));

    // Create a second pool material for the back wall (to avoid double-free)
    Material* poolMat2 = new Material(std::string("../assets/textures/white_pool_tiles.ppm"));
    poolMat2->setNormalsFromPath(std::string("../assets/normals/pool_tiles_n.ppm"));
    poolMat2->setMetalness(0.75f);

    // Back wall - white
    addShape(new Square(
        vec3(0.0f, 0.0f, 0.0f),  // pos
        vec3(1.5f, 0.0f, 0.0f),  // u_vec
        vec3(0.0f, 1.5f, 0.0f),  // v_vec
        vec3(0.0f, 0.0f, -1.0f), // normal
        "Back Wall",             // name
        poolMat2));

    // Triangle - blue
    addShape(new Triangle(
        "Triangle",               // name
        vec3(-0.1f, 0.2f, -0.3f), // vertex A
        vec3(0.0f, 0.3f, -0.3f),  // vertex C
        vec3(-0.0f, 0.2f, -0.3f)  // vertex B
        ));

    Mesh *mesh = new Mesh(std::string("../assets/models3D/tripod.off"));
    mesh->scale(vec3(0.4f));
    mesh->translate(vec3(-0.3f, 0.0f, -0.1f));
    mesh->rotate(vec3(180.0f * 0.0174533f, 0.0f, 0.0f));
    mesh->generateCpuTriangles();
    addShape(mesh);
}