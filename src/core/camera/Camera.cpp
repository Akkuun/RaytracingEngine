#include "Camera.h"
#include "Camera_Helper.h"
#include "../input/Keybinds.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <Qt>
#include <cmath>
#include <iostream>

Camera::Camera()
{
    init();
}

Camera::~Camera()
{
}

void Camera::reset()
{
    if (m_resetting || m_attached) return;
    m_initialFov = m_fovDegree;
    m_initialPosition = m_position;
    m_initialEulerAngle = m_eulerAngle;

    m_rotation_speed = DEFAULT_ROTATION_SPEED;
    m_translation_speed = DEFAULT_TRANSLATION_SPEED;
    m_distance_speed = DEFAULT_DISTANCE_SPEED;
    m_attached = DEFAULT_ATTACHED;
    m_resetting = true;
    m_resetTime = 0.f;
    
    // Emit signals for the reset target values
    emit positionChanged(m_targetPosition.x, m_targetPosition.y, m_targetPosition.z);
    emit rotationChanged(glm::degrees(m_targetEulerAngle.x), glm::degrees(m_targetEulerAngle.y), glm::degrees(m_targetEulerAngle.z));
    emit fovChanged(m_targetFov);
}

void Camera::init()
{
    m_position = DEFAULT_POSITION;
    m_eulerAngle = DEFAULT_EULER_ANGLE;
    m_rotation = glm::quat(m_eulerAngle);
    m_fovDegree = DEFAULT_FOV;
    m_hasMoved = false;
}

void Camera::handleKeyPress(int key, bool pressed)
{
    m_keysPressed[key] = pressed;
}

void Camera::handleMouseMove(float deltaX, float deltaY)
{
    if (m_resetting || m_mode != 0) return; // Only handle mouse in mode 0

    int invertY = m_invertY ? -1 : 1;
    int invertX = m_invertX ? -1 : 1;

    bool rotationChanged = false;
    if (deltaX != 0) {
        m_eulerAngle.y = Camera_Helper::clipAnglePI(
            m_eulerAngle.y - deltaX * M_PI / 180.0 * m_rotation_speed * invertX
        );
        m_hasMoved = true;
        rotationChanged = true;
    }
    if (deltaY != 0) {
        m_eulerAngle.x = Camera_Helper::clamp(
            Camera_Helper::clipAnglePI(m_eulerAngle.x - deltaY * M_PI / 180.0 * m_rotation_speed * invertY),
            -M_PI_2 + 0.1f, M_PI_2 - 0.1f
        );
        m_hasMoved = true;
        rotationChanged = true;
    }
    
    if (rotationChanged) {
        emit this->rotationChanged(glm::degrees(m_eulerAngle.x), glm::degrees(m_eulerAngle.y), glm::degrees(m_eulerAngle.z));
    }
}

void Camera::handleMouseScroll(float delta)
{
    if (m_attached) {
        m_distance = Camera_Helper::clamp(m_distance - delta * m_distance_speed, 1.0f, 50.0f);
        m_hasMoved = true;
    }
}

void Camera::setControlMode(int mode)
{
    m_mode = mode;
}

void Camera::update(float deltaTime)
{
    // Keybinds reference
    Keybinds& keybinds = Keybinds::getInstance();
    
    // Helper lambda to check if a keybind is currently pressed
    // Note: For single-key binds only. Multi-key combinations should use QShortcut
    auto isKeyPressed = [this](const QKeySequence& seq) -> bool {
        if (seq.isEmpty()) return false;
        int key = seq[0] & ~Qt::KeyboardModifierMask; // Extract key without modifiers
        return m_keysPressed[key];
    };
    
    if (m_resetting) {
        double ratio = Camera_Helper::interpolation(
            Camera_Helper::clamp(m_resetTime / m_resetDuration, 0., 1.), 
            InterPolationType::SQRT
        );
        m_fovDegree = ratio * m_targetFov + (1. - ratio) * m_initialFov;
        m_position = glm::vec3(ratio) * m_targetPosition + glm::vec3(1. - ratio) * m_initialPosition;
        m_eulerAngle = glm::vec3(ratio) * m_targetEulerAngle + glm::vec3(1. - ratio) * m_initialEulerAngle;
        m_resetTime += deltaTime;
        if (m_resetTime > m_resetDuration) {
            m_resetting = false;
            // Fix approximation errors
            m_fovDegree = m_targetFov;
            m_position = m_targetPosition;
            m_eulerAngle = m_targetEulerAngle;
        }
    }

    // Handle keyboard rotation (arrow keys) - works in all modes
    bool keyboardRotationChanged = false;
    if (!m_resetting) {
        if (isKeyPressed(keybinds.getKeybind(KB_CAMERA_LEFT))) {
            m_eulerAngle.y = Camera_Helper::clipAnglePI(
                m_eulerAngle.y + m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection
            );
            m_hasMoved = true;
            keyboardRotationChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_CAMERA_RIGHT))) {
            m_eulerAngle.y = Camera_Helper::clipAnglePI(
                m_eulerAngle.y - m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection
            );
            m_hasMoved = true;
            keyboardRotationChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_CAMERA_UP))) {
            m_eulerAngle.x = Camera_Helper::clamp(
                Camera_Helper::clipAnglePI(m_eulerAngle.x - m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection),
                -M_PI_2 + 0.1f, M_PI_2 - 0.1f
            );
            m_hasMoved = true;
            keyboardRotationChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_CAMERA_DOWN))) {
            m_eulerAngle.x = Camera_Helper::clamp(
                Camera_Helper::clipAnglePI(m_eulerAngle.x + m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection),
                -M_PI_2 + 0.1f, M_PI_2 - 0.1f
            );
            m_hasMoved = true;
            keyboardRotationChanged = true;
        }
    }
    
    if (keyboardRotationChanged) {
        emit rotationChanged(glm::degrees(m_eulerAngle.x), glm::degrees(m_eulerAngle.y), glm::degrees(m_eulerAngle.z));
    }
    
    // Handle WASD movement (free camera mode)
    bool keyboardPositionChanged = false;
    if (!m_resetting && !m_attached) {
        glm::vec3 front = glm::rotate(m_rotation, VEC_FRONT);
        glm::vec3 right = glm::rotate(m_rotation, VEC_RIGHT);
        
        if (isKeyPressed(keybinds.getKeybind(KB_MOVE_FORWARD))) {
            m_position += front * m_translation_speed;
            m_hasMoved = true;
            keyboardPositionChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_MOVE_BACKWARD))) {
            m_position -= front * m_translation_speed;
            m_hasMoved = true;
            keyboardPositionChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_MOVE_LEFT))) {
            m_position -= right * m_translation_speed;
            m_hasMoved = true;
            keyboardPositionChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_MOVE_RIGHT))) {
            m_position += right * m_translation_speed;
            m_hasMoved = true;
            keyboardPositionChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_MOVE_UP))) {
            m_position += VEC_UP * m_translation_speed;
            m_hasMoved = true;
            keyboardPositionChanged = true;
        }
        if (isKeyPressed(keybinds.getKeybind(KB_MOVE_DOWN))) {
            m_position -= VEC_UP * m_translation_speed;
            m_hasMoved = true;
            keyboardPositionChanged = true;
        }
    }
    
    if (keyboardPositionChanged) {
        emit positionChanged(m_position.x, m_position.y, m_position.z);
    }

    // Animation du FOV en fonction de la course de la target
    InterPolationType fovInterpolationType;
    if (m_sprinting) {
        m_runningFOVtime += deltaTime;
        if (m_runningFOVtime > m_runningFOVduration) {
            m_runningFOVtime = m_runningFOVduration;
        }
        fovInterpolationType = InterPolationType::SQRT;    
    } else {
        m_runningFOVtime -= deltaTime;
        if (m_runningFOVtime < 0) {
            m_runningFOVtime = 0;
        }
        fovInterpolationType = InterPolationType::CUBIC;
    }
    m_deltaFov = m_maxDeltaFOV * Camera_Helper::interpolation(m_runningFOVtime / m_runningFOVduration, fovInterpolationType);

    // Animation du sneak
    InterPolationType sneakInterpolationType;
    if (m_sneaking) {
        m_sneaktime += deltaTime;
        if (m_sneaktime > m_sneakDuration) {
            m_sneaktime = m_sneakDuration;
        }
        sneakInterpolationType = InterPolationType::SQRT;
    } else {
        m_sneaktime -= deltaTime;
        if (m_sneaktime < 0) {
            m_sneaktime = 0;
        }
        sneakInterpolationType = InterPolationType::CUBIC;
    }
    m_sneakDeltaY = DELTA_Y_SNEAK * Camera_Helper::interpolation(m_sneaktime / m_sneakDuration, sneakInterpolationType);

    if (m_attached) {
        // Rotation autour de la target
        m_rotation = glm::quat(m_eulerAngle);
        m_position = m_targetPrev - m_rotation * VEC_FRONT * m_distance;
    } else {
        m_rotation = glm::quat(m_eulerAngle);
    }

    m_position.y -= m_sneakDeltaY;
    
    Camera_Helper::computeFinalView(m_projectionMatrix, m_viewMatrix, m_position, m_rotation, 
                                     m_fovDegree + m_deltaFov, m_nearPlane, m_farPlane);
}

void Camera::setTarget(glm::vec3 target)
{
    m_targetPrev = target;
}

void Camera::updateTarget(glm::vec3 target)
{
    m_targetDeltaPos = target - m_targetPrev;
    m_targetPrev = target;
}

void Camera::setPlayerMotions(bool sprinting, bool sneaking)
{
    m_sprinting = sprinting;
    m_sneaking = sneaking;
}

void Camera::setRotation(const glm::vec3& eulerAngles)
{
    m_eulerAngle = eulerAngles;
    m_rotation = glm::quat(m_eulerAngle);
    emit rotationChanged(glm::degrees(m_eulerAngle.x), glm::degrees(m_eulerAngle.y), glm::degrees(m_eulerAngle.z));
}

GPUCamera Camera::toGPU() const {
    GPUCamera gpu_camera;
    
    // Calculate camera target point from position and rotation
    glm::vec3 front = getFront();
    glm::vec3 target = m_position + front;
    glm::vec3 up = glm::rotate(m_rotation, VEC_UP);
    
    // Convert glm::vec3 to GPUVec3 with proper padding
    gpu_camera.origin.x = m_position.x;
    gpu_camera.origin.y = m_position.y;
    gpu_camera.origin.z = m_position.z;
    gpu_camera.origin._padding = 0.0f; // MANDATORY
    
    gpu_camera.target.x = target.x;
    gpu_camera.target.y = target.y;
    gpu_camera.target.z = target.z;
    gpu_camera.target._padding = 0.0f; // MANDATORY
    
    gpu_camera.up.x = up.x;
    gpu_camera.up.y = up.y;
    gpu_camera.up.z = up.z;
    gpu_camera.up._padding = 0.0f; // MANDATORY 
    
    gpu_camera.fov = m_fovDegree;
    gpu_camera._padding[0] = 0.0f;
    gpu_camera._padding[1] = 0.0f;
    gpu_camera._padding[2] = 0.0f;
    
    return gpu_camera;
}
