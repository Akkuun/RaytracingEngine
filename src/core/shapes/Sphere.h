#pragma once
#include "Shape.h"
#include "../defines/Defines.h"


class Sphere : public Shape
{
public:
    // Default constructor with default values
    Sphere() 
        : Shape(vec3(0.0f, 0.0f, -1.5f)), radius(0.15f), color(0.9f, 0.9f, 0.9f), emission(0.0f, 0.0f, 0.0f) {}
    
    Sphere(float r, const vec3& center) 
        : Shape(center), radius(r), color(1.0f, 1.0f, 1.0f), emission(0.0f, 0.0f, 0.0f) {}
    
    Sphere(float r, const vec3& center, const vec3& color)
        : Shape(center), radius(r), color(color), emission(0.0f, 0.0f, 0.0f) {}
    
    Sphere(float r, const vec3& center, const vec3& color, const vec3& emi)
        : Shape(center), radius(r), color(color), emission(emi) {}


    GPUSphere toGPU() const;

    ShapeType getType() const override { return ShapeType::SPHERE; }
    

    float getRadius() const { return radius; }
    const vec3& getColor() const { return color; }
    const vec3& getEmission() const { return emission; }

    
    // Setters spécifiques à Sphere
    void setRadius(float r) { radius = r; }
    void setColor(const vec3& c) { color = c; }
    void setEmission(const vec3& e) { emission = e; }

    std::string toString() const;

private:
    float radius;   // Attribut spécifique à Sphere
    vec3 color;     // Color
    vec3 emission;  // Emission (light)
};