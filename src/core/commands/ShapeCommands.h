#pragma once

#include "ICommand.h"
#include "../systems/SceneManager/SceneManager.h"
#include "../shapes/Shape.h"
#include "../shapes/Sphere.h"
#include "../shapes/Square.h"
#include "../shapes/Triangle.h"

class AddShapeCommand : public ICommand {
private:
    SceneManager& sceneManager;
    Shape* shape;
    bool ownsShape;
    int commandID;
    static int nextCommandID;

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
    }

    void undo() override {
        sceneManager.deleteShape(shape);
        ownsShape = true;
    }

    int getID() const override {
        return commandID;
    }
};

class DeleteShapeCommand : public ICommand {
private:
    SceneManager& sceneManager;
    Shape* shape;
    bool ownsShape;
    int commandID;
    static int nextCommandID;

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
