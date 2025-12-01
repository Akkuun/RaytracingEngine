#pragma once

#include "../../../material/Material.h"
#include "../../../shapes/Shape.h"
#include "../../../systems/SceneManager/SceneManager.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class ClearMetallicShape : public ICommand {
private:
    Shape* shape;
    ppmLoader::ImageRGB previousMetallic;
    int commandID;
    inline static int nextCommandID = 0;
public:
    ClearMetallicShape(Shape* shape)
        : shape(shape), commandID(nextCommandID++) {
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
        shape->getMaterial()->removeMetallic();
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    void undo() override {
        shape->getMaterial()->setMetallic(previousMetallic);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    int getID() const override {
        return commandID;
    }
};