#pragma once

#include "../math/vec3.h"

// GPU-compatible Vec3 with padding to match kernel
typedef struct {
    float x, y, z;
    float _padding;  // Padding to align to 16 bytes (same as float4)
} GPUVec3;

// GPU-compatible camera structure
typedef struct {
    GPUVec3 origin;   // Camera position (16 bytes)
    GPUVec3 target;   // What the camera is looking at (16 bytes)
    GPUVec3 up;       // Up vector (16 bytes)
    float fov;        // Field of view in degrees (4 bytes)
    float _padding[3]; // Padding for alignment (12 bytes)
} GPUCamera;

class Camera
{
public:
    static Camera& getInstance() {
        static Camera instance;
        return instance;
    }
    Camera();
    ~Camera();

    // getters
    inline vec3 getOrigin() const { return origin; }
    inline vec3 getTarget() const { return target; }
    inline vec3 getUp() const { return up; }
    inline float getFOV() const { return fov; }
    inline vec3 getRotation() const {
        vec3 dir = target - origin;
        dir.normalize();
        float pitch = asinf(dir.y) * (180.0f / 3.14159265f);
        float yaw = atan2f(dir.z, dir.x) * (180.0f / 3.14159265f);
        return vec3(pitch, yaw, 0.0f);
    }
    inline vec3 getPosition() const { return origin; }

    // setters
    inline void setOrigin(const vec3 &o) { origin = o; }
    inline void setTarget(const vec3 &t) { target = t; }
    inline void setUp(const vec3 &u) { up = u; }
    inline void setFOV(float f) { fov = f; }
    inline void setPosition(const vec3 &pos) {
        vec3 dir = target - origin;
        target = pos + dir;
        origin = pos;
    }
    inline void setRotation(const vec3 &rot) {
        float pitch = rot.x * (3.14159265f / 180.0f);
        float yaw = rot.y * (3.14159265f / 180.0f);
        vec3 dir;
        dir.x = cosf(pitch) * cosf(yaw);
        dir.y = sinf(pitch);
        dir.z = cosf(pitch) * sinf(yaw);
        dir.normalize();
        target = origin + dir;
    }
    
    // Utility methods
    vec3 getForward() const { 
        vec3 forward = target - origin;
        forward.normalize();
        return forward;
    }
    vec3 getRight() const { 
        vec3 forward = getForward();
        return vec3::cross(forward, up);
    }
    
    // Convert to GPU format
    GPUCamera toGPU() const;

 private:
  vec3 origin;
  vec3 target;
  vec3 up;
  float fov;
};