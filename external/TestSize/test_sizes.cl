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
typedef struct {
    float radius;
    float _padding1[3];  // Padding to align next Vec3
    Vec3 pos;
    Vec3 emi;
    Vec3 color;
} GPUSphere;

// Match CPU-side GPUSquare exactly
typedef struct {
    Vec3 pos;
    Vec3 u_vec;
    Vec3 v_vec;
    Vec3 normal;
    Vec3 emi;
    Vec3 color;
} GPUSquare;

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
        printf("sizeof(int): %zu bytes\n", sizeof(int));
        printf("sizeof(float): %zu bytes\n", sizeof(float));
        printf("sizeof(float3): %zu bytes\n", sizeof(float3));
        printf("sizeof(Vec3): %zu bytes\n", sizeof(Vec3));
        printf("sizeof(GPUSphere): %zu bytes\n", sizeof(GPUSphere));
        printf("sizeof(GPUSquare): %zu bytes\n", sizeof(GPUSquare));
        printf("sizeof(GPUShape): %zu bytes\n", sizeof(GPUShape));
    }
}
