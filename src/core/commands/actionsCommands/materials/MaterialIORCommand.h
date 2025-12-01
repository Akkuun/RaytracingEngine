#pragma once

#include "../../../material/Material.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class MaterialIORCommand : public ICommand
{
private:
    Material &material;
    float previousIOR;
    float newIOR;
    int commandID;
    inline static int nextCommandID = 0;

public:
    MaterialIORCommand(Material &mat, float ior)
        : material(mat), commandID(nextCommandID++)
    {
        previousIOR = material.getIndexMedium();
        newIOR = ior;
    }
    void execute() override
    {
        material.setIndexMedium(newIOR);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    void undo() override
    {
        material.setIndexMedium(previousIOR);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    int getID() const override
    {
        return commandID;
    }
};