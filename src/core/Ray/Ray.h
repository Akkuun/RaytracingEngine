#pragma once
#include "../math/vec3.h"
class Ray
{

public:
    vec3 origin;    // Origin point of the ray
    vec3 direction; // Direction vector of the ray (should be normalized)

    Ray(const vec3 &orig, const vec3 &dir) : origin(orig), direction(dir.normalize()) {}

    // Compute a point along the ray at parameter t
    vec3 at(double t) const
    {
        return origin + t * direction;
    }
};