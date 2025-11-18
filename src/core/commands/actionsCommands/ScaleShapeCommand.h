#pragma once

#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"
#include "../../shapes/Triangle.h"

class ScaleShapeCommand : public ICommand {
private:
    SceneManager& sceneManager;
    Shape* shape;
    bool ownsShape;
    int commandID;
    inline static int nextCommandID = 0;
    vec3 previousScale;
    vec3 newScale;

public:
    ScaleShapeCommand(int shapeID, float newX, float newY, float newZ)
        : sceneManager(SceneManager::getInstance()), ownsShape(false), commandID(nextCommandID++)
    {
        shape = sceneManager.getShapeByID(shapeID);
        if (shape) {
            // Store original scale
            previousScale = shape->getScale();
            // New scale
            newScale = vec3(newX, newY, newZ);

        }
    }

    ~ScaleShapeCommand() {
        if (ownsShape && shape) {
            delete shape;
        }
    }
    // set the shape scale to newScale
    void execute() override {
        if (shape) {
            sceneManager.getShapeByID(shape->getID())->setScale(newScale);
        }
    }

    // revert the shape scale to previousScale
    void undo() override {
        if (shape) {
            sceneManager.getShapeByID(shape->getID())->setScale(previousScale);
        }
    }

    int getID() const override {
        return commandID;
    }
};
