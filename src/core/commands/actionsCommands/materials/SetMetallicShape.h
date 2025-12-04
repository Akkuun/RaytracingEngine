#pragma once

#include "../../../material/Material.h"
#include "../../../shapes/Shape.h"
#include "../../../systems/SceneManager/SceneManager.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class SetMetallicShape : public ICommand {
private:
    Shape* shape;
    ppmLoader::ImageRGB previousMetallic;
    ppmLoader::ImageRGB newMetallic;
    int commandID;
    inline static int nextCommandID = 0;
public:
    SetMetallicShape(Shape* shape, ppmLoader::ImageRGB newMetal)
        : shape(shape), newMetallic(newMetal), commandID(nextCommandID++) {
        Material* mat = shape->getMaterial();
        if (mat) {
            try {
                previousMetallic = mat->getMetallic();
            } catch (...) {
                // If getting previous metallic fails, initialize to empty
                previousMetallic = ppmLoader::ImageRGB{0, 0, {}};
            }
        } else { // Create a new material if none exists
            shape->setMaterial(new Material());
            try {
                previousMetallic = shape->getMaterial()->getMetallic();
            } catch (...) {
                // If getting previous metallic fails, initialize to empty
                previousMetallic = ppmLoader::ImageRGB{0, 0, {}};
            }
        }
    }
    void execute() override {
        shape->getMaterial()->setMetallic(newMetallic);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    void undo() override {
        shape->getMaterial()->setMetallic(previousMetallic);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    int getID() const override {
        return commandID;
    }
};