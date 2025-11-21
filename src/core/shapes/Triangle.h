// Triangle shape (CPU-side)
#pragma once

#include "Shape.h"
#include "../defines/Defines.h"

class Triangle : public Shape
{
public:
    // Default constructor with default values (upward-pointing triangle)
    Triangle()
        : Shape(vec3(-0.05f, 0.0f, -0.3f),
                std::string("Triangle " + std::to_string(nextID)), nullptr),
          v0(-0.2f, -0.15f, -1.5f),
          v1(0.0f, 0.15f, -1.5f), // swapped v1 and v2 to fix normal direction
          v2(0.2f, -0.15f, -1.5f)
    {
    }

    Triangle(const vec3 &v0, const vec3 &v1, const vec3 &v2)
        : Shape((v0 + v1 + v2) * (1.0f / 3.0f), "Triangle " + std::to_string(nextID), nullptr), v0(v0), v1(v1), v2(v2) {}

    Triangle(const vec3 &v0, const vec3 &v1, const vec3 &v2, std::string name)
        : Shape((v0 + v1 + v2) * (1.0f / 3.0f), name, nullptr), v0(v0), v1(v1), v2(v2) {}

    // Special constructor for mesh triangles (doesn't increment ID)
    Triangle(const vec3 &v0, const vec3 &v1, const vec3 &v2, bool isPartOfMesh)
        : Shape((v0 + v1 + v2) * (1.0f / 3.0f), false), v0(v0), v1(v1), v2(v2)
    {
        (void)isPartOfMesh; // unused, just for signature differentiation
    }

    // Convert to GPU-friendly struct
    GPUTriangle toGPU() const;

    ShapeType getType() const override { return ShapeType::TRIANGLE; }

    // Getters
    const vec3 &getV0() const { return v0; }
    const vec3 &getV1() const { return v1; }
    const vec3 &getV2() const { return v2; }

    // Setters
    void setV0(const vec3 &vv) { v0 = vv; }
    void setV1(const vec3 &vv) { v1 = vv; }
    void setV2(const vec3 &vv) { v2 = vv; }

    std::string toString() const;

private:
    vec3 v0;
    vec3 v1;
    vec3 v2;
};
