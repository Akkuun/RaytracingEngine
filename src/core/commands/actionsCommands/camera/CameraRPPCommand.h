#pragma once
    
#include "../../../camera/Camera.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class CameraRPPCommand : public ICommand
{
private:
    Camera &camera;
    int previousRPP;
    int newRPP;
    int commandID;
    inline static int nextCommandID = 0;

public:
    CameraRPPCommand(const Camera &cam, int rpp)
        : camera(const_cast<Camera &>(cam)), commandID(nextCommandID++)
    {
        previousRPP = camera.getRaysPerPixel();
        newRPP = rpp;
    }
    void execute() override
    {
        camera.setRaysPerPixel(newRPP);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    void undo() override
    {
        camera.setRaysPerPixel(previousRPP);
        CommandsManager::getInstance().notifyCameraChanged();
    }
    int getID() const override
    {
        return commandID;
    }
};
