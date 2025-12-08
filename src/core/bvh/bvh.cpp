#include "bvh.h"
#include <algorithm>

float max(float a, float b) {
    return (a > b) ? a : b;
}

int ceilToInt(float f) {
    int i = static_cast<int>(f);
    if (f > static_cast<float>(i)) {
        return i + 1;
    }
    return i;
}

BVH::BVH(const Mesh &mesh, int qualityLevel) : quality(qualityLevel) {
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
        nodesList.nodes[0].triangleCount = static_cast<int>(buildTriangles.size());
    }
    else {
        this->split(0, 0, static_cast<int>(buildTriangles.size()));
    }
    
    // Finalize data for GPU transfer
    triangles.resize(buildTriangles.size());
    const auto& meshTriangles = mesh.getTriangles();
    for (size_t i = 0; i < buildTriangles.size(); ++i) {
        triangles[i] = meshTriangles[buildTriangles[i].index];
    }
    
    nodes = nodesList.nodes;
}

void BVH::split(int parentIndex, int triGlobalStart, int triNum, int depth=0) {
    Node* parentNode = &nodesList.nodes[parentIndex];

    vec3 size = parentNode->boundingBox.maxPoint - parentNode->boundingBox.minPoint;

    float parentCost = NodeCost(size, triNum);

    Split split = chooseSplit(*parentNode, triGlobalStart, triNum);

    if (split.cost < parentCost && depth < MAX_DEPTH) {
        AABB leftBox;
        AABB rightBox;

        int numOnLeft = 0;

        for (int i = triGlobalStart; i < triGlobalStart + triNum; ++i) {
            BVHTriangle tri = buildTriangles[i];
            float c;
            switch(split.axis) {
                case 0:
                    c = tri.center.x;
                    break;
                case 1:
                    c = tri.center.y;
                    break;
                case 2:
                    c = tri.center.z;
                    break;
            }

            if (c < split.pos) {
                if (tri.box.minPoint.x < leftBox.minPoint.x) leftBox.minPoint.x = tri.box.minPoint.x;
                if (tri.box.minPoint.y < leftBox.minPoint.y) leftBox.minPoint.y = tri.box.minPoint.y;
                if (tri.box.minPoint.z < leftBox.minPoint.z) leftBox.minPoint.z = tri.box.minPoint.z;
                if (tri.box.maxPoint.x > leftBox.maxPoint.x) leftBox.maxPoint.x = tri.box.maxPoint.x;
                if (tri.box.maxPoint.y > leftBox.maxPoint.y) leftBox.maxPoint.y = tri.box.maxPoint.y;
                if (tri.box.maxPoint.z > leftBox.maxPoint.z) leftBox.maxPoint.z = tri.box.maxPoint.z;
                
                BVHTriangle swap = buildTriangles[numOnLeft + triGlobalStart];
                buildTriangles[numOnLeft + triGlobalStart] = tri;
                buildTriangles[i] = swap;
                numOnLeft++;
            } else {
                if (tri.box.minPoint.x < rightBox.minPoint.x) rightBox.minPoint.x = tri.box.minPoint.x;
                if (tri.box.minPoint.y < rightBox.minPoint.y) rightBox.minPoint.y = tri.box.minPoint.y;
                if (tri.box.minPoint.z < rightBox.minPoint.z) rightBox.minPoint.z = tri.box.minPoint.z;
                if (tri.box.maxPoint.x > rightBox.maxPoint.x) rightBox.maxPoint.x = tri.box.maxPoint.x;
                if (tri.box.maxPoint.y > rightBox.maxPoint.y) rightBox.maxPoint.y = tri.box.maxPoint.y;
                if (tri.box.maxPoint.z > rightBox.maxPoint.z) rightBox.maxPoint.z = tri.box.maxPoint.z;
            }
        }

        int numOnRight = triNum - numOnLeft;
        int triStartLeft = triGlobalStart + 0;
        int triStartRight = triGlobalStart + numOnLeft;

        // Split parent into two children
        Node childLeft = Node(leftBox, triStartLeft, 0);
        Node childRight = Node(rightBox, triStartRight, 0);
    
        int childIndexLeft = nodesList.add(childLeft);
        int childIndexRight = nodesList.add(childRight);

        // Update parent
        parentNode->startIndex = childIndexLeft;
        nodesList.nodes[parentIndex] = *parentNode;

        // Recursively split children
        this->split(childIndexLeft, triStartLeft, numOnLeft, depth + 1);
        this->split(childIndexRight, triStartRight, numOnRight, depth + 1);
    }
    else
    {
        // Parent is a leaf, assign triangles to it
        parentNode->startIndex = triGlobalStart;
        parentNode->triangleCount = triNum;
        nodesList.nodes[parentIndex] = *parentNode;
    }
}

float BVH::NodeCost(vec3 size, int numTris) {
    if (numTris == 0) return 0.0f;
    float area = size.x * size.x + size.y * size.y + size.z * size.z;
    return area * static_cast<float>(numTris);
}

float BVH::evaluateSplit(int splitAxis, float splitPos, int start, int count) {
    int numOnLeft = 0;
    int numOnRight = 0;

    AABB leftBox;
    AABB rightBox;

    int end = start + count;

    for (int i = start; i < end; ++i) {
        const BVHTriangle &tri = buildTriangles[i];
        float c;
        switch(splitAxis) {
            case 0:
                c = tri.center.x;
                break;
            case 1:
                c = tri.center.y;
                break;
            case 2:
                c = tri.center.z;
                break;
        }
        if (c < splitPos) {
            if (tri.box.minPoint.x < leftBox.minPoint.x) leftBox.minPoint.x = tri.box.minPoint.x;
            if (tri.box.minPoint.y < leftBox.minPoint.y) leftBox.minPoint.y = tri.box.minPoint.y;
            if (tri.box.minPoint.z < leftBox.minPoint.z) leftBox.minPoint.z = tri.box.minPoint.z;
            if (tri.box.maxPoint.x > leftBox.maxPoint.x) leftBox.maxPoint.x = tri.box.maxPoint.x;
            if (tri.box.maxPoint.y > leftBox.maxPoint.y) leftBox.maxPoint.y = tri.box.maxPoint.y;
            if (tri.box.maxPoint.z > leftBox.maxPoint.z) leftBox.maxPoint.z = tri.box.maxPoint.z;
            numOnLeft++;
        } else {
            if (tri.box.minPoint.x < rightBox.minPoint.x) rightBox.minPoint.x = tri.box.minPoint.x;
            if (tri.box.minPoint.y < rightBox.minPoint.y) rightBox.minPoint.y = tri.box.minPoint.y;
            if (tri.box.minPoint.z < rightBox.minPoint.z) rightBox.minPoint.z = tri.box.minPoint.z;
            if (tri.box.maxPoint.x > rightBox.maxPoint.x) rightBox.maxPoint.x = tri.box.maxPoint.x;
            if (tri.box.maxPoint.y > rightBox.maxPoint.y) rightBox.maxPoint.y = tri.box.maxPoint.y;
            if (tri.box.maxPoint.z > rightBox.maxPoint.z) rightBox.maxPoint.z = tri.box.maxPoint.z;
            numOnRight++;
        }
    }

    float costA = NodeCost(leftBox.maxPoint - leftBox.minPoint, numOnLeft);
    float costB = NodeCost(rightBox.maxPoint - rightBox.minPoint, numOnRight);

    return costA + costB;
}

BVH::Split BVH::chooseSplit(Node node, int start, int count) {
    Split bestSplit;
    bestSplit.cost = FLT_MAX;
    bestSplit.axis = 0;
    bestSplit.pos = 0.0f;
    if (count <= 1) {
        return bestSplit;
    }

    vec3 size = node.boundingBox.maxPoint - node.boundingBox.minPoint;

    if (quality == QUALITY_LOW) {
        int largestAxisIndex = size[0] > size[1] && size[0] > size[2] ? 0 : (size[1] > size[2] ? 1 : 2);
        float pos;
        if (largestAxisIndex == 0) {
            pos = node.boundingBox.minPoint.x + size.x * 0.5f;
        } else if (largestAxisIndex == 1) {
            pos = node.boundingBox.minPoint.y + size.y * 0.5f;
        } else {
            pos = node.boundingBox.minPoint.z + size.z * 0.5f;
        }
        bestSplit.axis = largestAxisIndex;
        bestSplit.pos = pos;
        // cost is not used in low quality
        return bestSplit;
    }

    float bestPos = 0.0f;
    int bestAxis = 0;
    int maxTests = count < 10 ? 3 : 5;
    float maxAxis = max(size.x, max(size.y, size.z));
    float bestCost = FLT_MAX;

    for (int axis = 0; axis < 3; ++axis) {
        float axisSize;
        float axisMin;
        
        switch(axis) {
            case 0:
                axisSize = size.x;
                axisMin = node.boundingBox.minPoint.x;
                break;
            case 1:
                axisSize = size.y;
                axisMin = node.boundingBox.minPoint.y;
                break;
            case 2:
                axisSize = size.z;
                axisMin = node.boundingBox.minPoint.z;
                break;
        }

        int numSplitTests = ceilToInt(maxTests * (axisSize / maxAxis));
        numSplitTests = max(numSplitTests, 2);

        for (int i = 0; i < numSplitTests; ++i) {
            float splitT = (i + 1) / static_cast<float>(numSplitTests + 1);
            float splitPos = axisMin + axisSize * splitT;
            float cost = evaluateSplit(axis, splitPos, start, count);
            if (cost < bestCost) {
                bestCost = cost;
                bestPos = splitPos;
                bestAxis = axis;
            }
        }
    }

    bestSplit.axis = bestAxis;
    bestSplit.pos = bestPos;
    bestSplit.cost = bestCost;

    return bestSplit;
}