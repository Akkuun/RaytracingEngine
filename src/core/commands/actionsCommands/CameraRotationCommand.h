#pragma once
#include "../ICommand.h"
#include "../../systems/SceneManager/SceneManager.h"
#include "../../camera/Camera.h"

class CameraRotationCommand : public ICommand {
private:
    Camera& camera;
    vec3 previousRotation;
    vec3 newRotation;
    int commandID;
    inline static int nextCommandID = 0;
public:
    CameraRotationCommand(Camera& cam, float newX, float newY, float newZ)
        : camera(cam), commandID(nextCommandID++) {
        previousRotation = camera.getRotation();
        newRotation = vec3(newX, newY, newZ);
    }
    void execute() override {
        camera.setRotation(newRotation);
    }
    void undo() override {
        camera.setRotation(previousRotation);
    }
    int getID() const override {
        return commandID;
    }
};
