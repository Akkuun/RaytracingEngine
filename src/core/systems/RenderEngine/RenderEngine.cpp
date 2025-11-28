#include "RenderEngine.h"
#include <iostream>
#include <cstring>
#include "../../defines/Defines.h"
#include "../../shapes/Triangle.h"
#include "../../shapes/Mesh.h"

RenderEngine::RenderEngine()
{
    kernelManager = &KernelManager::getInstance();
    deviceManager = DeviceManager::getInstance();
}

void RenderEngine::setupBuffers(int width, int height)
{
    // Only recreate buffers if size changed
    if (currentWidth != width || currentHeight != height)
    {
        size_t imageSize = 3 * sizeof(float) * width * height; //  float * 3 for RGB * width * height

        // Resize image data vector
        imageData.resize(width * height * 3);

        // Create or recreate buffers
        cl::Context context = deviceManager->getContext();
        outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, imageSize);
        accumBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, imageSize);

        // Reset frame count when resolution changes
        frameCount = 0;
        currentWidth = width;
        currentHeight = height;

        // Initialize accumulation buffer to zero
        cl::CommandQueue queue = deviceManager->getCommandQueue();
        std::vector<float> zeros(width * height * 3, 0.0f);
        queue.enqueueWriteBuffer(accumBuffer, CL_TRUE, 0, imageSize, zeros.data());
    }

    // Setup shapes buffer only if it's dirty (shapes changed) or first time
    if (shapesBufferDirty)
    {
        setupShapesBuffer();
        shapesBufferDirty = false;
    }

    // Camera parameters are now passed directly to kernel
    // Reset frame count if camera changed
    if (cameraBufferDirty)
    {
        frameCount = 0; // Reset accumulation when camera changes
        cameraBufferDirty = false;
    }

    // Setup Material buffer only if it's dirty (one of the material has been modified) or first time
    if (materialBufferDirty)
    {
        setupMaterialBuffer();
        materialBufferDirty = false;
    }

    if( bvhBufferDirty )
    {
        setupBVHBuffer();
        bvhBufferDirty = false;
    }
}

void RenderEngine::render(int width, int height)
{
    try
    {
        setupBuffers(width, height);

        cl::Kernel kernel = kernelManager->getKernel("render_kernel");
        cl::CommandQueue queue = deviceManager->getCommandQueue();

        // Get camera parameters and create GPU buffer
        GPUCamera gpu_camera = Camera::getInstance().toGPU();
        cl::Context context = deviceManager->getContext();
        cameraBuffer = cl::Buffer(context,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  sizeof(GPUCamera),
                                  &gpu_camera);

        // Set kernel arguments with camera buffer
        kernel.setArg(0, outputBuffer);
        kernel.setArg(1, accumBuffer);
        kernel.setArg(2, width);
        kernel.setArg(3, height);
        kernel.setArg(4, frameCount);
        kernel.setArg(5, shapesBuffer);
        // Pass the actual number of GPU shapes stored in the shapes buffer
        kernel.setArg(6, shapesCount);
        kernel.setArg(7, cameraBuffer);   // Use camera buffer instead of direct parameters , somehow it's giving better performance
        kernel.setArg(8, materialBuffer); // Buffer containing all the material data
        kernel.setArg(9, materialCount);  // Number of materials in the scene
        kernel.setArg(10, textureBuffer); // Buffer containing all texture data
        kernel.setArg(11, bvhCount );     // Number of BVH in the scene
        kernel.setArg(12, bvhBuffer);     // Buffer containing all BVH data


        // Use optimal work-group size for better GPU performance
        size_t globalSize = width * height;
        size_t localSize = 256; // Typical optimal size for modern GPUs

        // Round up to nearest multiple of localSize
        size_t adjustedGlobalSize = ((globalSize + localSize - 1) / localSize) * localSize;

        queue.enqueueNDRangeKernel(kernel, cl::NullRange,
                                   cl::NDRange(adjustedGlobalSize),
                                   cl::NDRange(localSize));

        // Map buffer for zero-copy read (faster than enqueueReadBuffer)
        float *mappedPtr = (float *)queue.enqueueMapBuffer(outputBuffer, CL_TRUE, CL_MAP_READ, 0,
                                                           width * height * 3 * sizeof(float));

        // Copy data from mapped memory
        std::memcpy(imageData.data(), mappedPtr, width * height * 3 * sizeof(float));

        // Unmap the buffer
        queue.enqueueUnmapMemObject(outputBuffer, mappedPtr);
        queue.finish();

        // std::cout << "Frame rendered: " << width << "x" << height << std::endl;

        // Increment frame count for next frame
        frameCount++;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred in render" << std::endl;
    }
}

// setup the bugger containing all GPU shapes
void RenderEngine::setupShapesBuffer()
{
    SceneManager &sceneManager = SceneManager::getInstance();
    const std::vector<Shape *> &shapes = sceneManager.getShapes();
    cl::CommandQueue queue = deviceManager->getCommandQueue();
    cl::Context context = deviceManager->getContext();

    std::vector<GPUShape> gpu_shapes;

    for (auto *shape : shapes)
    {
        GPUShape gpu_shape;
        ShapeType type = shape->getType();
        gpu_shape.type = type;

        switch (type)
        {
        case SPHERE:
        {
            Sphere *sphere = static_cast<Sphere *>(shape);
            gpu_shape.data.sphere = sphere->toGPU();
            break;
        }
        case SQUARE:
        {
            Square *square = static_cast<Square *>(shape);
            gpu_shape.data.square = square->toGPU();
            break;
        }
        case TRIANGLE:
        {
            Triangle *triangle = static_cast<Triangle *>(shape);
            gpu_shape.data.triangle = triangle->toGPU();
            break;
        }
        case MESH:
        {
            Mesh *mesh = static_cast<Mesh *>(shape);
            int i = 0;
            for (const auto &tri : mesh->getTriangles())
            {
                gpu_shapes.push_back(GPUShape());
                GPUShape &mesh_gpu_shape = gpu_shapes.back();
                mesh_gpu_shape.data.triangle = tri.toGPU();
                mesh_gpu_shape.data.triangle.materialIndex = mesh->getMaterial() ? mesh->getMaterial()->getMaterialId() : -1;
                mesh_gpu_shape.type = ShapeType::TRIANGLE;
                i++;
            }
            std::cout << "Mesh with " << i << " triangles added to GPU buffer." << std::endl;
            continue;
        }
        default:
            std::cerr << "Unknown shape type encountered in setupShapesBuffer: " << type << std::endl;
            break;
        }

        gpu_shapes.push_back(gpu_shape);
    }

    size_t buffer_size = gpu_shapes.size() * sizeof(GPUShape);
    // Update shapesCount for kernel use
    shapesCount = static_cast<int>(gpu_shapes.size());

    if (buffer_size > 0)
    {
        shapesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  buffer_size,
                                  gpu_shapes.data());
        std::cout << "Buffer created or updated successfully! (" << shapesCount << " shapes)" << std::endl;
    }
    else
    {
        // Create a dummy shape buffer to avoid null buffer issues
        GPUShape dummyShape = {};
        dummyShape.type = UNDEFINED;
        shapesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  sizeof(GPUShape),
                                  &dummyShape);
        shapesCount = 0;
        std::cout << "No GPU shapes - created dummy buffer" << std::endl;
    }
}

// setup the buffer that contain all the material
void RenderEngine::setupMaterialBuffer()
{
    SceneManager &sceneManager = SceneManager::getInstance();
    sceneManager.updateUniqueMaterials(); // update the materials list to have only unique materials
    const std::vector<Material *> &materials = sceneManager.getMaterials();
    cl::CommandQueue queue = deviceManager->getCommandQueue();
    cl::Context context = deviceManager->getContext();

    std::vector<GPUMaterial> gpu_materials;

    for (auto *material : materials)
    {
        if (!material) {
            std::cerr << "Warning: Null material pointer in materials vector, skipping..." << std::endl;
            continue;
        }
        GPUMaterial gpu_material = material->toGPU();
        gpu_materials.push_back(gpu_material);
    }

    // Setup texture buffer and update texture offsets in gpu_materials
    setupTextureBuffer(gpu_materials);

    size_t buffer_size = gpu_materials.size() * sizeof(GPUMaterial);
    // Update materialCount for kernel use
    materialCount = static_cast<int>(gpu_materials.size());

    if (buffer_size > 0)
    {
        materialBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    buffer_size,
                                    gpu_materials.data());
        std::cout << "Material buffer created or updated successfully! (" << materialCount << " materials)" << std::endl;
    }
    else
    {
        // Create a dummy material buffer to avoid null buffer issues
        GPUMaterial dummyMaterial = {};
        dummyMaterial.material_id = -1;
        materialBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(GPUMaterial),
                                    &dummyMaterial);
        materialCount = 0;
        std::cout << "No GPU materials - created dummy buffer" << std::endl;
    }
}

// Setup texture buffer containing all texture image data
void RenderEngine::setupTextureBuffer(std::vector<GPUMaterial>& gpu_materials)
{
    SceneManager &sceneManager = SceneManager::getInstance();
    const std::vector<Material *> &materials = sceneManager.getMaterials();
    cl::Context context = deviceManager->getContext();

    // Collect all texture data into a single buffer
    std::vector<unsigned char> allTextureData;
    int currentOffset = 0;

    // Safety check: ensure gpu_materials vector matches materials vector size
    if (gpu_materials.size() != materials.size()) {
        std::cerr << "Error: gpu_materials size (" << gpu_materials.size() 
                  << ") doesn't match materials size (" << materials.size() << ")" << std::endl;
        return;
    }

    for (size_t i = 0; i < materials.size(); i++)
    {
        Material* material = materials[i];
        if (!material) {
            std::cerr << "Warning: Null material at index " << i << std::endl;
            if (i < gpu_materials.size()) {
                gpu_materials[i].texture_offset = -1;
                gpu_materials[i].normal_map_offset = -1;
            }
            continue;
        }
        
        try {
            const ppmLoader::ImageRGB& image = material->getImage();
            const ppmLoader::ImageRGB& normals = material->getNormals();

            // Add texture data if exists
            if (!image.data.empty() && image.w > 0 && image.h > 0)
            {
                gpu_materials[i].texture_offset = currentOffset;
                
                // Copy RGB data (3 bytes per pixel)
                for (const auto& pixel : image.data)
                {
                    allTextureData.push_back(pixel.r);
                    allTextureData.push_back(pixel.g);
                    allTextureData.push_back(pixel.b);
                }
                
                currentOffset += image.data.size() * 3; // 3 bytes per pixel (RGB)
            }
            else
            {
                gpu_materials[i].texture_offset = -1; // No texture
            }

            // Add normal map data if exists
            if (material->hasNormalMap() && !normals.data.empty() && normals.w > 0 && normals.h > 0)
            {
                gpu_materials[i].normal_map_offset = currentOffset;
                
                // Copy RGB data for normal map
                for (const auto& pixel : normals.data)
                {
                    allTextureData.push_back(pixel.r);
                    allTextureData.push_back(pixel.g);
                    allTextureData.push_back(pixel.b);
                }
                
                currentOffset += normals.data.size() * 3; // 3 bytes per pixel (RGB)
            }
            else
            {
                gpu_materials[i].normal_map_offset = -1; // No normal map
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing material " << i << ": " << e.what() << std::endl;
            gpu_materials[i].texture_offset = -1;
            gpu_materials[i].normal_map_offset = -1;
        }
    }

    // Create texture buffer - always create a buffer even if empty (OpenCL requires valid buffer)
    if (!allTextureData.empty())
    {
        size_t buffer_size = allTextureData.size() * sizeof(unsigned char);
        textureBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   buffer_size,
                                   allTextureData.data());
        std::cout << "Texture buffer created successfully! (" << allTextureData.size() << " bytes)" << std::endl;
    }
    else
    {
        // Create a dummy 1-byte buffer to avoid null buffer issues
        unsigned char dummyData[1] = {0};
        textureBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(dummyData),
                                   dummyData);
        std::cout << "No texture data - created dummy buffer" << std::endl;
    }
}

void RenderEngine::setupBVHBuffer()
{
    SceneManager &sceneManager = SceneManager::getInstance();

    cl::CommandQueue queue = deviceManager->getCommandQueue();
    cl::Context context = deviceManager->getContext();
    std::vector<BVH*> bvhList = sceneManager.getBVHLists();

    std::vector<GPUBVH> bvhListGPU;

    // convert each BVH to GPUBVH
    for (auto* bvh : bvhList)
    {
        GPUBVH gpu_bvh = bvh->toGPU();
        bvhListGPU.push_back(gpu_bvh);
    }

    bvhCount = static_cast<int>(bvhList.size());

    size_t buffer_size = bvhListGPU.size() * sizeof(GPUBVH);

    if (buffer_size > 0)
    {
        bvhBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  buffer_size,
                                  bvhListGPU.data());
        std::cout << "BVH Buffer created or updated successfully! (" << bvhCount << " BVH)" << std::endl;
    }
    else
    {
        // Create a dummy shape buffer to avoid null buffer issues
        BVH dummyBVH = {};
        bvhBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  sizeof(BVH),
                                  &dummyBVH);
        bvhCount = 0;
        std::cout << "No GPU BVH - created dummy buffer" << std::endl;
    }

}
