#pragma once
#include "../math/aabb.h"
#include <vector>
#include "../shapes/Triangle.h"
#include "../defines/Defines.h"

class bvhNode {
public:
    bvhNode() = default;
    ~bvhNode() = default;

    AABB boundingBox = AABB();
    std::vector<Triangle> triangles = std::vector<Triangle>();
    bvhNode* childA = nullptr; // left child
    bvhNode* childB = nullptr; // right child

    GPUBVHNode toGPU() const {
        GPUBVHNode gpuNode;
        // TODO serialize the bvhNode data into gpuNode
        return gpuNode;
    }

    inline vec3 getMinOfBoundingBox() const { return boundingBox.minPoint; }
    inline vec3 getMaxOfBoundingBox() const { return boundingBox.maxPoint; }

};