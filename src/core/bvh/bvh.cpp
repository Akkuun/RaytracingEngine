#include "bvh.h"
#include <algorithm>

void BVH::build(const Mesh &mesh)
{
    triangles = mesh.getTriangles();
    associatedMeshID = mesh.getID();
    if (triangles.empty())
        return;
    root = buildRecursive(triangles.begin(), triangles.end()); // setup the division
    boundingBox = root->boundingBox;
}

bvhNode *BVH::buildRecursive(std::vector<Triangle>::iterator start,
                             std::vector<Triangle>::iterator end,
                             int depth)
{
    // each time we call this function, we execute the same operation for each node :
    // 1 - compute the bounding box of the node and store it
    // 2 - check the stop conditions (max depth or min number of triangles)
    // 3 - if not leaf, compute the best split using SAH and create child nodes (before it was median split, a naive solution to have balanced tree)
    // based on best axis choosen, reorder the triangles and call recursively the function for each child
    size_t numTriangles = end - start;

    auto node = new bvhNode();

    // Compute bbox of node
    for (auto it = start; it != end; ++it)
        node->boundingBox.GrowToInclude(*it);

    // Leaf stop conditions - use larger leaf size for better GPU performance
    // GPU prefers fewer but larger leaf nodes (reduces divergence)
    const int MIN_LEAF_TRIANGLES = 8; // Increased from 4 for better GPU performance
    if (depth >= maxDepth || numTriangles <= MIN_LEAF_TRIANGLES)
    {
        node->triangles.assign(start, end);
        return node;
    }

    // Binned SAH for faster build times with large meshes
    // Instead of testing every split, we use bins to approximate the best split
    const int NUM_BINS = 16;

    float bestCost = std::numeric_limits<float>::infinity();
    int bestAxis = -1;
    float bestSplitPos = 0.0f;

    AABB parentBB = node->boundingBox;
    float SAparent = parentBB.SurfaceArea();

    if (SAparent < 1e-8f)
    {
        // Degenerate case - just make a leaf
        node->triangles.assign(start, end);
        return node;
    }

    // SAH constants
    const float C_trav = 1.0f;
    const float C_isect = 1.0f;

    // Compute centroids once
    std::vector<vec3> centroids;
    centroids.reserve(numTriangles);
    for (auto it = start; it != end; ++it)
    {
        centroids.push_back((it->getV0() + it->getV1() + it->getV2()) / 3.0f);
    }

    // Test each axis with binned SAH
    for (int axis = 0; axis < 3; axis++)
    {
        float minC = parentBB.minPoint[axis];
        float maxC = parentBB.maxPoint[axis];
        float range = maxC - minC;

        if (range < 1e-6f)
            continue; // Skip degenerate axes

        // Initialize bins
        struct Bin
        {
            AABB bounds;
            int count = 0;
        };
        std::vector<Bin> bins(NUM_BINS);

        // Assign triangles to bins
        for (size_t i = 0; i < numTriangles; i++)
        {
            float c = centroids[i][axis];
            int binIdx = std::min(NUM_BINS - 1, (int)((c - minC) / range * NUM_BINS));
            bins[binIdx].bounds.GrowToInclude(*(start + i));
            bins[binIdx].count++;
        }

        // Compute prefix and suffix areas/counts
        std::vector<float> leftArea(NUM_BINS);
        std::vector<int> leftCount(NUM_BINS);
        std::vector<float> rightArea(NUM_BINS);
        std::vector<int> rightCount(NUM_BINS);

        AABB leftBB, rightBB;
        int countL = 0, countR = 0;

        for (int i = 0; i < NUM_BINS; i++)
        {
            if (bins[i].count > 0)
            {
                leftBB.GrowToInclude(bins[i].bounds.minPoint);
                leftBB.GrowToInclude(bins[i].bounds.maxPoint);
            }
            countL += bins[i].count;
            leftArea[i] = leftBB.SurfaceArea();
            leftCount[i] = countL;
        }

        for (int i = NUM_BINS - 1; i >= 0; i--)
        {
            if (bins[i].count > 0)
            {
                rightBB.GrowToInclude(bins[i].bounds.minPoint);
                rightBB.GrowToInclude(bins[i].bounds.maxPoint);
            }
            countR += bins[i].count;
            rightArea[i] = rightBB.SurfaceArea();
            rightCount[i] = countR;
        }

        // Find best split position for this axis
        for (int i = 0; i < NUM_BINS - 1; i++)
        {
            if (leftCount[i] == 0 || rightCount[i + 1] == 0)
                continue;

            float cost = C_trav +
                         C_isect * (leftArea[i] / SAparent) * leftCount[i] +
                         C_isect * (rightArea[i + 1] / SAparent) * rightCount[i + 1];

            if (cost < bestCost)
            {
                bestCost = cost;
                bestAxis = axis;
                bestSplitPos = minC + (i + 1) * range / NUM_BINS;
            }
        }
    }

    // Check if splitting is worth it (cost should be less than not splitting)
    float leafCost = C_isect * numTriangles;
    if (bestAxis == -1 || bestCost >= leafCost)
    {
        node->triangles.assign(start, end);
        return node;
    }

    // Partition triangles based on best split
    auto mid = std::partition(start, end, [bestAxis, bestSplitPos](const Triangle &t)
                              {
        vec3 c = (t.getV0() + t.getV1() + t.getV2()) / 3.0f;
        return c[bestAxis] < bestSplitPos; });

    // Handle edge case where partition puts everything on one side
    if (mid == start || mid == end)
    {
        // Fall back to median split
        mid = start + numTriangles / 2;
    }

    node->childA = buildRecursive(start, mid, depth + 1);
    node->childB = buildRecursive(mid, end, depth + 1);

    return node;
}

GPUBVH BVH::toGPU(std::vector<GPUBVHNode> &outNodes, std::vector<GPUTriangle> &outTriangles) const
{
    GPUBVH gpuBVH;
    gpuBVH.rootNodeIndex = 0;
    gpuBVH.meshID = associatedMeshID;

    if (!root)
    {
        gpuBVH.numNodes = 0;
        gpuBVH.numTriangles = 0;
        return gpuBVH;
    }

    outNodes.clear();
    outTriangles.clear();

    int currentNodeIdx = 0;
    flattenBVH(root, outNodes, outTriangles, currentNodeIdx);

    gpuBVH.numNodes = static_cast<int>(outNodes.size());
    gpuBVH.numTriangles = static_cast<int>(outTriangles.size());

    return gpuBVH;
}

void BVH::flattenBVH(bvhNode *node, std::vector<GPUBVHNode> &outNodes,
                     std::vector<GPUTriangle> &outTriangles, int &currentNodeIdx) const
{
    if (!node)
        return;

    int myIndex = currentNodeIdx++;

    // Reserve space for this node (we'll fill it properly after processing children)
    outNodes.push_back(GPUBVHNode{});

    if (node->isLeaf())
    {
        // Leaf node: store triangles
        int triStartIdx = static_cast<int>(outTriangles.size());
        int triCount = static_cast<int>(node->triangles.size());

        // Add triangles to the output array
        for (const Triangle &tri : node->triangles)
        {
            GPUTriangle gpuTri;
            vec3 v0 = tri.getV0();
            vec3 v1 = tri.getV1();
            vec3 v2 = tri.getV2();

            gpuTri.v0 = {v0.x, v0.y, v0.z, 0.0f};
            gpuTri.v1 = {v1.x, v1.y, v1.z, 0.0f};
            gpuTri.v2 = {v2.x, v2.y, v2.z, 0.0f};
            gpuTri.materialIndex = (tri.getMaterial() != nullptr) ? tri.getMaterial()->getMaterialId() : -1;
            gpuTri._padding[0] = 0.0f;
            gpuTri._padding[1] = 0.0f;
            gpuTri._padding[2] = 0.0f;

            outTriangles.push_back(gpuTri);
        }

        // Fill the node with leaf data
        outNodes[myIndex] = node->toGPU(-1, triStartIdx, triCount);
    }
    else
    {
        // Internal node: we need to ensure children are placed consecutively
        // Left child will be at currentNodeIdx, right child at currentNodeIdx + 1
        // BUT we need to process them in a specific order to achieve this

        int leftChildIdx = currentNodeIdx;

        // Reserve space for BOTH children first to ensure they're consecutive
        // We'll use a different approach: store left child index and process breadth-first style
        // Actually, for depth-first with consecutive children, we need to know subtree sizes

        // Simpler approach: just record where the right child ends up
        // Process left child first
        flattenBVH(node->childA, outNodes, outTriangles, currentNodeIdx);

        // Right child index is wherever we are now
        int rightChildIdx = currentNodeIdx;
        flattenBVH(node->childB, outNodes, outTriangles, currentNodeIdx);

        // Store the node with left child index
        // The kernel needs to know both children - let's store left and derive right
        // Since right is NOT at leftChildIdx + 1, we need to change our approach

        // SOLUTION: Use triangleStartIdx to store rightChildIdx for internal nodes
        // childIndex = leftChildIdx, triangleStartIdx = rightChildIdx, triangleCount = 0
        GPUBVHNode gpuNode = node->toGPU(leftChildIdx, rightChildIdx, 0);
        outNodes[myIndex] = gpuNode;
    }
}
// debug function to print the BVH structure in terminal
void BVH::printRecursive(bvhNode *node, int depth) const
{
    if (!node)
        return;

    // indentation
    for (int i = 0; i < depth; i++)
        std::cout << "  ";

    // node info
    std::cout << "- Node (depth " << depth << ")";
    // if leaf
    if (!node->childA && !node->childB)
    {
        std::cout << " [LEAF] triangles=" << node->triangles.size() << "\n";
    }
    else // if internal node
    {
        std::cout << " [INTERNAL]\n";
    }

    // display children
    if (node->childA)
        printRecursive(node->childA, depth + 1);
    if (node->childB)
        printRecursive(node->childB, depth + 1);
}
