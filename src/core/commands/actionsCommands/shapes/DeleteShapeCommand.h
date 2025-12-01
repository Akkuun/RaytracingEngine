#pragma once

#include "../../../shapes/Shape.h"
#include "../../../shapes/Sphere.h"
#include "../../../shapes/Square.h"
#include "../../../shapes/Triangle.h"
#include "../../../systems/SceneManager/SceneManager.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class DeleteShapeCommand : public ICommand {
private:
    SceneManager& sceneManager;
    Shape* shape;
    bool ownsShape;
    int commandID;
    inline static int nextCommandID = 0;

public:
    DeleteShapeCommand(Shape* shape)
        : sceneManager(SceneManager::getInstance())
        , shape(shape)
        , ownsShape(false)
        , commandID(nextCommandID++) {}
    
    ~DeleteShapeCommand() {
        if (ownsShape && shape) {
            delete shape;
        }
    }

    void execute() override {
        sceneManager.deleteShape(shape);
        ownsShape = true;
        CommandsManager::getInstance().notifyShapesChanged();

        // notify BVH changed ONLY if the shape is a mesh
        if (shape->getType() == ShapeType::MESH) {
            CommandsManager::getInstance().notifyBVHChanged();
        }
    }

    void undo() override {
        sceneManager.addShape(shape);
        ownsShape = false;
        CommandsManager::getInstance().notifyShapesChanged();

        // notify BVH changed ONLY if the shape is a mesh
        if (shape->getType() == ShapeType::MESH) {
            CommandsManager::getInstance().notifyBVHChanged();
        }
    }

    int getID() const override {
        return commandID;
    }
};
