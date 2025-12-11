#ifndef F34B7FA8_CC42_4B72_B8E4_853E2D2C7ACF
#define F34B7FA8_CC42_4B72_B8E4_853E2D2C7ACF

#include "../../../material/Material.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

class MaterialEmissiveCommand : public ICommand
{
private:
    Material &material;
    float previousEmissive;
    float newEmissive;
    int commandID;
    inline static int nextCommandID = 0;

public:
    MaterialEmissiveCommand(Material &mat, float emissive)
        : material(mat), commandID(nextCommandID++)
    {
        previousEmissive = material.getLightIntensity();
        newEmissive = emissive;
    }
    void execute() override
    {
        material.setLightIntensity(newEmissive);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    void undo() override
    {
        material.setLightIntensity(previousEmissive);
        CommandsManager::getInstance().notifyMaterialChanged();
    }
    int getID() const override
    {
        return commandID;
    }
};


#endif /* F34B7FA8_CC42_4B72_B8E4_853E2D2C7ACF */
