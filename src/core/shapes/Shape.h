#pragma once
#include "../math/vec3.h"
#include "../defines/Defines.h"
#include "../material/Material.h"

class Shape
{
public:
    virtual ~Shape() {
        // Clean up material if it was allocated by this shape
        if (material != nullptr) {
            delete material;
            material = nullptr;
        }
    }
    virtual ShapeType getType() const { return ShapeType::UNDEFINED; }

    // Getters
    inline const vec3 &getPosition() const { return position; }
    inline const vec3 &getScale() const { return scale; }
    inline const vec3 &getRotation() const { return rotation; }
    inline int getID() const { return id; }
    inline const std::string &getName() const { return shapeName; }
    inline Material *getMaterial() const { return material; }
    // Setters
    inline void setPosition(const vec3 &pos) { position = pos; }
    inline void setScale(const vec3 &s) { scale = s; }
    inline void setRotation(const vec3 &rot) { rotation = rot; }
    inline void setMaterial(Material *mat) {
        if (material != nullptr) {
            delete material;
        }
        material = mat;
    }

    Shape() : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(nextID++), shapeName("Shape " + std::to_string(id)), material(new Material()) {
        std::cout << "1" <<std::endl;
    }
    Shape(bool dontIncrementID) : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(-1), shapeName(""), material(new Material())
    {
        (void)dontIncrementID; // unused, just for signature differentiation
    }
    Shape(const vec3 &pos, const std::string &name, Material *mat) : position(pos), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(nextID++), shapeName(name), material(mat) {
        std::cout << "2" <<std::endl;
    }
    Shape(const vec3 &pos) : position(pos), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(nextID++), shapeName("Shape " + std::to_string(id)), material(new Material()) {
        std::cout << "3" <<std::endl;
    }
    Shape(std::string name) : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(nextID++), shapeName(name), material(new Material()) {
        std::cout << "4" <<std::endl;
    }

    // Don't increment ID when we add triangles that are part of a mesh
    Shape(const vec3 &pos, bool incrementID) : position(pos), scale(1.0f, 1.0f, 1.0f), rotation(0.0f, 0.0f, 0.0f), id(-1), shapeName(""), material(nullptr)
    {
        (void)incrementID; // unused, just for signature differentiation
    }

    std::string toString() const
    {
        return "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")\n" +
               "Scale: (" + std::to_string(scale.x) + ", " + std::to_string(scale.y) + ", " + std::to_string(scale.z) + ")\n" +
               "Rotation: (" + std::to_string(rotation.x) + ", " + std::to_string(rotation.y) + ", " + std::to_string(rotation.z) + ")\n";
    }

protected:
    // Protected so derived classes can access them
    vec3 position;                // Position of the shape in 3D space
    vec3 scale;                   // Scale of the shape (default: 1,1,1)
    vec3 rotation;                // Rotation of the shape (Euler angles)
    int id;                       // Unique identifier for the shape
    inline static int nextID = 0; // Header definition to allow a static variable trackable across all Shape instances
    std::string shapeName;
    Material *material; // Material associated with the shape
};