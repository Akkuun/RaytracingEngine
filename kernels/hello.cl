__kernel void hello(__global int* out)
{
    int a = 3;
    int b = 3;
    out[0] = a + b;
}



// AABB-Ray intersection test using slab method
// Returns true if ray intersects AABB, and sets tMin/tMax to entry/exit distances
// invDir must be precomputed as (1/ray.dir.x, 1/ray.dir.y, 1/ray.dir.z) ONCE per ray
inline __attribute__((always_inline)) bool intersect_aabb(
    __global const AABBGPU* restrict aabb,
    const float3 rayOrigin,
    const float3 invDir,
    float* restrict tMin,
    float* restrict tMax)
{
    float3 aabbMin = vec3_to_float3(aabb->minPoint);
    float3 aabbMax = vec3_to_float3(aabb->maxPoint);
    
    float3 t0 = (aabbMin - rayOrigin) * invDir;
    float3 t1 = (aabbMax - rayOrigin) * invDir;
    
    // Handle negative directions
    float3 tmin = fmin(t0, t1);
    float3 tmax = fmax(t0, t1);
    
    *tMin = fmax(fmax(tmin.x, tmin.y), tmin.z);
    *tMax = fmin(fmin(tmax.x, tmax.y), tmax.z);
    
    return *tMax >= *tMin && *tMax >= 0.0f;
}

// Intersect ray with a single triangle from BVH triangle buffer
inline __attribute__((always_inline)) struct Intersection intersect_bvh_triangle(
    __global const GPUTriangle* restrict triangle,
    const struct Ray* restrict ray,
    float* restrict t)
{
    struct Intersection result;
    result.t = -1.0f;
    
    float3 v0 = vec3_to_float3(triangle->v0);
    float3 v1 = vec3_to_float3(triangle->v1);
    float3 v2 = vec3_to_float3(triangle->v2);
    
    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;
    float3 h = cross(ray->dir, edge2);
    float a = dot(edge1, h);
    
    if (a < EPSILON) return result; // Backface culling
    
    float f = 1.0f / a;
    float3 s = ray->origin - v0;
    float u = f * dot(s, h);
    
    if (u < 0.0f || u > 1.0f) return result;
    
    float3 q = cross(s, edge1);
    float v = f * dot(ray->dir, q);
    
    if (v < 0.0f || u + v > 1.0f) return result;
    
    *t = f * dot(edge2, q);
    
    if (*t < EPSILON) return result;
    
    result.t = *t;
    result.hitpoint = ray->origin + ray->dir * (*t);
    result.normal = normalize(cross(edge1, edge2));
    result.uv = (float2)(u, v);
    
    return result;
}

// BVH traversal using iterative stack-based approach
// Returns the closest intersection with triangles in the BVH
// invDir must be precomputed ONCE per ray before calling this function
inline __attribute__((always_inline)) struct Intersection traverse_bvh(
    __global const GPUBVHNode* restrict nodes,
    __global const GPUTriangle* restrict triangles,
    int rootNodeIndex,
    const struct Ray* restrict ray,
    const float3 invDir,
    int* restrict hitMaterialIndex)
{
    struct Intersection closestHit;
    closestHit.t = -1.0f;
    *hitMaterialIndex = -1;
    
    float closestT = 1e30f;
    
    // Stack for iterative traversal (max depth typically 32-64 for BVH)
    int stack[64];
    int stackPtr = 0;
    
    stack[stackPtr++] = rootNodeIndex;
    
    while (stackPtr > 0) {
        int nodeIdx = stack[--stackPtr];
        __global const GPUBVHNode* node = &nodes[nodeIdx];
        
        // Test AABB intersection
        float tMin, tMax;
        if (!intersect_aabb(&node->boundingBox, ray->origin, invDir, &tMin, &tMax) || tMin > closestT)
            continue;
        
        if (node->childIndex == -1) {
            // Leaf node: test all triangles
            int start = node->triangleStartIdx;
            int end = start + node->triangleCount;
            
            for (int i = start; i < end; i++) {
                float t;
                struct Intersection hit = intersect_bvh_triangle(&triangles[i], ray, &t);
                
                if (hit.t > 0.0f && hit.t < closestT) {
                    closestT = hit.t;
                    closestHit = hit;
                    *hitMaterialIndex = triangles[i].materialIndex;
                }
            }
        } else {
            // Internal node: push children onto stack
            // Push right child first so left child is processed first
            stack[stackPtr++] = node->childIndex + 1;
            stack[stackPtr++] = node->childIndex;
        }
    }
    
    return closestHit;
}

// Traverse all BVHs and find the closest intersection
// Precomputes invDir once for all BVH traversals (avoids redundant divisions)
inline __attribute__((always_inline)) struct Intersection compute_bvh_intersection(
    __global const GPUBVH* restrict bvhHeaders,
    __global const GPUBVHNode* restrict bvhNodes,
    __global const GPUTriangle* restrict bvhTriangles,
    int numBVH,
    const struct Ray* restrict ray,
    int* restrict hitMaterialIndex)
{
    struct Intersection closestHit;
    closestHit.t = -1.0f;
    *hitMaterialIndex = -1;
    
    // Precompute inverse direction ONCE for all BVH traversals
    // This eliminates redundant divisions (divisions cost ~20x more than multiplications)
    float3 invDir = (float3)(1.0f / ray->dir.x, 1.0f / ray->dir.y, 1.0f / ray->dir.z);
    
    float closestT = 1e20f;
    int nodeOffset = 0;
    int triangleOffset = 0;
    
    for (int bvhIdx = 0; bvhIdx < numBVH; bvhIdx++) {
        __global const GPUBVH* bvh = &bvhHeaders[bvhIdx];
        
        if (bvh->numNodes == 0) {
            continue;
        }
        
        int matIdx;
        struct Intersection hit = traverse_bvh(
            bvhNodes + nodeOffset,
            bvhTriangles + triangleOffset,
            bvh->rootNodeIndex,
            ray,
            invDir,
            &matIdx);
        
        if (hit.t > EPSILON && hit.t < closestT) {
            closestT = hit.t;
            closestHit = hit;
            *hitMaterialIndex = matIdx;
        }
        
        nodeOffset += bvh->numNodes;
        triangleOffset += bvh->numTriangles;
    }
    
    return closestHit;
}