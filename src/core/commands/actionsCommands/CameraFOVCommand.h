#pragma once
#include "../ICommand.h"
#include "../../camera/Camera.h"
#include "../CommandsManager.h"

class CameraFOVCommand : public ICommand
{
private:
    Camera &camera;
    float previousFOV;
    float newFOV;
    int commandID;
    inline static int nextCommandID = 0;

public:
    CameraFOVCommand(const Camera &cam, float fov)
        : camera(const_cast<Camera &>(cam)), commandID(nextCommandID++)
    {
        previousFOV = camera.getFOV();
        newFOV = fov;
    }
    void execute() override
    {
        camera.setFOV(newFOV);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    void undo() override
    {
        camera.setFOV(previousFOV);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    int getID() const override
    {
        return commandID;
    }
};