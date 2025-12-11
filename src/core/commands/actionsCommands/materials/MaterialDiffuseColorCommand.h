#pragma once

#include "../../../material/Material.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class MaterialDiffuseColorCommand : public ICommand
{
private:
    Material &material;
    vec3 oldColor;
    vec3 newColor;
    int commandID;
    inline static int nextCommandID = 0;

public:
    MaterialDiffuseColorCommand(Material &mat, const vec3 &color)
        : material(mat), commandID(nextCommandID++)
    {
        oldColor = material.getDiffuse();
        newColor = color;
    }

    ~MaterialDiffuseColorCommand() = default;

    void execute() override
    {
        material.setDiffuse(newColor);
        CommandsManager::getInstance().notifyMaterialChanged();
    }

    void undo() override
    {
        material.setDiffuse(oldColor);
        CommandsManager::getInstance().notifyMaterialChanged();
    }

    int getID() const override
    {
        return commandID;
    }
};
