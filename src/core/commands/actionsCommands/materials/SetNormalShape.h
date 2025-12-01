#pragma once

#include "../../../material/Material.h"
#include "../../../shapes/Shape.h"
#include "../../../systems/SceneManager/SceneManager.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class SetNormalShape : public ICommand {
private:
    Shape* shape;
    ppmLoader::ImageRGB previousNormal;
    ppmLoader::ImageRGB newNormal;
    int commandID;
    inline static int nextCommandID = 0;
public:
    SetNormalShape(Shape* shape, ppmLoader::ImageRGB newNorm)
        : shape(shape), newNormal(newNorm), commandID(nextCommandID++) {
        Material* mat = shape->getMaterial();
        if (mat) {
            try {
                previousNormal = mat->getNormals();
            } catch (...) {
                // If getting previous normal fails, initialize to empty
                previousNormal = ppmLoader::ImageRGB{0, 0, {}};
            }
        } else { // Create a new material if none exists
            shape->setMaterial(new Material());
            try {
                previousNormal = shape->getMaterial()->getNormals();
            } catch (...) {
                // If getting previous normal fails, initialize to empty
                previousNormal = ppmLoader::ImageRGB{0, 0, {}};
            }
        }
    }
    void execute() override {
        shape->getMaterial()->setNormals(newNormal);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    void undo() override {
        shape->getMaterial()->setNormals(previousNormal);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    int getID() const override {
        return commandID;
    }
};