// Test kernel to print struct sizes
__kernel void test_sizes()
{
    if (get_global_id(0) == 0) {
        printf("=== OpenCL Side Structure Sizes ===\n");
        printf("sizeof(int): %zu bytes\n", sizeof(int));
        printf("sizeof(float): %zu bytes\n", sizeof(float));
        printf("sizeof(float3): %zu bytes\n", sizeof(float3));
        
        // Define structures like in testKernel.cl
        struct Sphere {
            float radius;
            float3 pos;
            float3 emi;
            float3 color;
        };
        
        struct Square {
            float3 pos;
            float3 u_vec;
            float3 v_vec;
            float3 normal;
            float3 color;
        };
        
        struct Shape {
            int type;
            union {
                struct Sphere sphere;
                struct Square square;
            } data;
        };
        
        printf("sizeof(Sphere): %zu bytes\n", sizeof(struct Sphere));
        printf("sizeof(Square): %zu bytes\n", sizeof(struct Square));
        printf("sizeof(Shape): %zu bytes\n", sizeof(struct Shape));
    }
}
