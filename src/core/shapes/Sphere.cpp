#include "Sphere.h"

GPUSphere Sphere::toGPU() const
{
    GPUSphere gpuSphere;
    gpuSphere.center[0] = static_cast<float>(center.x);
    gpuSphere.center[1] = static_cast<float>(center.y);
    gpuSphere.center[2] = static_cast<float>(center.z);
    gpuSphere.radius = static_cast<float>(radius);
    

    return gpuSphere;
}