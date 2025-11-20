// Triangle shape (CPU-side)
#pragma once

#include "Shape.h"
#include "../defines/Defines.h"

class Triangle : public Shape
{
public:
    // Default constructor with default values (upward-pointing triangle)
    Triangle()
        : Shape(vec3(0.0f, 0.0f, -1.5f),
          std::string("Triangle " + std::to_string(nextID)), nullptr),
          v0(-0.2f, -0.15f, -1.5f),
          v1(0.2f, -0.15f, -1.5f),
          v2(0.0f, 0.15f, -1.5f),
          color(0.9f, 0.1f, 0.9f),
          emission(0.0f, 0.0f, 0.0f) {}
    
    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& col)
        : Shape((v0 + v1 + v2) * (1.0f / 3.0f), "Triangle " + std::to_string(nextID), nullptr), v0(v0), v1(v1), v2(v2), color(col), emission(0.0f, 0.0f, 0.0f) {}

    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& col, std::string name)
    : Shape((v0 + v1 + v2) * (1.0f / 3.0f), name, nullptr), v0(v0), v1(v1), v2(v2), color(col), emission(0.0f, 0.0f, 0.0f) {}

    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& col, const vec3& emi)
        : Shape((v0 + v1 + v2) * (1.0f / 3.0f)), v0(v0), v1(v1), v2(v2), color(col), emission(emi) {}

    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& col, const vec3& emi, std::string name)
    : Shape((v0 + v1 + v2) * (1.0f / 3.0f), name, nullptr), v0(v0), v1(v1), v2(v2), color(col), emission(emi) {}

    // Special constructor for mesh triangles (doesn't increment ID)
    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& col, const vec3& emi, bool isPartOfMesh)
    : Shape((v0 + v1 + v2) * (1.0f / 3.0f), false), v0(v0), v1(v1), v2(v2), color(col), emission(emi) {
        (void)isPartOfMesh; // unused, just for signature differentiation
    }

    // Convert to GPU-friendly struct
    GPUTriangle toGPU() const;

    ShapeType getType() const override { return ShapeType::TRIANGLE; }

    // Getters
    const vec3& getV0() const { return v0; }
    const vec3& getV1() const { return v1; }
    const vec3& getV2() const { return v2; }
    const vec3& getColor() const { return color; }
    const vec3& getEmission() const { return emission; }

    // Setters
    void setV0(const vec3& vv) { v0 = vv; }
    void setV1(const vec3& vv) { v1 = vv; }
    void setV2(const vec3& vv) { v2 = vv; }
    void setColor(const vec3& c) { color = c; }
    void setEmission(const vec3& e) { emission = e; }

    std::string toString() const;

private:
    vec3 v0;
    vec3 v1;
    vec3 v2;
    vec3 color;
    vec3 emission;
};
