#pragma once
#include "../math/vec3.h"
#include "../defines/Defines.h"

class Shape
{
public:
    virtual ~Shape() = default;
    virtual ShapeType getType() const { return ShapeType::UNDEFINED; }

    // Getters
    inline const vec3& getPosition() const { return position; }
    inline const vec3& getScale() const { return scale; }
    inline const vec3& getRotation() const { return rotation; }
    inline int getID() const { return id; }
    // Setters
    inline void setPosition(const vec3& pos) { position = pos; }
    inline void setScale(const vec3& s) { scale = s; }
    inline void setRotation(const vec3& rot) { rotation = rot; }
    Shape() : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(nextID++) {}
    Shape(const vec3& pos) : position(pos), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(nextID++) {}

    std::string toString() const {
        return "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")\n" +
               "Scale: (" + std::to_string(scale.x) + ", " + std::to_string(scale.y) + ", " + std::to_string(scale.z) + ")\n" +
               "Rotation: (" + std::to_string(rotation.x) + ", " + std::to_string(rotation.y) + ", " + std::to_string(rotation.z) + ")\n";
    }

protected:
    // Protected so derived classes can access them
    vec3 position; // Position of the shape in 3D space
    vec3 scale;    // Scale of the shape (default: 1,1,1)
    vec3 rotation; // Rotation of the shape (Euler angles)
    int id;        // Unique identifier for the shape
    inline static int nextID = 0; // Header definition to allow a static variable trackable across all Shape instances
};