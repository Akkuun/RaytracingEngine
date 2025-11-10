#include "Square.h"

GPUSquare Square::toGPU() const
{
    GPUSquare gpuSquare;
    
    // Position
    gpuSquare.pos.x = static_cast<float>(position.x);
    gpuSquare.pos.y = static_cast<float>(position.y);
    gpuSquare.pos.z = static_cast<float>(position.z);
    gpuSquare.pos._padding = 0.0f;
    
    // U vector
    gpuSquare.u_vec.x = static_cast<float>(u_vec.x);
    gpuSquare.u_vec.y = static_cast<float>(u_vec.y);
    gpuSquare.u_vec.z = static_cast<float>(u_vec.z);
    gpuSquare.u_vec._padding = 0.0f;
    
    // V vector
    gpuSquare.v_vec.x = static_cast<float>(v_vec.x);
    gpuSquare.v_vec.y = static_cast<float>(v_vec.y);
    gpuSquare.v_vec.z = static_cast<float>(v_vec.z);
    gpuSquare.v_vec._padding = 0.0f;
    
    // Normal
    gpuSquare.normal.x = static_cast<float>(normal.x);
    gpuSquare.normal.y = static_cast<float>(normal.y);
    gpuSquare.normal.z = static_cast<float>(normal.z);
    gpuSquare.normal._padding = 0.0f;
    
    // Color
    gpuSquare.color.x = static_cast<float>(color.x);
    gpuSquare.color.y = static_cast<float>(color.y);
    gpuSquare.color.z = static_cast<float>(color.z);
    gpuSquare.color._padding = 0.0f;

    return gpuSquare;
}

std::string Square::toString() const
{
    std::string baseStr = Shape::toString();
    baseStr += "U Vector: (" + std::to_string(u_vec.x) + ", " + std::to_string(u_vec.y) + ", " + std::to_string(u_vec.z) + ")\n";
    baseStr += "V Vector: (" + std::to_string(v_vec.x) + ", " + std::to_string(v_vec.y) + ", " + std::to_string(v_vec.z) + ")\n";
    baseStr += "Normal: (" + std::to_string(normal.x) + ", " + std::to_string(normal.y) + ", " + std::to_string(normal.z) + ")\n";
    baseStr += "Color: (" + std::to_string(color.x) + ", " + std::to_string(color.y) + ", " + std::to_string(color.z) + ")\n";
    return baseStr;
}