#pragma once

#include "../../../camera/Camera.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class CameraNbBouncesCommand : public ICommand
{
private:
    Camera &camera;
    int previousBounces;
    int newBounces;
    int commandID;
    inline static int nextCommandID = 0;

public:
    CameraNbBouncesCommand(const Camera &cam, int bounces)
        : camera(const_cast<Camera &>(cam)), commandID(nextCommandID++)
    {
        previousBounces = camera.getNbBounces();
        newBounces = bounces;
    }
    void execute() override
    {
        camera.setNbBounces(newBounces);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    void undo() override
    {
        camera.setNbBounces(previousBounces);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    int getID() const override
    {
        return commandID;
    }
};