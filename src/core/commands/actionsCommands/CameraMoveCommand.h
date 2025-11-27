#pragma once
#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../camera/Camera.h"
#include <glm/glm.hpp>
#include "../CommandsManager.h"

class CameraMoveCommand : public ICommand {
private:
    Camera& camera;
    glm::vec3 previousPosition;
    glm::vec3 newPosition;
    int commandID;
    inline static int nextCommandID = 0;
public:
    CameraMoveCommand(const Camera& cam, float newX, float newY, float newZ)
        : camera(const_cast<Camera&>(cam)), commandID(nextCommandID++) {
        previousPosition = camera.getPosition();
        newPosition = glm::vec3(newX, newY, newZ);
    }
    void execute() override {
        camera.setPosition(newPosition);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    void undo() override {
        camera.setPosition(previousPosition);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    int getID() const override {
        return commandID;
    }
};
