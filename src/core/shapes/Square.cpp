#include "Square.h"

GPUSquare Square::toGPU() const
{
    GPUSquare gpuSquare;
    
    // Position
    gpuSquare.pos[0] = static_cast<float>(position.x);
    gpuSquare.pos[1] = static_cast<float>(position.y);
    gpuSquare.pos[2] = static_cast<float>(position.z);
    
    // U vector
    gpuSquare.u_vec[0] = static_cast<float>(u_vec.x);
    gpuSquare.u_vec[1] = static_cast<float>(u_vec.y);
    gpuSquare.u_vec[2] = static_cast<float>(u_vec.z);
    
    // V vector
    gpuSquare.v_vec[0] = static_cast<float>(v_vec.x);
    gpuSquare.v_vec[1] = static_cast<float>(v_vec.y);
    gpuSquare.v_vec[2] = static_cast<float>(v_vec.z);
    
    // Normal
    gpuSquare.normal[0] = static_cast<float>(normal.x);
    gpuSquare.normal[1] = static_cast<float>(normal.y);
    gpuSquare.normal[2] = static_cast<float>(normal.z);
    
    // Color
    gpuSquare.color[0] = static_cast<float>(color.x);
    gpuSquare.color[1] = static_cast<float>(color.y);
    gpuSquare.color[2] = static_cast<float>(color.z);

    return gpuSquare;
}
