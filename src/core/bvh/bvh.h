#pragma once
#include <vector>
#include "../shapes/Triangle.h"
#include "../math/aabb.h"
#include "../defines/Defines.h"

// Forward declaration to avoid circular dependency
class Mesh;

#define QUALITY_DISABLED -1
#define QUALITY_LOW 0
#define QUALITY_HIGH 2
#define MAX_DEPTH 32

class BVH : public Shape
{
public:
    ~BVH() {
        nodes.clear();
        triangles.clear();
        buildTriangles.clear();
    }

    struct Node 
    {
        AABB boundingBox;

        int startIndex; // Index of 1st child (if triangle count is negative) otherwise index of first triangle
        int triangleCount;

        Node(AABB box, int start, int count) : boundingBox(box), startIndex(start), triangleCount(count) {}
    
        GPUBVHNode toGPU() const
        {
            GPUBVHNode gpuNode;
            gpuNode.minx = boundingBox.minPoint.x;
            gpuNode.miny = boundingBox.minPoint.y;
            gpuNode.minz = boundingBox.minPoint.z;
            gpuNode._padding1 = 0.0f;
            gpuNode.maxx = boundingBox.maxPoint.x;
            gpuNode.maxy = boundingBox.maxPoint.y;
            gpuNode.maxz = boundingBox.maxPoint.z;
            gpuNode._padding2 = 0.0f;
            gpuNode.startIndex = startIndex;
            gpuNode.triangleCount = triangleCount;
            gpuNode._padding3[0] = 0;
            gpuNode._padding3[1] = 0;
            return gpuNode;
        };
    };

    struct BVHTriangle
    {
        vec3 center;
        AABB box;
        int index;

        BVHTriangle(const Triangle &tri, int idx)
        {
            center = (tri.getV0() + tri.getV1() + tri.getV2()) / 3.0f;
            box.GrowToInclude(tri);
            index = idx;
        }

        
    };

    struct NodeList
    {
        std::vector<Node> nodes;
        int index;

        int add(Node node)
        {
            nodes.push_back(node);
            return index++;
        }

        int nodeCount() const { return static_cast<int>(nodes.size()); }
    };

    struct Split
    {
        int axis;
        float pos;
        float cost;
    };


public:
    std::vector<Triangle> triangles;
    std::vector<Node> nodes;

    NodeList nodesList;
    std::vector<BVHTriangle> buildTriangles;
    int quality;

    BVH(const Mesh &mesh, int qualityLevel = QUALITY_HIGH);

private:
    void split(int parentIndex, int triGlobalStart, int triNum, int depth=0);
    Split chooseSplit(Node node, int start, int count);
    float evaluateSplit(int splitAxis, float splitPos, int start, int count);
    float NodeCost(vec3 size, int numTris);
};