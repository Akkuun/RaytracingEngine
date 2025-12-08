#pragma once
#include <vector>
#include "../shapes/Triangle.h"
#include "../shapes/Mesh.h"
#include "../math/aabb.h"
#include "../defines/Defines.h"

#define QUALITY_DISABLED -1
#define QUALITY_LOW 0
#define QUALITY_HIGH 2
#define MAX_DEPTH 32

class BVH
{
public:
    ~BVH() = default;

    struct Node 
    {
        AABB boundingBox;

        int startIndex; // Index of 1st child (if triangle count is negative) otherwise index of first triangle
        int triangleCount;

        Node(AABB box, int start, int count) : boundingBox(box), startIndex(start), triangleCount(count) {}
    };

    struct BVHTriangle
    {
        vec3 center;
        AABB box;
        int index;

        BVHTriangle(const Triangle &tri)
        {
            center = (tri.getV0() + tri.getV1() + tri.getV2()) / 3.0f;
            box.GrowToInclude(tri);
            index = tri.getID();
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

    BVH(const Mesh &mesh, int qualityLevel = QUALITY_HIGH) : quality(qualityLevel) {
        nodesList.index = 0;

        buildTriangles.reserve(mesh.getTriangles().size());

        AABB globalBox;
        for (const auto &tri : mesh.getTriangles()) {
            buildTriangles.emplace_back(new BVHTriangle(tri));
            globalBox.GrowToInclude(tri);
        }

        nodesList.add(Node(globalBox, -1, -1)); // root node

        if (quality == QUALITY_DISABLED)
        {
            nodesList.nodes[0].startIndex = 0;
            nodesList.nodes[0].triangleCount = static_cast<int>(triangles.size());
        }
        else {
            // split
        }
        
    
    }
private:
    void split(int parentIndex, int triGlobalStart, int triNum, int depth=0);
    Split chooseSplit(Node node, int start, int count);
    float evaluateSplit(int splitAxis, float splitPos, int start, int count);
    float NodeCost(vec3 size, int numTris);
};