#pragma once

#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"
#include "../../shapes/Triangle.h"

class MoveShapeCommand : public ICommand {
private:
    SceneManager& sceneManager;
    Shape* shape;
    bool ownsShape;
    int commandID;
    inline static int nextCommandID = 0;
    vec3 previousPosition;
    vec3 newPosition;

public:
    MoveShapeCommand(int shapeID, float newX, float newY, float newZ)
        : sceneManager(SceneManager::getInstance()), ownsShape(false), commandID(nextCommandID++)
    {
        shape = sceneManager.getShapeByID(shapeID);
        if (shape) {
            // Store original position
            previousPosition = shape->getPosition();
            // New position
            newPosition = vec3(newX, newY, newZ);
            
        }
    }

    ~MoveShapeCommand() {
        if (ownsShape && shape) {
            delete shape;
        }
    }
    // set the shape position to newPosition
    void execute() override {
        if (shape) {
            sceneManager.getShapeByID(shape->getID())->setPosition(newPosition);
        }
    }

    // revert the shape position to previousPosition
    void undo() override {
        if (shape) {
            sceneManager.getShapeByID(shape->getID())->setPosition(previousPosition);
        }
    }

    int getID() const override {
        return commandID;
    }
};
