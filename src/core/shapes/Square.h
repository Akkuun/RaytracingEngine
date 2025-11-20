#pragma once
#include "Shape.h"
#include "../defines/Defines.h"

class Square : public Shape
{
public:
    // Default constructor with default values (horizontal square)
    Square()
        : Shape(vec3(0.0f, 0.0f, -1.5f),
        "Square " + std::to_string(nextID),nullptr), 
          u_vec(0.3f, 0.0f, 0.0f), 
          v_vec(0.0f, 0.3f, 0.0f), 
          normal(0.0f, 0.0f, 1.0f) {}
    
    Square(const vec3& pos, const vec3& u, const vec3& v, const vec3& norm, const vec3& col, const std::string& name, Material* mat)
        : Shape(pos, name, mat), u_vec(u), v_vec(v), normal(norm) {}

    Square(const vec3& pos, const vec3& u, const vec3& v, const vec3& norm, const vec3& col)
    : Shape(pos, "Square " + std::to_string(nextID), nullptr), u_vec(u), v_vec(v), normal(norm) {}


    Square(const vec3& pos, const vec3& u, const vec3& v, const vec3& norm)
        : Shape(pos, "Square " + std::to_string(nextID), nullptr), u_vec(u), v_vec(v), normal(norm) {}

    // Méthode pour convertir vers GPU
    GPUSquare toGPU() const;

    ShapeType getType() const override { return ShapeType::SQUARE; }
    
    // Getters spécifiques à Square
    const vec3& getUVector() const { return u_vec; }
    const vec3& getVVector() const { return v_vec; }
    const vec3& getNormal() const { return normal; }
    const vec3& getColor() const { return color; }
    
    // Setters spécifiques à Square
    void setUVector(const vec3& u) { u_vec = u; }
    void setVVector(const vec3& v) { v_vec = v; }
    void setNormal(const vec3& n) { normal = n; }

    std::string toString() const;

private:
    // Attributs spécifiques à Square
    vec3 u_vec;     // U vector (width direction)
    vec3 v_vec;     // V vector (height direction)
    vec3 normal;    // Normal vector
    vec3 color = vec3(0.9f, 0.9f, 0.9f); // Default color
};
