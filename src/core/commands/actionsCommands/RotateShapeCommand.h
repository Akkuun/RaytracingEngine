#pragma once

#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"
#include "../../shapes/Triangle.h"

class RotateShapeCommand : public ICommand {
private:
    SceneManager& sceneManager;
    Shape* shape;
    bool ownsShape;
    int commandID;
    inline static int nextCommandID = 0;
    vec3 previousRotation;
    vec3 newRotation;

public:
    RotateShapeCommand(int shapeID, float newX, float newY, float newZ)
        : sceneManager(SceneManager::getInstance()), ownsShape(false), commandID(nextCommandID++)
    {
        shape = sceneManager.getShapeByID(shapeID);
        if (shape) {
            // Store original rotation
            previousRotation = shape->getRotation();
            // New rotation
            newRotation = vec3(newX, newY, newZ);
        }
    }

    ~RotateShapeCommand() {
        if (ownsShape && shape) {
            delete shape;
        }
    }
    // set the shape rotation to newRotation
    void execute() override {
        if (shape) {
            sceneManager.getShapeByID(shape->getID())->setRotation(newRotation);
        }
        if (shape->getType() == ShapeType::MESH)
        {
            Mesh *mesh = static_cast<Mesh *>(shape);
            mesh->rotate(newRotation - previousRotation); // Rotate all vertices
            mesh->generateCpuTriangles();
        }
    }

    // revert the shape rotation to previousRotation
    void undo() override {
        if (shape) {
            sceneManager.getShapeByID(shape->getID())->setRotation(previousRotation);
        }
        if (shape->getType() == ShapeType::MESH)
        {
            Mesh *mesh = static_cast<Mesh *>(shape);
            mesh->rotate(previousRotation - newRotation); // Rotate all vertices
            mesh->generateCpuTriangles();
        }
    }

    int getID() const override {
        return commandID;
    }
};
