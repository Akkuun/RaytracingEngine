#pragma once
#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
class AddShapeCommand : public ICommand {
    private:
        SceneManager& sceneManager;
        Shape* targetShape;
        bool ownsShape;
    
    public:
        AddShapeCommand(Shape* shape) 
            : sceneManager(SceneManager::getInstance())
            , targetShape(shape)
            , ownsShape(true) {}
        
        ~AddShapeCommand() {
            if (ownsShape && targetShape) {
                delete targetShape;
            }
        }
    
        void execute() override {
            sceneManager.addShape(targetShape);
            ownsShape = false;
        }
    
        void undo() override {
            sceneManager.deleteShape(targetShape);
            ownsShape = true;
        }

        int getID() const override {
            return targetShape->getID();
        }
    };