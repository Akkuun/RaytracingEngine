#pragma once
#include "Shape.h"
#include "../defines/Defines.h"


class Sphere : public Shape
{
public:
    Sphere(float r, const vec3& center) 
        : Shape(center), radius(r), color(1.0f, 1.0f, 1.0f) {}
    
    Sphere(float r, const vec3& center, const vec3& color)
        : Shape(center), radius(r), color(color) {}
    


    GPUSphere toGPU() const;

    ShapeType getType() const override { return ShapeType::SPHERE; }
    

    float getRadius() const { return radius; }
    const vec3& getColor() const { return color; }

    
    // Setters spécifiques à Sphere
    void setRadius(float r) { radius = r; }
    void setColor(const vec3& c) { color = c; }

    std::string toString() const;

private:
    float radius;   // Attribut spécifique à Sphere
    vec3 color;     // Color
};