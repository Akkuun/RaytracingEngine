#include "Triangle.h"
#include "../math/transformUtils.h"

GPUTriangle Triangle::toGPU() const
{
    GPUTriangle gpuTri;

    vec3 center = (v0 + v1 + v2) / 3.0;
    
    vec3 relV0 = v0 - center;
    vec3 relV1 = v1 - center;
    vec3 relV2 = v2 - center;
    
    // rotation deg to rad
    vec3 rotationRad = TransformUtils::degToRad(rotation);
    
    // Apply rotation and scale to relative vertices
    vec3 transformedV0 = TransformUtils::applyRotationAndScale(relV0, rotationRad, scale);
    vec3 transformedV1 = TransformUtils::applyRotationAndScale(relV1, rotationRad, scale);
    vec3 transformedV2 = TransformUtils::applyRotationAndScale(relV2, rotationRad, scale);
    
    //  add transformed position to relative to get world position
    transformedV0 = transformedV0 + position;
    transformedV1 = transformedV1 + position;
    transformedV2 = transformedV2 + position;

    // v0
    gpuTri.v0.x = static_cast<float>(transformedV0.x);
    gpuTri.v0.y = static_cast<float>(transformedV0.y);
    gpuTri.v0.z = static_cast<float>(transformedV0.z);
    gpuTri.v0._padding = 0.0f;

    // v1
    gpuTri.v1.x = static_cast<float>(transformedV1.x);
    gpuTri.v1.y = static_cast<float>(transformedV1.y);
    gpuTri.v1.z = static_cast<float>(transformedV1.z);
    gpuTri.v1._padding = 0.0f;

    // v2
    gpuTri.v2.x = static_cast<float>(transformedV2.x);
    gpuTri.v2.y = static_cast<float>(transformedV2.y);
    gpuTri.v2.z = static_cast<float>(transformedV2.z);
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
