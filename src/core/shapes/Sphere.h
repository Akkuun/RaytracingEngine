#pragma once
#include "Shape.h"
#include "../defines/Defines.h"

// Struct GPU-compatible (for the kernel)
struct GPUSphere
{
    float center[3]; // x, y, z
    float radius;
    float material[3]; // r, g, b
};

class Sphere : public Shape
{
public:
    Sphere(double r, vec3 c) : radius(r), center(c) {}
    ~Sphere() override = default;

    // Méthode pour convertir vers GPU
    GPUSphere toGPU() const;

    inline ShapeType getType() const override { return ShapeType::SPHERE; }

private:
    double radius;
    vec3 center;
};