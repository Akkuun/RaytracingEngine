typedef struct {
    float center[3];
    float radius;
    float material[4];
} GPUSphere;

typedef struct {
    int type;           //  form type(SPHERE=1, PLANE=2, etc.)
    union {
        GPUSphere sphere;
        // GPUPlane plane;    // later
        // GPUTriangle tri;   // later
    } data;
} GPUShape;


float intersect_sphere(float3 ray_origin, float3 ray_dir, __global GPUShape* shape) {
    // Access sphere data directly to avoid address space mismatch
    float3 center = (float3)(shape->data.sphere.center[0], shape->data.sphere.center[1], shape->data.sphere.center[2]);
    
    return center.x;

}

/*PIPELINE DE CONVERSION
void RenderEngine::updateGPUBuffers() {
    gpu_shapes.clear();
    
    for (auto* shape : shapes) {
        GPUShape gpu_shape;
        gpu_shape.type = shape->getType();
        
        switch (shape->getType()) {
            case SPHERE: {
                Sphere* sphere = static_cast<Sphere*>(shape);
                gpu_shape.data.sphere = sphere->toGPU();
                break;
            }
            // Autres formes...
        }
        
        gpu_shapes.push_back(gpu_shape);
    }
    
    // Upload vers GPU
    queue.enqueueWriteBuffer(shapes_buffer, CL_TRUE, 0, 
                            gpu_shapes.size() * sizeof(GPUShape), 
                            gpu_shapes.data());
}
*/
