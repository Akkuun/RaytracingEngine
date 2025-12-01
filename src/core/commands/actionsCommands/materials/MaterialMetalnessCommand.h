#pragma once

#include "../../../material/Material.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class MaterialMetalnessCommand : public ICommand
{
private:
    Material &material;
    float previousMetalness;
    float newMetalness;
    int commandID;
    inline static int nextCommandID = 0;

public:
    MaterialMetalnessCommand(Material &mat, float metalness)
        : material(mat), commandID(nextCommandID++)
    {
        previousMetalness = material.getMetalness();
        newMetalness = metalness;
    }
    void execute() override
    {
        material.setMetalness(newMetalness);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    void undo() override
    {
        material.setMetalness(previousMetalness);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    int getID() const override
    {
        return commandID;
    }
};