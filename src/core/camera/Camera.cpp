#include "Camera.h"
#include "../math/vec3.h"

Camera::Camera()
    : origin(vec3(0.0f, 0.0f, 5.f)), 
      target(vec3(0.0f, 0.0f, 0)),  //  center of the scene
      up(vec3(0.0f, 1.0f, 0.0f)),
      fov(45.0f)
{
}

Camera::~Camera()
{
}

GPUCamera Camera::toGPU() const {
    GPUCamera gpu_camera;
    
    // Convert vec3 to GPUVec3 with proper padding
    gpu_camera.origin.x = origin.x;
    gpu_camera.origin.y = origin.y;
    gpu_camera.origin.z = origin.z;
    gpu_camera.origin._padding = 0.0f; // MANDATORY
    
    gpu_camera.target.x = target.x;
    gpu_camera.target.y = target.y;
    gpu_camera.target.z = target.z;
    gpu_camera.target._padding = 0.0f;// MANDATORY
    
    gpu_camera.up.x = up.x;
    gpu_camera.up.y = up.y;
    gpu_camera.up.z = up.z;
    gpu_camera.up._padding = 0.0f;// MANDATORY 
    
    gpu_camera.fov = fov;
    gpu_camera._padding[0] = 0.0f;
    gpu_camera._padding[1] = 0.0f;
    gpu_camera._padding[2] = 0.0f;
    
    return gpu_camera;
}
