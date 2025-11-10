#pragma once

enum ShapeType
{
    UNDEFINED,
    SPHERE,
    PLANE,
    SQUARE,
};

// Struct GPU-compatible (for the kernel)
struct GPUSphere
{
    float radius;
	vec3 pos;
	vec3 emi;
	vec3 color;
};

struct GPUSquare
{
    vec3 pos;
    vec3 u_vec;
    vec3 v_vec;
    vec3 normal;
    vec3 color;
};
