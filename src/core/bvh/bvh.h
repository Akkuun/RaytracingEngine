#pragma once
#include <vector>
#include "../shapes/Triangle.h"
#include "../shapes/Mesh.h"
#include "bvhNode.h"
#include "../math/aabb.h"
#include "../defines/Defines.h"

class BVH
{
public:
    BVH() = default;
    ~BVH() = default;

    void build(const Mesh &mesh);

    // Convert BVH to GPU format - returns header info and fills the output vectors
    GPUBVH toGPU(std::vector<GPUBVHNode> &outNodes, std::vector<GPUTriangle> &outTriangles) const;

    inline bvhNode *getRoot() const { return root; }
    void printRecursive(bvhNode *node, int depth = 0) const;

private:
    bvhNode *buildRecursive(std::vector<Triangle>::iterator start, std::vector<Triangle>::iterator end, int depth = 0);

    // Helper for flattening the BVH tree into arrays
    void flattenBVH(bvhNode *node, std::vector<GPUBVHNode> &outNodes,
                    std::vector<GPUTriangle> &outTriangles, int &currentNodeIdx) const;

    std::vector<Triangle> triangles;
    bvhNode *root = nullptr; // represent the entire mesh unsplit
    AABB boundingBox;
    int maxDepth = 5;
    int associatedMeshID = -1; // ID of the mesh this BVH belongs to to avoid to send number of BVH in kernel

    inline int getAssociatedMeshID() const { return associatedMeshID; }
    inline int getMaxDepth() const { return maxDepth; }
};