#pragma once

// POD structure compatible with OpenCL float3
// OpenCL aligns float3 to 16 bytes (same as float4)
struct Vec3 {
    float x, y, z;
    float _padding;  // Padding to match OpenCL float3 (16 bytes)
};

enum ShapeType
{
    UNDEFINED = 0,  // explicit values
    SPHERE = 1,
    SQUARE = 2,
    TRIANGLE = 3,
    MESH = 4
};

struct GPUSphere
{
    float radius;
    float _padding1[3];  // Padding to align next Vec3
    Vec3 pos;
    Vec3 emi;
    Vec3 color;
};

struct GPUSquare
{
    Vec3 pos;
    Vec3 u_vec;
    Vec3 v_vec;
    Vec3 normal;
    Vec3 emi;
    Vec3 color;
};

struct GPUTriangle
{
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 emi;
    Vec3 color;
};

// Struct GPU-compatible (for the kernel)
typedef struct __attribute__((aligned(16))) {
    int type;           //  form type(SPHERE=1, PLANE=2, SQUARE=3, etc.)
    float _padding[3];  // Padding after int to align union to 16 bytes
    union {
        GPUSphere sphere;
        GPUSquare square;
        GPUTriangle triangle;
    } data;
} GPUShape;



enum TextureType {
    Texture_None = 0, // explicit values
    Texture_Image = 1
};

struct GPUMaterial {
    Vec3 diffuse;
};
