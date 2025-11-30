#pragma once

#include "../../../material/Material.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class MaterialTransparencyCommand : public ICommand
{
private:
    Material &material;
    float previousTransparency;
    float newTransparency;
    int commandID;
    inline static int nextCommandID = 0;

public:
    MaterialTransparencyCommand(Material &mat, float transparency)
        : material(mat), commandID(nextCommandID++)
    {
        previousTransparency = material.getTransparency();
        newTransparency = transparency;
    }
    void execute() override
    {
        material.setTransparency(newTransparency);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    void undo() override
    {
        material.setTransparency(previousTransparency);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    int getID() const override
    {
        return commandID;
    }
};