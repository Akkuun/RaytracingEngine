#pragma once

#include "../../../material/Material.h"
#include "../../../shapes/Shape.h"
#include "../../../systems/SceneManager/SceneManager.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class SetEmissiveShape : public ICommand {
private:
    Shape* shape;
    ppmLoader::ImageRGB previousEmissive;
    ppmLoader::ImageRGB newEmissive;
    int commandID;
    inline static int nextCommandID = 0;
    std::string pathFileEmissive;
public:
    SetEmissiveShape(Shape* shape, ppmLoader::ImageRGB newEmissive, const std::string& pathFileEmissive)
        : shape(shape), newEmissive(newEmissive), commandID(nextCommandID++), pathFileEmissive(pathFileEmissive) {
        Material* mat = shape->getMaterial();
        if (mat) {
            try {
                previousEmissive = mat->getEmissive();
            } catch (...) {
                // If getting previous emissive fails, initialize to empty
                previousEmissive = ppmLoader::ImageRGB{0, 0, {}};
            }
        } else { // Create a new material if none exists
            shape->setMaterial(new Material());
            try {
                previousEmissive = shape->getMaterial()->getEmissive();
            } catch (...) {
                // If getting previous emissive fails, initialize to empty
                previousEmissive = ppmLoader::ImageRGB{0, 0, {}};
            }
        }
    }
    void execute() override {
        shape->getMaterial()->setEmissive(newEmissive);
        shape->getMaterial()->setHasEmissiveMap(true);
        std::cout << pathFileEmissive << std::endl;
        shape->getMaterial()->setPathFileEmissiveMap(pathFileEmissive);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    void undo() override {
        shape->getMaterial()->setEmissive(previousEmissive);
        CommandsManager::getInstance().notifySceneChanged(); // notifyMaterialChanged didn't work to update texture for meshes
    }
    int getID() const override {
        return commandID;
    }
};