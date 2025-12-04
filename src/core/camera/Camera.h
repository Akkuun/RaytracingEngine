#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <map>
#include <memory>
#include <QObject>
#include "../defines/Defines.h"

class QKeyEvent;
class QMouseEvent;

#define PI 3.14159265358979323846

// GPU-compatible Vec3 with padding to match kernel
typedef struct
{
    float x, y, z;
    float _padding; // Padding to align to 16 bytes (same as float4)
} GPUVec3;

// GPU-compatible camera structure
typedef struct
{
    GPUVec3 origin;    // Camera position (16 bytes)
    GPUVec3 target;    // What the camera is looking at (16 bytes)
    GPUVec3 up;        // Up vector (16 bytes)
    float fov;         // Field of view in degrees (4 bytes)
	int nbBounces;    // Number of ray bounces (4 bytes)
	int raysPerPixel; //  Number of rays per pixel (4 bytes)
    int bufferType;
} GPUCamera;

// Camera constants
static const float DEFAULT_FOV = 80.0f;
static const glm::vec3 DEFAULT_POSITION = glm::vec3(0.0f, 0.0f, -1.0f);
static const glm::vec3 DEFAULT_EULER_ANGLE = glm::vec3(0.0f, 0.0f, 0.0f);
static const int DEFAULT_BOUNCES = 3;
static const int DEFAULT_RPP = 1;
static const bool DEFAULT_ATTACHED = false;
static const float DEFAULT_TRANSLATION_SPEED = 0.005f;
static const float DEFAULT_DISTANCE_SPEED = 0.1f;
static const float DEFAULT_ROTATION_SPEED = PI/7.5f;
static const float KEYS_ROTATION_SPEED_CORRECTION = 1.0f;
static const glm::vec3 CAMERA_POSITION_RELATIVE_TO_PLAYER = glm::vec3(0.0f, 2.0f, 5.0f);
static const float DELTA_Y_SNEAK = 0.3f;

static const glm::vec3 VEC_ZERO = glm::vec3(0.f, 0.f, 0.f);
static const glm::vec3 VEC_UP = glm::vec3(0.f, 1.f, 0.f);
static const glm::vec3 VEC_FRONT = glm::vec3(0.f, 0.f, 1.f);
static const glm::vec3 VEC_RIGHT = glm::vec3(1.f, 0.f, 0.f);

class Camera : public QObject
{
    Q_OBJECT

public:
    static Camera &getInstance()
    {
        static Camera instance;
        return instance;
    }
    Camera();
    ~Camera();

signals:
    void positionChanged(float x, float y, float z);
    void rotationChanged(float x, float y, float z); // In degrees
    void fovChanged(float fov);
    void nbBouncesChanged(int bounces);
    void raysPerPixelChanged(int rpp);
    void bufferTypeChanged(int type);

public:
    void reset();
    void init();
    void update(float deltaTime);
    void setTarget(const glm::vec3 &target);
    void updateTarget(const glm::vec3 &target);
    void setPlayerMotions(bool sprinting, bool sneaking);
    
    // Convert to GPU format
    GPUCamera toGPU() const;

    // Check if camera has changed (for TAA accumulation reset)
    inline bool hasMoved() const { return m_hasMoved; }
    inline void clearMovedFlag() { m_hasMoved = false; }

    // Input handling (to be called from Qt widgets)
    void handleKeyPress(int key, bool pressed);
    void handleMouseMove(float deltaX, float deltaY);
    void handleMouseScroll(float delta);
    void setControlMode(int mode); // 0 = mouse, 1 = keyboard

    inline bool isFPS() const { return !m_attached; }

    // Getters
    inline glm::vec3 getFront() const{return glm::rotate(m_rotation, VEC_FRONT);}
    inline glm::vec3 getPosition() const { return m_position; }
    inline glm::vec3 getRotationEuler() const { return m_eulerAngle; }
    inline glm::quat getRotation() const { return m_rotation; }
    // Matrix getters
    inline glm::mat4 getViewMatrix() const { return m_viewMatrix; }
    inline glm::mat4 getProjectionMatrix() const { return m_projectionMatrix; }
    // Camera parameters
    inline float getFOV() const { return m_fovDegree; }
    inline float getNearPlane() const { return m_nearPlane; }
    inline float getFarPlane() const { return m_farPlane; }
    inline int getNbBounces() const { return m_nb_bounces; }
    inline int getRaysPerPixel() const { return m_rays_per_pixel; }

    // Setters
    void setBufferType(int type);
    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::vec3 &eulerAngles);
    void setFOV(float fov);
    void setNbBounces(int bounces);
    void setRaysPerPixel(int rpp);

    bool m_attached = DEFAULT_ATTACHED;


private:
    // Camera parameters
    float m_fovDegree{DEFAULT_FOV};
    float m_nearPlane{0.1f};
    float m_farPlane{10000.f};
    int m_nb_bounces = DEFAULT_BOUNCES;
    int m_rays_per_pixel = DEFAULT_RPP;
    glm::vec3 m_position{DEFAULT_POSITION};
    glm::vec3 m_eulerAngle{DEFAULT_EULER_ANGLE};
    glm::quat m_rotation{};
    float m_translation_speed = DEFAULT_TRANSLATION_SPEED;
    float m_distance_speed = DEFAULT_DISTANCE_SPEED;
    float m_rotation_speed = DEFAULT_ROTATION_SPEED;
    float m_rotationSpeedKeysCorrection = KEYS_ROTATION_SPEED_CORRECTION;

    bool m_invertY = false;
    bool m_invertX = false;

    // Camera mode
    int m_mode = 0; // 0 = mouse control, 1 = keyboard control
    double m_prevMouseX = 0.0, m_prevMouseY = 0.0;

    // Reset animation
    bool m_resetting = false;
    float m_resetTime = 0.f;
    float m_resetDuration = 1.f;
    float m_initialFov;
    glm::vec3 m_initialPosition;
    glm::vec3 m_initialEulerAngle;
    float m_targetFov = DEFAULT_FOV;
    glm::vec3 m_targetPosition = DEFAULT_POSITION;
    glm::vec3 m_targetEulerAngle = DEFAULT_EULER_ANGLE;

    // Camera Third person
    glm::vec3 m_relativePos = CAMERA_POSITION_RELATIVE_TO_PLAYER;
    glm::vec3 m_targetDeltaPos;
    glm::vec3 m_targetPrev;
    float m_distance = 5.0f;

    // Sprint FOV animation
    bool m_sprinting = false;
    float m_deltaFov = 0.0f;
    float m_maxDeltaFOV = 10.0f;
    float m_runningFOVtime = 0.0f;
    float m_runningFOVduration = 0.25f;

    // Sneak animation
    bool m_sneaking = false;
    float m_sneakDeltaY = 0.0f;
    float m_sneaktime = 0.0f;
    float m_sneakDuration = 0.25f;

    // View matrices
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    inline glm::vec3 getTarget() const { return m_targetPrev; }

    // Simple key state tracking for camera control
    std::map<int, bool> m_keysPressed;

    // Movement tracking for TAA reset
    bool m_hasMoved = false;

    int m_bufferType = BufferType::IMAGE;


    void loadCameraSettings();
};