#include "Sphere.h"

GPUSphere Sphere::toGPU() const
{
    GPUSphere gpuSphere;
    // radius
    gpuSphere.radius = radius;
    // center position (from Shape base class)
    gpuSphere.pos[0] = static_cast<float>(position.x);
    gpuSphere.pos[1] = static_cast<float>(position.y);
    gpuSphere.pos[2] = static_cast<float>(position.z);
    // emi and color
    gpuSphere.emi[0] = static_cast<float>(emi.x);
    gpuSphere.emi[1] = static_cast<float>(emi.y);
    gpuSphere.emi[2] = static_cast<float>(emi.z);
    gpuSphere.color[0] = static_cast<float>(color.x);
    gpuSphere.color[1] = static_cast<float>(color.y);
    gpuSphere.color[2] = static_cast<float>(color.z);

    return gpuSphere;
}