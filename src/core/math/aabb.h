#pragma once
#include "../math/vec3.h"
#include <cfloat>

class AABB {
public:
    AABB() : minPoint(vec3(FLT_MAX, FLT_MAX, FLT_MAX)), maxPoint(vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {}
    AABB(const vec3& min, const vec3& max) : minPoint(min), maxPoint(max) {}

    vec3 minPoint;
    vec3 maxPoint;
};