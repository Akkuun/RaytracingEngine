#pragma once
#include "../math/vec3.h"
#include "../defines/Defines.h"
class Shape
{
public:
    virtual ~Shape() = default;
    virtual ShapeType getType() const = 0;
    

private:
    vec3 position; // Position of the shape in 3D space
    vec3 scale;   // Scale of the shape
    vec3 rotation; // Rotation of the shape (Euler angles)
};