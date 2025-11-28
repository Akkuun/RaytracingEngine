#pragma once
#include <vector>
#include "../shapes/Triangle.h"
#include "../shapes/Mesh.h"
#include "bvhNode.h"
#include "../math/aabb.h"
#include "../defines/Defines.h"

class BVH {
public:
    BVH() = default;
    ~BVH() = default;
   
    void build(const Mesh& mesh);
    GPUBVH toGPU() const;

private:
    bvhNode* buildRecursive(std::vector<Triangle>::iterator start, std::vector<Triangle>::iterator end, const int &actualDepth = 0);
    std::vector<Triangle> triangles;
    bvhNode* root = nullptr; // represent the entire mesh unsplit
    AABB boundingBox;
    int maxDepth = 5;
    int associatedMeshID = -1; // ID of the mesh this BVH belongs to to avoid to send number of BVH in kernel

    inline int getAssociatedMeshID() const { return associatedMeshID; }
    inline int getMaxDepth() const { return maxDepth; }
    inline bvhNode* getRoot() const { return root; }

  
};