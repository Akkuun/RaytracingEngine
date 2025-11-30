#ifndef C72C8348_C156_4C4A_81A9_F9C62C34766A
#define C72C8348_C156_4C4A_81A9_F9C62C34766A

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


#endif /* C72C8348_C156_4C4A_81A9_F9C62C34766A */
