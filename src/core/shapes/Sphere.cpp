#include "Sphere.h"

GPUSphere Sphere::toGPU() const
{
    GPUSphere gpuSphere;
    // radius (first float in the struct)
    gpuSphere.radius = radius;
    // padding is automatically zero-initialized
    gpuSphere._padding1[0] = 0.0f;
    gpuSphere._padding1[1] = 0.0f;
    gpuSphere._padding1[2] = 0.0f;
    
    // center position (from Shape base class)
    gpuSphere.pos.x = static_cast<float>(position.x);
    gpuSphere.pos.y = static_cast<float>(position.y);
    gpuSphere.pos.z = static_cast<float>(position.z);
    gpuSphere.pos._padding = 0.0f;
    
    // emission
    gpuSphere.emi.x = static_cast<float>(emission.x);
    gpuSphere.emi.y = static_cast<float>(emission.y);
    gpuSphere.emi.z = static_cast<float>(emission.z);
    gpuSphere.emi._padding = 0.0f;
    
    // color
    gpuSphere.color.x = static_cast<float>(color.x);
    gpuSphere.color.y = static_cast<float>(color.y);
    gpuSphere.color.z = static_cast<float>(color.z);
    gpuSphere.color._padding = 0.0f;

    return gpuSphere;
}

std::string Sphere::toString() const
{
    std::string baseStr = Shape::toString();
    baseStr += "Radius: " + std::to_string(radius) + "\n";
    baseStr += "Color: (" + std::to_string(color.x) + ", " + std::to_string(color.y) + ", " + std::to_string(color.z) + ")\n";
    return baseStr;
}