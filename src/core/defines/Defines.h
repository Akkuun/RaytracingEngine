#pragma once

// POD structure compatible with OpenCL float3
// OpenCL aligns float3 to 16 bytes (same as float4)
struct Vec3
{
    float x, y, z;
    float _padding; // Padding to match OpenCL float3 (16 bytes)
};

enum ShapeType
{
    UNDEFINED = 0, // explicit values
    SPHERE = 1,
    SQUARE = 2,
    TRIANGLE = 3,
    MESH = 4
};

struct __attribute__((aligned(16))) GPUSphere
{
    float radius;       // 4 bytes (offset 0)
    float _padding1[3]; // 12 bytes (offset 4)
    Vec3 pos;           // 16 bytes (offset 16)
    int materialIndex;  // 4 bytes (offset 32)
    float _padding2[3]; // 12 bytes (offset 36)
}; // Total: 48 bytes

struct __attribute__((aligned(16))) GPUSquare
{
    Vec3 pos;          // 16 bytes (offset 0)
    Vec3 u_vec;        // 16 bytes (offset 16)
    Vec3 v_vec;        // 16 bytes (offset 32)
    Vec3 normal;       // 16 bytes (offset 48)
    int materialIndex; // 4 bytes (offset 64)
    float _padding[3]; // 12 bytes (offset 68)
}; // Total: 80 bytes

struct __attribute__((aligned(16))) GPUTriangle
{
    Vec3 v0;           // 16 bytes (offset 0)
    Vec3 v1;           // 16 bytes (offset 16)
    Vec3 v2;           // 16 bytes (offset 32)
    int materialIndex; // 4 bytes (offset 48)
    float _padding[3]; // 12 bytes (offset 52)
}; // Total: 64 bytes

// Struct GPU-compatible (for the kernel)
typedef struct __attribute__((aligned(16)))
{
    int type;          //  form type(SPHERE=1, PLANE=2, SQUARE=3, etc.)
    float _padding[3]; // Padding after int to align union to 16 bytes
    union
    {
        GPUSphere sphere;
        GPUSquare square;
        GPUTriangle triangle;
    } data;
} GPUShape;

enum TextureType
{
    Texture_None = 0, // explicit values
    Texture_Image = 1
};

struct __attribute__((aligned(16))) GPUMaterial
{
    Vec3 ambient;  // 16 bytes (offset 0)
    Vec3 diffuse;  // 16 bytes (offset 16)
    Vec3 specular; // 16 bytes (offset 32)

    float shininess;       // 4 bytes (offset 48)
    float index_medium;    // 4 bytes (offset 52)
    float transparency;    // 4 bytes (offset 56)
    float texture_scale_x; // 4 bytes (offset 60)

    float texture_scale_y; // 4 bytes (offset 64)
    int emissive;          // 4 bytes (offset 68)
    float metalness;       // 4 bytes (offset 72)
    float _padding1;       // 4 bytes (offset 76)

    Vec3 light_color; // 16 bytes (offset 80)

    float light_intensity; // 4 bytes (offset 96)
    int has_texture;       // 4 bytes (offset 100)
    int texture_width;     // 4 bytes (offset 104)
    int texture_height;    // 4 bytes (offset 108)

    int texture_offset;    // 4 bytes (offset 112)
    int has_normal_map;    // 4 bytes (offset 116)
    int normal_map_width;  // 4 bytes (offset 120)
    int normal_map_height; // 4 bytes (offset 124)

    int normal_map_offset; // 4 bytes (offset 128)
    int has_metal_map;     // 4 bytes (offset 132)
    int metal_map_width;   // 4 bytes (offset 136)
    int metal_map_height;  // 4 bytes (offset 140)

    int metal_map_offset;    // 4 bytes (offset 144)
    int has_emissive_map;    // 4 bytes (offset 148)
    int emissive_map_width;  // 4 bytes (offset 152)
    int emissive_map_height; // 4 bytes (offset 156)

    int emissive_map_offset; // 4 bytes (offset 160)
    int material_id;         // 4 bytes (offset 164)
    int _padding2[2];        // 8 bytes (offset 168) - padding for 16-byte alignment
}; // Total: 176 bytes

// GPU-compatible AABB structure
struct __attribute__((aligned(16))) AABBGPU
{
    Vec3 minPoint; // 16 bytes (offset 0)
    Vec3 maxPoint; // 16 bytes (offset 16)
}; // Total: 32 bytes

// GPU-compatible BVH Node structure
// Using a flattened array representation for GPU traversal
// If childIndex == -1, it's a leaf node and triangleStartIdx/triangleCount are valid
// Otherwise, childIndex points to the left child (right child is childIndex + 1)
struct __attribute__((aligned(16))) GPUBVHNode
{
    AABBGPU boundingBox;  // 32 bytes (offset 0)
    int childIndex;       // 4 bytes (offset 32) - index of left child (-1 if leaf)
    int triangleStartIdx; // 4 bytes (offset 36) - start index in triangle array (for leaves)
    int triangleCount;    // 4 bytes (offset 40) - number of triangles (for leaves)
    int _padding;         // 4 bytes (offset 44) - padding for 16-byte alignment
}; // Total: 48 bytes

// GPU-compatible BVH structure
// Contains flattened arrays of nodes and triangles
struct __attribute__((aligned(16))) GPUBVH
{
    int numNodes;      // 4 bytes (offset 0)
    int numTriangles;  // 4 bytes (offset 4)
    int rootNodeIndex; // 4 bytes (offset 8) - always 0 for single BVH
    int meshID;        // 4 bytes (offset 12) - associated mesh ID
}; // Total: 16 bytes (header only, nodes and triangles are separate buffers)
