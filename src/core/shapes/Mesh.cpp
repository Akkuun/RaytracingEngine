#include "Mesh.h"

#include <iostream>
#include <fstream>
#include <sstream>

void Mesh::loadOFF(const std::string &filename)
{
    std::ifstream in(filename.c_str());
    if (!in)
        exit(EXIT_FAILURE);

    std::string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    vertices.resize(sizeV);
    triangles.resize(sizeT);

    for (unsigned int i = 0; i < sizeV; ++i)
    {
        float x, y, z;
        in >> x >> y >> z;
        vec3 position = {x, y, z};
        vertices[i].position = position;
    }

    for (unsigned int i = 0; i < sizeT; ++i)
    {
        unsigned int v0, v1, v2;
        in >> tmp >> v0 >> v1 >> v2; // tmp is the number of vertices per face (should be 3)
        triangles[i] = MeshTriangle(v0, v1, v2, i);
        
        // Ignore per face attributes
        std::string restOfLine;
        std::getline(in, restOfLine);
    }
}

void Mesh::recomputeNormals()
{
    for (unsigned int i = 0; i < vertices.size(); i++)
        vertices[i].normal = vec3(0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < triangles.size(); i++)
    {
        vec3 e01 = vertices[triangles[i].v[1]].position - vertices[triangles[i].v[0]].position;
        vec3 e02 = vertices[triangles[i].v[2]].position - vertices[triangles[i].v[0]].position;
        vec3 n = vec3::cross(e01, e02);
        n.normalize();
        for (unsigned int j = 0; j < 3; j++)
            vertices[triangles[i].v[j]].normal += n;
    }
    for (unsigned int i = 0; i < vertices.size(); i++)
        vertices[i].normal.normalize();
}

AABB Mesh::computeAABB()
{
    if (vertices.empty())
        return AABB();

    vec3 minPoint = vertices[0].position;
    vec3 maxPoint = vertices[0].position;

    for (const auto &vertex : vertices)
    {
        const vec3 &pos = vertex.position;
        if (pos.x < minPoint.x)
            minPoint.x = pos.x;
        if (pos.y < minPoint.y)
            minPoint.y = pos.y;
        if (pos.z < minPoint.z)
            minPoint.z = pos.z;

        if (pos.x > maxPoint.x)
            maxPoint.x = pos.x;
        if (pos.y > maxPoint.y)
            maxPoint.y = pos.y;
        if (pos.z > maxPoint.z)
            maxPoint.z = pos.z;
    }

    return AABB(minPoint, maxPoint);
}

void Mesh::scaleToUnit()
{
    AABB aabb = computeAABB();
    vec3 size = aabb.maxPoint - aabb.minPoint;
    if (size.x > 0)
    {
        for (auto &vertex : vertices)
        {
            vertex.position.x /= size.x;
        }
    }
    if (size.y > 0)
    {
        for (auto &vertex : vertices)
        {
            vertex.position.y /= size.y;
        }
    }
    if (size.z > 0)
    {
        for (auto &vertex : vertices)
        {
            vertex.position.z /= size.z;
        }
    }
}

void Mesh::applyTransformationMatrix(Mat3 &mat)
{
    for (auto &vertex : vertices)
    {
        vertex.position = mat * vertex.position;
    }
}

void Mesh::scale(const vec3 &factors)
{
    Mat3 scaleMatrix = {
        factors.x, 0.0f, 0.0f,
        0.0f, factors.y, 0.0f,
        0.0f, 0.0f, factors.z};
    applyTransformationMatrix(scaleMatrix);
}

void Mesh::setPosition(const vec3 &pos)
{
    vec3 offset = pos - this->position;
    translate(offset);
    this->position = pos;
}

void Mesh::translate(const vec3 &offset)
{
    for (auto &vertex : vertices)
    {
        vertex.position += offset;
    }
}

void Mesh::rotate(const vec3 &angles)
{
    Mat3 rotationMatrix = Mat3::rotationX(angles.x) * Mat3::rotationY(angles.y) * Mat3::rotationZ(angles.z);

    vec3 center = getCenterPos();
    translate(vec3(-center.x, -center.y, -center.z));

    for (auto &vertex : vertices)
    {
        vertex.position = rotationMatrix * vertex.position;
    }

    translate(center);
}

// MeshVertex implementations
MeshVertex::MeshVertex() {}
MeshVertex::MeshVertex(const vec3 &_p) : position(_p), u(0), v(0) {}
MeshVertex::MeshVertex(const MeshVertex &vertex) : position(vertex.position), u(vertex.u), v(vertex.v) {}
MeshVertex::~MeshVertex() {}
MeshVertex &MeshVertex::operator=(const MeshVertex &vertex)
{
    position = vertex.position;
    u = vertex.u;
    v = vertex.v;
    return *this;
}

// MeshTriangle implementations
MeshTriangle::MeshTriangle() { v[0] = v[1] = v[2] = v[3] = 0; }
MeshTriangle::MeshTriangle(const MeshTriangle &t)
{
    v[0] = t.v[0];
    v[1] = t.v[1];
    v[2] = t.v[2];
    v[3] = t.v[3];
}
MeshTriangle::MeshTriangle(unsigned int v0, unsigned int v1, unsigned int v2)
{
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = 0;
}
MeshTriangle::MeshTriangle(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int index)
{
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = index;
}
unsigned int &MeshTriangle::operator[](unsigned int iv) { return v[iv]; }
unsigned int MeshTriangle::operator[](unsigned int iv) const { return v[iv]; }
MeshTriangle::~MeshTriangle() {}
MeshTriangle &MeshTriangle::operator=(const MeshTriangle &t)
{
    v[0] = t.v[0];
    v[1] = t.v[1];
    v[2] = t.v[2];
    v[3] = t.v[3];
    return *this;
}

// Mesh::extractFilename implementation
std::string Mesh::extractFilename(const std::string &filepath)
{
    size_t lastSlash = filepath.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? filepath : filepath.substr(lastSlash + 1);
    size_t lastDot = filename.find_last_of('.');
    if (lastDot != std::string::npos)
    {
        filename = filename.substr(0, lastDot);
    }
    return filename;
}