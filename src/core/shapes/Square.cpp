#include "Square.h"
#include "../math/transformUtils.h"

GPUSquare Square::toGPU() const
{
    GPUSquare gpuSquare;

    // rotation deg to rad
    vec3 rotationRad = TransformUtils::degToRad(rotation);

    // apply rotation and scale to U and V
    vec3 transformedU = TransformUtils::applyRotationAndScale(u_vec, rotationRad, scale);
    vec3 transformedV = TransformUtils::applyRotationAndScale(v_vec, rotationRad, scale);
    
    // Transform the normal: for normals, we need to apply only rotation 
    vec3 transformedNormal = TransformUtils::applyRotation(normal, rotationRad);
    transformedNormal.normalize();
    
    // Position
    gpuSquare.pos.x = static_cast<float>(position.x);
    gpuSquare.pos.y = static_cast<float>(position.y);
    gpuSquare.pos.z = static_cast<float>(position.z);
    gpuSquare.pos._padding = 0.0f;
    
    // U vector
    gpuSquare.u_vec.x = static_cast<float>(transformedU.x);
    gpuSquare.u_vec.y = static_cast<float>(transformedU.y);
    gpuSquare.u_vec.z = static_cast<float>(transformedU.z);
    gpuSquare.u_vec._padding = 0.0f;
    
    // V vector
    gpuSquare.v_vec.x = static_cast<float>(transformedV.x);
    gpuSquare.v_vec.y = static_cast<float>(transformedV.y);
    gpuSquare.v_vec.z = static_cast<float>(transformedV.z);
    gpuSquare.v_vec._padding = 0.0f;
    
    // Normal
    gpuSquare.normal.x = static_cast<float>(transformedNormal.x);
    gpuSquare.normal.y = static_cast<float>(transformedNormal.y);
    gpuSquare.normal.z = static_cast<float>(transformedNormal.z);
    gpuSquare.normal._padding = 0.0f;
    
    // Material index
    gpuSquare.materialIndex = (material != nullptr) ? material->getMaterialId() : -1;
    gpuSquare._padding[0] = 0.0f;
    gpuSquare._padding[1] = 0.0f;
    gpuSquare._padding[2] = 0.0f;

    return gpuSquare;
}

std::string Square::toString() const
{
    std::string baseStr = Shape::toString();
    baseStr += "U Vector: (" + std::to_string(u_vec.x) + ", " + std::to_string(u_vec.y) + ", " + std::to_string(u_vec.z) + ")\n";
    baseStr += "V Vector: (" + std::to_string(v_vec.x) + ", " + std::to_string(v_vec.y) + ", " + std::to_string(v_vec.z) + ")\n";
    baseStr += "Normal: (" + std::to_string(normal.x) + ", " + std::to_string(normal.y) + ", " + std::to_string(normal.z) + ")\n";
    return baseStr;
}