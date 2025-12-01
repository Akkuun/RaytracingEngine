#include "bvh.h"
#include <algorithm>

struct CentroidData
{
    Triangle *tri;
    vec3 centroid;
};

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

    // Leaf stop conditions (numtriangles <=4 <=> moore than the last triangle)
    if (depth >= maxDepth || numTriangles <= 4)
    {
        node->triangles.assign(start, end);
        return node;
    }

    // Start SAH computation
    // SAH = Surface Area Heuristic, search for the best split that minimize the cost function instead of just picking the median
    std::vector<CentroidData> centroids;
    centroids.reserve(numTriangles);

    for (auto it = start; it != end; ++it)
    {
        vec3 c = (it->getV0() + it->getV1() + it->getV2()) / 3.0f;
        centroids.push_back({&(*it), c});
    }

    float bestCost = std::numeric_limits<float>::infinity(); // initialize to +inf
    int bestAxis = -1;                                       // no axis yet
    int bestSplit = -1;

    AABB parentBB = node->boundingBox;

    // SAH constants
    const float C_trav = 1.0f;
    const float C_isect = 1.0f;

    // we test on each axes
    for (int axis = 0; axis < 3; axis++)
    {

        // Sort by centroid
        std::sort(centroids.begin(), centroids.end(),
                  [axis](const CentroidData &a, const CentroidData &b)
                  {
                      return a.centroid[axis] < b.centroid[axis];
                  });

        // Precompute left and right BBoxes
        std::vector<AABB> leftBB(numTriangles);
        std::vector<AABB> rightBB(numTriangles);

        AABB bb;

        // prefix (left boxes)
        for (int i = 0; i < numTriangles; i++)
        {
            bb.GrowToInclude(*centroids[i].tri);
            leftBB[i] = bb;
        }

        // suffix (right boxes)
        bb = AABB();
        for (int i = numTriangles - 1; i >= 0; i--)
        {
            bb.GrowToInclude(*centroids[i].tri);
            rightBB[i] = bb;
        }

        // if we found a better splitScore, we store it and we continue until getting the best one (at the end)
        for (int i = 1; i < numTriangles; i++)
        {
            float SAleft = leftBB[i - 1].SurfaceArea();
            float SAright = rightBB[i].SurfaceArea();
            float SAparent = parentBB.SurfaceArea();

            float cost =
                C_trav +
                C_isect * (SAleft / SAparent) * i +
                C_isect * (SAright / SAparent) * (numTriangles - i);

            if (cost < bestCost)
            {
                bestCost = cost;
                bestAxis = axis;
                bestSplit = i;
            }
        }
    }

    // if no valid axes found -> no more triangle to split -> become leaf
    if (bestAxis == -1)
    {
        node->triangles.assign(start, end);
        return node;
    }

    // Reorder original triangles according to the best axis
    std::sort(start, end, [bestAxis](const Triangle &a, const Triangle &b)
              {
                  vec3 ca = (a.getV0() + a.getV1() + a.getV2()) / 3.0f;
                  vec3 cb = (b.getV0() + b.getV1() + b.getV2()) / 3.0f;
                  return ca[bestAxis] < cb[bestAxis]; });

    auto mid = start + bestSplit;

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
        // Internal node: process children first to get their indices
        int leftChildIdx = currentNodeIdx;

        // Process left child
        flattenBVH(node->childA, outNodes, outTriangles, currentNodeIdx);

        // Process right child (will be at leftChildIdx + 1 if tree is complete,
        // but we use currentNodeIdx to handle any case)
        flattenBVH(node->childB, outNodes, outTriangles, currentNodeIdx);

        // Fill the node with internal node data
        // childIndex points to left child, right child is always at childIndex + 1
        outNodes[myIndex] = node->toGPU(leftChildIdx, -1, 0);
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
