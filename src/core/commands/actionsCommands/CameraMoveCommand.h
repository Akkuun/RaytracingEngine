#pragma once
#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../camera/Camera.h"

class CameraMoveCommand : public ICommand {
private:
    Camera& camera;
    vec3 previousPosition;
    vec3 newPosition;
    int commandID;
    inline static int nextCommandID = 0;
public:
    CameraMoveCommand(Camera& cam, float newX, float newY, float newZ)
        : camera(cam), commandID(nextCommandID++) {
        previousPosition = camera.getPosition();
        newPosition = vec3(newX, newY, newZ);
    }
    void execute() override {
        camera.setPosition(newPosition);
    }
    void undo() override {
        camera.setPosition(previousPosition);
    }
    int getID() const override {
        return commandID;
    }
};
