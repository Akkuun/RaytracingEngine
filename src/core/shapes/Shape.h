#pragma once
#include "../math/vec3.h"
#include "../defines/Defines.h"

class Shape
{
public:
    virtual ~Shape() = default;
    virtual ShapeType getType() const = 0;
    
    // Getters
    const vec3& getPosition() const { return position; }
    const vec3& getScale() const { return scale; }
    const vec3& getRotation() const { return rotation; }
    
    // Setters
    void setPosition(const vec3& pos) { position = pos; }
    void setScale(const vec3& s) { scale = s; }
    void setRotation(const vec3& rot) { rotation = rot; }

protected:
    // Protected so derived classes can access them
    vec3 position; // Position of the shape in 3D space
    vec3 scale;    // Scale of the shape (default: 1,1,1)
    vec3 rotation; // Rotation of the shape (Euler angles)
    
    // Protected constructor to initialize base attributes
    Shape() : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f) {}
    Shape(const vec3& pos) : position(pos), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f) {}
};