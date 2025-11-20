// Test kernel to print struct sizes

// Match CPU-side Vec3 with padding to align to 16 bytes (same as float4)
typedef struct {
    float x, y, z;
    float _padding;
} Vec3;

// GPU Camera structure matching CPU side exactly (64 bytes total)
typedef struct {
    Vec3 origin;      // Camera position (16 bytes)
    Vec3 target;      // What the camera is looking at (16 bytes)  
    Vec3 up;          // Up vector (16 bytes)
    float fov;        // Field of view in degrees (4 bytes)
    float _padding[3]; // Padding for alignment (12 bytes)
} GPUCamera;


// Match CPU-side GPUSphere exactly
typedef struct __attribute__((aligned(16))) {
    float radius;           // 4 bytes (offset 0)
    float _padding1[3];     // 12 bytes (offset 4)
    Vec3 pos;               // 16 bytes (offset 16)
	int materialIndex;      // 4 bytes (offset 32)
	float _padding2[3];     // 12 bytes (offset 36)
} GPUSphere;  // Total: 48 bytes

// Match CPU-side GPUSquare exactly
typedef struct __attribute__((aligned(16))) {
    Vec3 pos;               // 16 bytes (offset 0)
    Vec3 u_vec;             // 16 bytes (offset 16)
    Vec3 v_vec;             // 16 bytes (offset 32)
    Vec3 normal;            // 16 bytes (offset 48)
	int materialIndex;      // 4 bytes (offset 64)
	float _padding[3];      // 12 bytes (offset 68)
} GPUSquare;  // Total: 80 bytes

typedef struct __attribute__((aligned(16))) {
	Vec3 v0;                // 16 bytes (offset 0)
	Vec3 v1;                // 16 bytes (offset 16)
	Vec3 v2;                // 16 bytes (offset 32)
	int materialIndex;      // 4 bytes (offset 48)
	float _padding[3];      // 12 bytes (offset 52)
} GPUTriangle;  // Total: 64 bytes

typedef struct __attribute__((aligned(16))) {
	Vec3 ambient;              // 16 bytes (offset 0)
	Vec3 diffuse;              // 16 bytes (offset 16)
	Vec3 specular;             // 16 bytes (offset 32)
	
	float shininess;           // 4 bytes (offset 48)
	float index_medium;        // 4 bytes (offset 52)
	float transparency;        // 4 bytes (offset 56)
	float texture_scale_x;     // 4 bytes (offset 60)
	
	float texture_scale_y;     // 4 bytes (offset 64)
	int emissive;              // 4 bytes (offset 68)
	float _padding1[2];        // 8 bytes (offset 72)
	
	Vec3 light_color;          // 16 bytes (offset 80)
	
	float light_intensity;     // 4 bytes (offset 96)
	int has_texture;           // 4 bytes (offset 100)
	int has_normal_map;        // 4 bytes (offset 104)
	int texture_width;         // 4 bytes (offset 108)
	
	int texture_height;        // 4 bytes (offset 112)
	int texture_offset;        // 4 bytes (offset 116)
	int normal_map_offset;     // 4 bytes (offset 120)
	int material_id;           // 4 bytes (offset 124)
} GPUMaterial;  // Total: 128 bytes

// Match CPU-side GPUShape exactly
typedef struct __attribute__((aligned(16))) {
    int type;           // form type(SPHERE=1, PLANE=2, SQUARE=3, etc.)
    float _padding[3];  // Padding after int to align union to 16 bytes
    union {
        GPUSphere sphere;
        GPUSquare square;
    } data;
} GPUShape;

__kernel void test_sizes()
{
    if (get_global_id(0) == 0) {
        printf("=== OpenCL Side Structure Sizes ===\n");
        printf("sizeof(int): %d bytes\n", (int)sizeof(int));
        printf("sizeof(float): %d bytes\n", (int)sizeof(float));
        printf("sizeof(float3): %d bytes\n", (int)sizeof(float3));
        printf("sizeof(Vec3): %d bytes\n", (int)sizeof(Vec3));
        printf("sizeof(GPUSphere): %d bytes (expected: 48)\n", (int)sizeof(GPUSphere));
        printf("sizeof(GPUSquare): %d bytes (expected: 80)\n", (int)sizeof(GPUSquare));
        printf("sizeof(GPUTriangle): %d bytes (expected: 64)\n", (int)sizeof(GPUTriangle));
        printf("sizeof(GPUMaterial): %d bytes (expected: 128)\n", (int)sizeof(GPUMaterial));
        printf("sizeof(GPUShape): %d bytes\n", (int)sizeof(GPUShape));
    }
}
