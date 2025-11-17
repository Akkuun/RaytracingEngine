#pragma once
#include "Shape.h"
#include "../defines/Defines.h"

#include <vector>
#include "Triangle.h"

#include "../math/aabb.h"
#include "../math/vec3.h"
#include "../math/mat3.h"


struct MeshVertex {
    inline MeshVertex () {}
    inline MeshVertex (const vec3 & _p) : position (_p), /*normal (_n) ,*/ u(0) , v(0) {}
    inline MeshVertex (const MeshVertex & vertex) : position (vertex.position), /*normal (vertex.normal) ,*/ u(vertex.u) , v(vertex.v) {}
    inline virtual ~MeshVertex () {}
    inline MeshVertex & operator = (const MeshVertex & vertex) {
        position = vertex.position;
        //normal = vertex.normal;
        u = vertex.u;
        v = vertex.v;
        return (*this);
    }
    // membres :
    vec3 position; // une position
    vec3 normal = vec3(0.0f, 0.0f, 0.0f); // une normale
    float u,v; // coordonnees uv
};

struct MeshTriangle {
    inline MeshTriangle () {
        v[0] = v[1] = v[2] = v[3] = 0;
    }
    inline MeshTriangle (const MeshTriangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];  v[3] = t.v[3];
    }
    inline MeshTriangle (unsigned int v0, unsigned int v1, unsigned int v2) {
        v[0] = v0;   v[1] = v1;   v[2] = v2;  v[3] = 0;
    }
    inline MeshTriangle (unsigned int v0, unsigned int v1, unsigned int v2, unsigned int index) {
        v[0] = v0;   v[1] = v1;   v[2] = v2;  v[3] = index;
    }
    unsigned int & operator [] (unsigned int iv) { return v[iv]; }
    unsigned int operator [] (unsigned int iv) const { return v[iv]; }
    inline virtual ~MeshTriangle () {}
    inline MeshTriangle & operator = (const MeshTriangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];   v[3] = t.v[3];
        return (*this);
    }
    // membres :
    unsigned int v[4]; // v[0 - 2] : indices des 3 sommets du triangle, v[3] : index du triangle
};

class Mesh : public Shape
{
private:
    std::vector<MeshVertex> vertices;
    std::vector<MeshTriangle> triangles;
    std::vector<Triangle> cpuTriangles;

    vec3 color = vec3(0.9f, 0.9f, 0.1f);
    vec3 emission = vec3(0.0f);

public:
    Mesh() : Shape() {}
    Mesh(const std::string& filename) : Shape(extractFilename(filename) + " " + std::to_string(nextID)) {
        loadOFF(filename);
        recomputeNormals();
        setPosition(vec3(0.0f));
        scaleToUnit();
        generateCpuTriangles();
    }
    ShapeType getType() const override { return ShapeType::MESH; }
    
    // Extract filename from path (removes path and extension)
    static std::string extractFilename(const std::string& filepath) {
        // Find last '/' or '\' (for cross-platform compatibility)
        size_t lastSlash = filepath.find_last_of("/\\");
        std::string filename = (lastSlash == std::string::npos) ? filepath : filepath.substr(lastSlash + 1);
        
        // Remove extension (.off, .obj, etc.)
        size_t lastDot = filename.find_last_of('.');
        if (lastDot != std::string::npos) {
            filename = filename.substr(0, lastDot);
        }
        
        return filename;
    }
    void loadOFF(const std::string& filename);
    void recomputeNormals();
    void generateCpuTriangles() {
        cpuTriangles.clear();
        for (const auto& tri : triangles) {
            const vec3& v0 = vertices[tri.v[0]].position;
            const vec3& v1 = vertices[tri.v[1]].position;
            const vec3& v2 = vertices[tri.v[2]].position;
            // don´t increment ID for mesh triangles
            cpuTriangles.emplace_back(v0, v1, v2, vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)), emission, true);
        }
    }
    const std::vector<Triangle>& getTriangles() const {
        return cpuTriangles;
    }

    vec3 getCenterPos() const {
        vec3 center(0.0f, 0.0f, 0.0f);
        for (const auto& vertex : vertices) {
            center += vertex.position;
        }
        center /= static_cast<float>(vertices.size());
        return center;
    }
    void applyTransformationMatrix(Mat3 &mat);
    AABB computeAABB();
    void scaleToUnit();
    // broken
    void scale(const vec3& factors);
    void setPosition(const vec3& pos);
    void translate(const vec3& offset);
    // Uses angles in radians
    void rotate(const vec3& angles);
};