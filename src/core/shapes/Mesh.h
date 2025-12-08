#pragma once
#include "Shape.h"
#include "../defines/Defines.h"

#include <vector>
#include "Triangle.h"

#include "../math/aabb.h"
#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../bvh/bvh.h"

struct MeshVertex
{
    MeshVertex();
    MeshVertex(const vec3 &_p);
    MeshVertex(const MeshVertex &vertex);
    virtual ~MeshVertex();
    MeshVertex &operator=(const MeshVertex &vertex);
    vec3 position;
    vec3 normal = vec3(0.0f, 0.0f, 0.0f);
    float u, v;
};

struct MeshTriangle
{
    MeshTriangle();
    MeshTriangle(const MeshTriangle &t);
    MeshTriangle(unsigned int v0, unsigned int v1, unsigned int v2);
    MeshTriangle(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int index);
    unsigned int &operator[](unsigned int iv);
    unsigned int operator[](unsigned int iv) const;
    virtual ~MeshTriangle();
    MeshTriangle &operator=(const MeshTriangle &t);
    unsigned int v[4];
};

class Mesh : public Shape
{
private:
    std::vector<MeshVertex> vertices;
    std::vector<MeshTriangle> triangles;
    std::vector<Triangle> cpuTriangles;
    std::string filename;
    BVH bvh = BVH(*this);

public:
    Mesh() : Shape() {}
    Mesh(const std::string &filename) : Shape(extractFilename(filename) + " " + std::to_string(nextID))
    {
        loadOFF(filename);
        recomputeNormals();
        setPosition(vec3(0.0f));
        scaleToUnit();
        generateCpuTriangles();
        this->filename = filename;
    }
    ShapeType getType() const override { return ShapeType::MESH; }

    // Extract filename from path (removes path and extension)
    static std::string extractFilename(const std::string &filepath);
    void loadOFF(const std::string &filename);
    void recomputeNormals();
    void generateCpuTriangles()
    {
        cpuTriangles.clear();
        for (const auto &tri : triangles)
        {
            const vec3 &v0 = vertices[tri.v[0]].position;
            const vec3 &v1 = vertices[tri.v[1]].position;
            const vec3 &v2 = vertices[tri.v[2]].position;
            // don´t increment ID for mesh triangles
            cpuTriangles.emplace_back(v0, v1, v2, true);
        }
    }
    const std::vector<Triangle> &getTriangles() const
    {
        return cpuTriangles;
    }

    vec3 getCenterPos() const
    {
        vec3 center(0.0f, 0.0f, 0.0f);
        for (const auto &vertex : vertices)
        {
            center += vertex.position;
        }
        center /= static_cast<float>(vertices.size());
        return center;
    }
    void applyTransformationMatrix(Mat3 &mat);
    AABB computeAABB();
    void scaleToUnit();
    // broken
    void scale(const vec3 &factors);
    void setPosition(const vec3 &pos);
    void translate(const vec3 &offset);
    // Uses angles in radians
    void rotate(const vec3 &angles);
    BVH &getBVH() { return bvh; }

    std::string getFilename() const { return filename; }
};