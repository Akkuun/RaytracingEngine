#pragma once

#include <glm/glm.hpp>
#include <string>
#include "../../../camera/Camera.h"
#include "../../CommandsManager.h"
#include "../../ICommand.h"

/**
 * @brief Command to reset camera to default or save/restore camera states
 *
 * This command saves the current camera state and can reset it,
 * making it undoable.
 */
class CameraResetCommand : public ICommand
{
private:
    glm::vec3 previousPosition;
    glm::vec3 previousRotation;
    float previousFOV;

    glm::vec3 targetPosition;
    glm::vec3 targetRotation;
    float targetFOV;

    int commandID;
    inline static int nextCommandID = 0;

public:
    /**
     * @brief Reset camera to default values (uses singleton)
     */
    CameraResetCommand()
        : commandID(nextCommandID++)
    {
        Camera &camera = Camera::getInstance();

        // Save current state
        previousPosition = camera.getPosition();
        previousRotation = camera.getRotationEuler();
        previousFOV = camera.getFOV();

        // Set target to defaults
        targetPosition = DEFAULT_POSITION;
        targetRotation = DEFAULT_EULER_ANGLE;
        targetFOV = DEFAULT_FOV;
    }

    /**
     * @brief Reset specific camera to default values (for UI panels)
     */
    CameraResetCommand(const Camera &cam)
        : commandID(nextCommandID++)
    {
        // Save current state
        previousPosition = cam.getPosition();
        previousRotation = cam.getRotationEuler();
        previousFOV = cam.getFOV();

        // Set target to defaults
        targetPosition = DEFAULT_POSITION;
        targetRotation = DEFAULT_EULER_ANGLE;
        targetFOV = DEFAULT_FOV;
    }

    /**
     * @brief Set camera to specific state (uses singleton)
     */
    CameraResetCommand(const glm::vec3 &pos, const glm::vec3 &rot, float fov)
        : commandID(nextCommandID++)
    {
        Camera &camera = Camera::getInstance();

        // Save current state
        previousPosition = camera.getPosition();
        previousRotation = camera.getRotationEuler();
        previousFOV = camera.getFOV();

        // Set target state
        targetPosition = pos;
        targetRotation = rot;
        targetFOV = fov;
    }

    /**
     * @brief Set specific camera to specific state (for UI panels)
     */
    CameraResetCommand(const Camera &cam, const glm::vec3 &pos, const glm::vec3 &rot, float fov)
        : commandID(nextCommandID++)
    {
        // Save current state
        previousPosition = cam.getPosition();
        previousRotation = cam.getRotationEuler();
        previousFOV = cam.getFOV();

        // Set target state
        targetPosition = pos;
        targetRotation = rot;
        targetFOV = fov;
    }

    void execute() override
    {
        Camera &camera = Camera::getInstance();
        camera.setPosition(targetPosition);
        camera.setRotation(targetRotation);
        camera.setFOV(targetFOV);
        CommandsManager::getInstance().notifyCameraChanged();
    }

    void undo() override
    {
        Camera &camera = Camera::getInstance();
        camera.setPosition(previousPosition);
        camera.setRotation(previousRotation);
        camera.setFOV(previousFOV);
        CommandsManager::getInstance().notifyCameraChanged();
    }

    int getID() const override
    {
        return commandID;
    }
};

