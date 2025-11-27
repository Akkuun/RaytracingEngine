#pragma once

#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../shapes/Sphere.h"
#include "../../shapes/Square.h"
#include "../../shapes/Triangle.h"
#include "../CommandsManager.h"

class AddShapeCommand : public ICommand {
private:
    SceneManager& sceneManager;
    Shape* shape;
    bool ownsShape;
    int commandID;
    inline static int nextCommandID = 0;

public:
    AddShapeCommand(Shape* shape) 
        : sceneManager(SceneManager::getInstance())
        , shape(shape)
        , ownsShape(true)
        , commandID(nextCommandID++) {}
    
    ~AddShapeCommand() {
        if (ownsShape && shape) {
            delete shape;
        }
    }

    void execute() override {
        sceneManager.addShape(shape);
        ownsShape = false;
        CommandsManager::getInstance().notifyShapesChanged();
    }

    void undo() override {
        sceneManager.deleteShape(shape);
        ownsShape = true;
        CommandsManager::getInstance().notifyShapesChanged();
    }

    int getID() const override {
        return commandID;
    }
};
