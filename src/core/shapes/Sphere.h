#pragma once
#include "Shape.h"
#include "../defines/Defines.h"


class Sphere : public Shape
{
public:
    Sphere(float r, const vec3& center) 
        : Shape(center), radius(r), emi(0.0f, 0.0f, 0.0f), color(1.0f, 1.0f, 1.0f) {}
    
    Sphere(float r, const vec3& center, const vec3& color)
        : Shape(center), radius(r), emi(0.0f, 0.0f, 0.0f), color(color) {}
    
    ~Sphere() override = default;

    // Méthode pour convertir vers GPU
    GPUSphere toGPU() const;

    inline ShapeType getType() const override { return ShapeType::SPHERE; }
    
    // Getters spécifiques à Sphere
    float getRadius() const { return radius; }
    const vec3& getColor() const { return color; }
    const vec3& getEmission() const { return emi; }
    
    // Setters spécifiques à Sphere
    void setRadius(float r) { radius = r; }
    void setColor(const vec3& c) { color = c; }
    void setEmission(const vec3& e) { emi = e; }

private:
    float radius;   // Attribut spécifique à Sphere
    vec3 emi;       // Emission (light)
    vec3 color;     // Color
};