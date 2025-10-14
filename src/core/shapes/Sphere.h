#pragma once
#include "Shape.h"
class Sphere : public Shape {
public:
    
    Sphere(double r, vec3 c) : radius(r), center(c) {}
    ~Sphere() override = default;

private:
    double radius; // Radius of the sphere
    vec3 center; // Center position of the sphere

};