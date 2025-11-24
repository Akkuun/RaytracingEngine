#pragma once
#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../shapes/Shape.h"
#include "../../material/Material.h"

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
        }
    }
    void execute() override {
        shape->getMaterial()->set_texture(newTexture);
    }
    void undo() override {
        shape->getMaterial()->set_texture(previousTexture);
    }
    int getID() const override {
        return commandID;
    }
};
