#pragma once

#include "../../../material/Material.h"
#include "../../../shapes/Shape.h"
#include "../../../systems/SceneManager/SceneManager.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class SetTextureShape : public ICommand {
private:
    Shape* shape;
    ppmLoader::ImageRGB previousTexture;
    ppmLoader::ImageRGB newTexture;
    int commandID;
    inline static int nextCommandID = 0;
public:
    SetTextureShape(Shape* shape, ppmLoader::ImageRGB newTex)
        : shape(shape), newTexture(newTex), commandID(nextCommandID++) {
        Material* mat = shape->getMaterial();
        if (mat) {
            try {
                previousTexture = mat->getImage();
            } catch (...) {
                // If getting previous texture fails, initialize to empty
                previousTexture = ppmLoader::ImageRGB{0, 0, {}};
            }
        } else { // Create a new material if none exists
            shape->setMaterial(new Material());
            try {
                previousTexture = shape->getMaterial()->getImage();
            } catch (...) {
                // If getting previous texture fails, initialize to empty
                previousTexture = ppmLoader::ImageRGB{0, 0, {}};
            }
        }
    }
    void execute() override {
        shape->getMaterial()->set_texture(newTexture);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    void undo() override {
        shape->getMaterial()->set_texture(previousTexture);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    int getID() const override {
        return commandID;
    }
};
