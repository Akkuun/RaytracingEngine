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
    updateRecentProjectsList(); // add the project to recent projects list to future menu opening
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

    currentProjectName = projectName;
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

        // Based on shape type, save specific properties
        switch (shape->getType())
        {
            case ShapeType::SPHERE:
            {
                Sphere *sphere = dynamic_cast<Sphere *>(shape);
                if (sphere)
                {
                    shapeJson["radius"] = sphere->getRadius();
                    shapeJson["center"] = {sphere->getCenter().x, sphere->getCenter().y, sphere->getCenter().z};
                }
                break;
            }
            case ShapeType::SQUARE:
            {
                Square *square = dynamic_cast<Square *>(shape);
                if (square)
                {
                    shapeJson["u_vec"] = {square->getUVector().x, square->getUVector().y, square->getUVector().z};
                    shapeJson["v_vec"] = {square->getVVector().x, square->getVVector().y, square->getVVector().z};
                    shapeJson["normal"] = {square->getNormal().x, square->getNormal().y, square->getNormal().z};
                }
                break;
            }

            case ShapeType::TRIANGLE:
            {
                Triangle *triangle = dynamic_cast<Triangle *>(shape);
                if (triangle)
                {
                    shapeJson["vertexA"] = {triangle->getV0().x, triangle->getV0().y, triangle->getV0().z};
                    shapeJson["vertexB"] = {triangle->getV2().x, triangle->getV2().y, triangle->getV2().z}; // Switch v1 and v2 to fix normal
                    shapeJson["vertexC"] = {triangle->getV1().x, triangle->getV1().y, triangle->getV1().z};
                }
                break;
            }
            case ShapeType::MESH:
            {
                Mesh *mesh = dynamic_cast<Mesh *>(shape);
                if (mesh)
                {
                    shapeJson["file_path"] = mesh->getFilename();
                }
                break;
            }
            default:
                std::cerr << "Warning: Unknown shape type while saving project." << std::endl;
                break;
        }

        jsonData["shapes"].push_back(shapeJson);
    }

    // TODO aditional data -> rays/pixel, maxrebounce

    // Finally, write the JSON data to the file
    fileStream << jsonData.dump(4); // Pretty print with 4 spaces indentation
    fileStream.close();
}

void FileManager::updateRecentProjectsList()
{

    std::string recentProjectsPath = "../saves/recentProject.json";

    std::cout << "Updating recent projects list at: " << recentProjectsPath << std::endl;

    // Read or create the recent projects JSON file
    nlohmann::json recentProjectsJson;
    std::ifstream file(recentProjectsPath);
    if (!file)
    {
        // File doesn't exist, create an empty array
        recentProjectsJson = nlohmann::json::array();
        std::ofstream outFile(recentProjectsPath);
        outFile << recentProjectsJson.dump(4);
        outFile.close();
    }
    else
    {
        file >> recentProjectsJson;
        file.close();
    }

    // Add the current project path and date to recentProject.json
    std::string currentDate;
    {
        // Get current date/time as string
        std::time_t now = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        currentDate = buf;
    }
    nlohmann::json entry = {
        {"name", currentProjectName},
        {"path", actualProjectPath},
        {"date", currentDate}};
    // Remove any previous entry for this path
    for (auto it = recentProjectsJson.begin(); it != recentProjectsJson.end();)
    {
        if ((*it)["path"] == actualProjectPath)
        {
            it = recentProjectsJson.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // Add new entry at the front (most recent)
    recentProjectsJson.insert(recentProjectsJson.begin(), entry);
    // Optionally limit the number of recent projects
    const int maxRecent = 20;
    while (recentProjectsJson.size() > maxRecent)
    {
        recentProjectsJson.erase(recentProjectsJson.end() - 1);
    }
    // Write back to file
    std::ofstream outFile(recentProjectsPath);
    outFile << recentProjectsJson.dump(4);
    outFile.close();
}