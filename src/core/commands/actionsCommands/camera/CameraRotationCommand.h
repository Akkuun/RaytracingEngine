#pragma once

#include <glm/glm.hpp>
#include "../../../systems/SceneManager/SceneManager.h"
#include "../../../camera/Camera.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class CameraRotationCommand : public ICommand {
private:
    Camera& camera;
    glm::vec3 previousRotation;
    glm::vec3 newRotation;
    int commandID;
    inline static int nextCommandID = 0;
public:
    CameraRotationCommand(const Camera& cam, float newX, float newY, float newZ)
        : camera(const_cast<Camera&>(cam)), commandID(nextCommandID++) {
        previousRotation = camera.getRotationEuler();
        newRotation = glm::vec3(newX, newY, newZ);
    }
    void execute() override {
        camera.setRotation(newRotation);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    void undo() override {
        camera.setRotation(previousRotation);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    int getID() const override {
        return commandID;
    }
};
