#pragma once
#include "../math/aabb.h"
#include <vector>
#include "../shapes/Triangle.h"
#include "../defines/Defines.h"

class bvhNode
{
public:
    bvhNode() = default;
    ~bvhNode() = default;

    AABB boundingBox = AABB();
    std::vector<Triangle> triangles = std::vector<Triangle>();
    bvhNode *childA = nullptr; // left child
    bvhNode *childB = nullptr; // right child

    // Convert to GPU node
    // childIdx: index of left child in flattened array (-1 if leaf)
    // triStartIdx: start index of triangles in flattened triangle array
    // triCount: number of triangles in this leaf node
    GPUBVHNode toGPU(int childIdx, int triStartIdx, int triCount) const
    {
        GPUBVHNode gpuNode;
        gpuNode.boundingBox = boundingBox.toGPU();
        gpuNode.childIndex = childIdx;
        gpuNode.triangleStartIdx = triStartIdx;
        gpuNode.triangleCount = triCount;
        gpuNode._padding = 0;
        return gpuNode;
    }

    bool isLeaf() const { return childA == nullptr && childB == nullptr; }

    inline vec3 getMinOfBoundingBox() const { return boundingBox.minPoint; }
    inline vec3 getMaxOfBoundingBox() const { return boundingBox.maxPoint; }
};