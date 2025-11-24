#pragma once
#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../material/Material.h"

class ClearTextureShape : public ICommand {
private:
    Shape* shape;
    ppmLoader::ImageRGB previousTexture;
    int commandID;
    inline static int nextCommandID = 0;
public:
    ClearTextureShape(Shape* shape)
        : shape(shape), commandID(nextCommandID++) {
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
        shape->getMaterial()->remove_texture();
    }
    void undo() override {
        shape->getMaterial()->set_texture(previousTexture);
    }
    int getID() const override {
        return commandID;
    }
};
