#pragma once
#include "../math/vec3.h"

class Shape
{
public:
    virtual ~Shape() = default;

    

private:
    vec3 position; // Position of the shape in 3D space
    vec3 scale;   // Scale of the shape
    vec3 rotation; // Rotation of the shape (Euler angles)
};