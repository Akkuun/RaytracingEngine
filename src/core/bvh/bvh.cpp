#include "bvh.h"
#include <algorithm>



void BVH::build(const Mesh& mesh) {
    triangles = mesh.getTriangles();
    associatedMeshID = mesh.getID();
    if (triangles.empty()) return;
    root = buildRecursive(triangles.begin(), triangles.end()); // setup the division
    boundingBox = root->boundingBox;
}


bvhNode* BVH::buildRecursive(std::vector<Triangle>::iterator start, std::vector<Triangle>::iterator end, const int &actualDepth) {
    if(actualDepth == maxDepth) return nullptr; // stop condition to avoid too deep recursion
    
    auto node = new bvhNode();
    // we add all triangles to the total bounding box to have the proper limits
    for (auto it = start; it != end; ++it) {
        node->boundingBox.GrowToInclude(*it);
    }
    size_t numTriangles = end - start;
    if (numTriangles <= 4) { // leaf
        node->triangles.assign(start, end);
        return node;
    }
    // find split axis (either x, y or z) to improve balance for search performance
    vec3 extent = node->boundingBox.maxPoint - node->boundingBox.minPoint;
    int axis = 0; // 0=x, 1=y, 2=z
    if (extent.y > extent[axis]) axis = 1;
    if (extent.z > extent[axis]) axis = 2;
    // compare function to sort triangles by centroid along the chosen axis
    auto comparator = [axis](const Triangle& a, const Triangle& b) {
        vec3 ca = (a.getV0() + a.getV1() + a.getV2()) * (1.0f / 3.0f);
        vec3 cb = (b.getV0() + b.getV1() + b.getV2()) * (1.0f / 3.0f);
        return ca[axis] < cb[axis];
    };
    std::sort(start, end, comparator);
    // split at midpoint of the sorted list
    auto mid = start + numTriangles / 2;
    // recursively build child nodes
    node->childA = buildRecursive(start, mid);
    node->childB = buildRecursive(mid, end);
    return node;
}

GPUBVH BVH::toGPU() const {
    GPUBVH gpuBVH;
    // TODO serialize the BVH data into gpuBVH
    return gpuBVH;
}



