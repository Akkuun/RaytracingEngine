#include "Triangle.h"

GPUTriangle Triangle::toGPU() const
{
    GPUTriangle gpuTri;
    // v0
    gpuTri.v0.x = static_cast<float>(v0.x);
    gpuTri.v0.y = static_cast<float>(v0.y);
    gpuTri.v0.z = static_cast<float>(v0.z);
    gpuTri.v0._padding = 0.0f;

    // v1
    gpuTri.v1.x = static_cast<float>(v1.x);
    gpuTri.v1.y = static_cast<float>(v1.y);
    gpuTri.v1.z = static_cast<float>(v1.z);
    gpuTri.v1._padding = 0.0f;

    // v2
    gpuTri.v2.x = static_cast<float>(v2.x);
    gpuTri.v2.y = static_cast<float>(v2.y);
    gpuTri.v2.z = static_cast<float>(v2.z);
    gpuTri.v2._padding = 0.0f;

    // emission
    gpuTri.emi.x = static_cast<float>(emission.x);
    gpuTri.emi.y = static_cast<float>(emission.y);
    gpuTri.emi.z = static_cast<float>(emission.z);
    gpuTri.emi._padding = 0.0f;

    // color
    gpuTri.color.x = static_cast<float>(color.x);
    gpuTri.color.y = static_cast<float>(color.y);
    gpuTri.color.z = static_cast<float>(color.z);
    gpuTri.color._padding = 0.0f;

    return gpuTri;
}

std::string Triangle::toString() const
{
    std::string baseStr = Shape::toString();
    baseStr += "V0: (" + std::to_string(v0.x) + ", " + std::to_string(v0.y) + ", " + std::to_string(v0.z) + ")\n";
    baseStr += "V1: (" + std::to_string(v1.x) + ", " + std::to_string(v1.y) + ", " + std::to_string(v1.z) + ")\n";
    baseStr += "V2: (" + std::to_string(v2.x) + ", " + std::to_string(v2.y) + ", " + std::to_string(v2.z) + ")\n";
    baseStr += "Color: (" + std::to_string(color.x) + ", " + std::to_string(color.y) + ", " + std::to_string(color.z) + ")\n";
    return baseStr;
}
