#pragma once
#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
class DeleteShapeCommand : public ICommand {
    private:
        SceneManager& sceneManager;
        Shape* shape;
        bool ownsShape;
    
    public:
        DeleteShapeCommand(Shape* shape)
            : sceneManager(SceneManager::getInstance())
            , shape(shape)
            , ownsShape(false) {}
        
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
            return shape->getID();
        }
    };