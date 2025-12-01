#pragma once
#include "../math/vec3.h"
#include <cfloat>
#include "../shapes/Triangle.h"
#include "../defines/Defines.h"

class AABB
{
public:
    AABB() : minPoint(vec3(FLT_MAX, FLT_MAX, FLT_MAX)), maxPoint(vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX)), center(vec3(0, 0, 0)) {}
    AABB(const vec3 &min, const vec3 &max) : minPoint(min), maxPoint(max), center((min + max) * 0.5f) {}

    vec3 minPoint;
    vec3 maxPoint;
    vec3 center;

    // Expand the AABB to include a given point
    void GrowToInclude(const vec3 &point)
    {
        minPoint.x = std::min(minPoint.x, point.x);
        minPoint.y = std::min(minPoint.y, point.y);
        minPoint.z = std::min(minPoint.z, point.z);

        maxPoint.x = std::max(maxPoint.x, point.x);
        maxPoint.y = std::max(maxPoint.y, point.y);
        maxPoint.z = std::max(maxPoint.z, point.z);
    }
    // Expand the AABB to include a given triangle
    void GrowToInclude(const Triangle &triangle)
    {
        GrowToInclude(triangle.getV0());
        GrowToInclude(triangle.getV2()); // TODO if swapped v1 and v2 in Triangle is correct
        GrowToInclude(triangle.getV1());
    }

    // return surface area of the AABB
    float SurfaceArea() const
    {
        vec3 extents = maxPoint - minPoint;
        return 2.0f * (extents.x * extents.y + extents.y * extents.z + extents.z * extents.x);
    }

    // Convert to GPU-compatible structure
    AABBGPU toGPU() const
    {
        AABBGPU gpuAABB;
        gpuAABB.minPoint = {minPoint.x, minPoint.y, minPoint.z, 0.0f};
        gpuAABB.maxPoint = {maxPoint.x, maxPoint.y, maxPoint.z, 0.0f};
        return gpuAABB;
    }
};