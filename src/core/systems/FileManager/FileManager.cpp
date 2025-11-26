#include "FileManager.h"
#include <iostream>
#include <QFileDialog>
#include "../SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../camera/Camera.h"
#include "../../../../external/json/single_include/nlohmann/json.hpp"

FileManager &FileManager::getInstance()
{
    static FileManager instance;
    return instance;
}
FileManager::FileManager() : actualProjectPath("")
{
}
FileManager::~FileManager()
{
}

void FileManager::saveProject()
{
    // the file is not set, we create a new save dialog to choose the path, and then save the entities into the JSON
    if (actualProjectPath.empty())
    {
        createNewProjectSaveFile();
    }
    else
    { // the file is already set, we just save the current project to that path
        saveProjectAs(actualProjectPath);
    }
}

void FileManager::createNewProjectSaveFile()
{
    // open QDialog to choose the path and name of the new save file
    // then call saveProjectAs with the chosen path
    QString newPath = QFileDialog::getSaveFileName(nullptr, "Save Project As", "", "JSON Files (*.json)");
    if (!newPath.isEmpty())
    {
        actualProjectPath = newPath.toStdString() + ".json"; // add .json extension
        saveProjectAs(actualProjectPath);
    }
}

void FileManager::saveProjectAs(const std::string &newProjectPath)
{

    std::cout << "Saving project to: " << newProjectPath << std::endl;
    nlohmann::json jsonData; // JSON object to hold project data

    // Update the actual project path
    actualProjectPath = newProjectPath;

    // setup the fstream to write into the JSON
    std::ofstream fileStream(newProjectPath);
    if (!fileStream.is_open())
    {
        std::cerr << "Error: Could not open file " << newProjectPath << " for writing." << std::endl;
        return;
    }

    // 1 - save name of the project get the name of the file without extension
    std::string projectName;
    size_t lastSlash = newProjectPath.find_last_of("/\\");
    size_t lastDot = newProjectPath.find_last_of(".");
    if (lastDot == std::string::npos || lastDot < lastSlash)
        projectName = newProjectPath.substr(lastSlash + 1);
    else
        projectName = newProjectPath.substr(lastSlash + 1, lastDot - lastSlash - 1);

    jsonData["project_name"] = projectName;

    // 2 - save camera settings
    Camera &camera = Camera::getInstance();
    jsonData["camera"] = {
        {"position", {camera.getPosition().x, camera.getPosition().y, camera.getPosition().z}},
        {"rotation", {camera.getRotationEuler().x, camera.getRotationEuler().y, camera.getRotationEuler().z}},
        {"fov", camera.getFOV()},
        {"near_plane", camera.getNearPlane()},
        {"far_plane", camera.getFarPlane()}};


    // 3 - save shapes and their properties

    std::vector<Shape *> shapes = SceneManager::getInstance().getShapes();
    for (const auto &shape : shapes)
    {
        nlohmann::json shapeJson;
        // shapes common properties
        shapeJson["id"] = shape->getID();
        shapeJson["name"] = shape->getName();
        shapeJson["type"] = static_cast<int>(shape->getType());
        shapeJson["position"] = {shape->getPosition().x, shape->getPosition().y, shape->getPosition().z};
        shapeJson["scale"] = {shape->getScale().x, shape->getScale().y, shape->getScale().z};
        shapeJson["rotation"] = {shape->getRotation().x, shape->getRotation().y, shape->getRotation().z};
        if(shape->getType() == ShapeType::MESH)
        {
            Mesh* meshShape = dynamic_cast<Mesh*>(shape);
            // For mesh, we might want to save additional data like file path, number of triangles, etc.
            // Here we just add a placeholder
            shapeJson["file_path"] = meshShape->getFilename();
        }

        // material properties
        Material *mat = shape->getMaterial();
        if (mat)
        {
            nlohmann::json matJson;
            matJson["id"] = mat->getMaterialId();
            matJson["ambient"] = {mat->getAmbient().x, mat->getAmbient().y, mat->getAmbient().z};
            matJson["diffuse"] = {mat->getDiffuse().x, mat->getDiffuse().y, mat->getDiffuse().z};
            matJson["specular"] = {mat->getSpecular().x, mat->getSpecular().y, mat->getSpecular().z};
            matJson["shininess"] = mat->getShininess();
            matJson["transparency"] = mat->getTransparency();
            matJson["index_medium"] = mat->getIndexMedium();
            matJson["emissive"] = mat->isEmissive();
            matJson["light_color"] = {mat->getLightColor().x, mat->getLightColor().y, mat->getLightColor().z};
            matJson["light_intensity"] = mat->getLightIntensity();
            matJson["has_normal_map"] = mat->hasNormalMap();
            matJson["material_id"] = mat->getMaterialId();
            matJson["texture_scale_x"] = mat->getTextureScaleX();
            matJson["texture_scale_y"] = mat->getTextureScaleY();
            if (!mat->hasTexture())
            {
                matJson["texture"] = mat->getPathFileTexture();
            }
            if (!mat->hasNormalMap())
            {
                matJson["normal_map"] = mat->getPathFileNormalMap();
            }
            shapeJson["material"] = matJson;
        }

        jsonData["shapes"].push_back(shapeJson);
    }

    // 4 TODO aditional data -> rays/pixel, maxrebounce


    // Finally, write the JSON data to the file
    fileStream << jsonData.dump(4); // Pretty print with 4 spaces indentation
    fileStream.close();
}