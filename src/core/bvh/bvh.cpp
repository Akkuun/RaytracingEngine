#include "bvh.h"
#include <algorithm>


struct CentroidData
{
    Triangle *tri;
    vec3 centroid;
};

void BVH::build(const Mesh& mesh) {
    triangles = mesh.getTriangles();
    associatedMeshID = mesh.getID();
    if (triangles.empty()) return;
    root = buildRecursive(triangles.begin(), triangles.end()); // setup the division
    boundingBox = root->boundingBox;
}

bvhNode *BVH::buildRecursive(std::vector<Triangle>::iterator start,
                             std::vector<Triangle>::iterator end,
                             int depth)
{
    size_t numTriangles = end - start;

    auto node = new bvhNode();

    // Compute bbox of node
    for (auto it = start; it != end; ++it)
        node->boundingBox.GrowToInclude(*it);

    // Leaf stop conditions
    if (depth >= maxDepth || numTriangles <= 4)
    {
        node->triangles.assign(start, end);
        return node;
    }

    // ---- BUILD CENTROID ARRAY ----
    std::vector<CentroidData> centroids;
    centroids.reserve(numTriangles);

    for (auto it = start; it != end; ++it)
    {
        vec3 c = (it->getV0() + it->getV1() + it->getV2()) / 3.0f;
        centroids.push_back({&(*it), c});
    }

    float bestCost = std::numeric_limits<float>::infinity();
    int bestAxis = -1;
    int bestSplit = -1;

    AABB parentBB = node->boundingBox;

    // SAH constants
    const float C_trav = 1.0f;
    const float C_isect = 1.0f;

    // ---- TEST ALL 3 AXES ----
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

        // ---- Evaluate all split positions ----
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

    // If no valid SAH split → leaf
    if (bestAxis == -1)
    {
        node->triangles.assign(start, end);
        return node;
    }

    // ---- APPLY THE BEST SPLIT ----
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

GPUBVH BVH::toGPU() const {
    GPUBVH gpuBVH;
    // TODO serialize the BVH data into gpuBVH
    return gpuBVH;
}

void BVH::printRecursive(bvhNode *node, int depth) const
{
    if (!node)
        return;

    // indentation
    for (int i = 0; i < depth; i++)
        std::cout << "  ";

    // affichage node
    std::cout << "- Node (depth " << depth << ")";

    if (!node->childA && !node->childB)
    {
        std::cout << " [LEAF] triangles=" << node->triangles.size() << "\n";
    }
    else
    {
        std::cout << " [INTERNAL]\n";
    }

    // display children
    if (node->childA)
        printRecursive(node->childA, depth + 1);
    if (node->childB)
        printRecursive(node->childB, depth + 1);
}
