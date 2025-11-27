#pragma once
#include "Shape.h"
#include "../defines/Defines.h"


class Sphere : public Shape
{
public:
    // Default constructor with default values
    Sphere()
        : Shape(vec3(-0.05f, 0.0f, -0.3f), "Sphere " + std::to_string(nextID), nullptr), radius(0.15f) {}

    Sphere(float r, const vec3& center) 
        : Shape(center, "Sphere "  + std::to_string(nextID), nullptr), radius(r) {}


    Sphere(float r, const vec3& center, const std::string& name, Material* mat)
    : Shape(center, name, mat), radius(r) {}
    Sphere(float r, const vec3 &center, Material *mat)
        : Shape(center, "Sphere " + std::to_string(nextID), mat), radius(r) {}
    Sphere(float r, const vec3 &center, const std::string &name)
        : Shape(center, name, nullptr), radius(r) {}

    GPUSphere toGPU() const;

    ShapeType getType() const override { return ShapeType::SPHERE; }
    

    inline float getRadius() const { return radius; }
    inline vec3 getCenter() const { return position; }

    
    // Setters spécifiques à Sphere
    void setRadius(float r) { radius = r; }

    std::string toString() const;

private:
    float radius;   // Attribut spécifique à Sphere
    // Note: material is inherited from Shape base class, no need to redeclare
};