#pragma once

#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"
#include "../../shapes/Triangle.h"

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
    }

    void undo() override {
        sceneManager.addShape(shape);
        ownsShape = false;
    }

    int getID() const override {
        return commandID;
    }
};
