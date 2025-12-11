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
        kernel.setArg(7, cameraBuffer);        // Use camera buffer instead of direct parameters , somehow it's giving better performance
        kernel.setArg(8, materialBuffer);      // Buffer containing all the material data
        kernel.setArg(9, materialCount);       // Number of materials in the scene
        kernel.setArg(10, textureBuffer);      // Buffer containing all texture data
        kernel.setArg(11, bvhCount);           // Number of BVH in the scene
        kernel.setArg(12, bvhNodesBuffer);     // BVH nodes buffer (flattened)
        kernel.setArg(13, bvhTrianglesCount); // Number of BVH triangles
        kernel.setArg(14, bvhTrianglesBuffer); // BVH triangles buffer

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
    std::vector<GPUBVHNode> gpu_bvh_nodes;
    std::vector<GPUTriangle> gpu_bvh_triangles;

    bool containsBVH = false;

    // Pre-calculate total size needed to avoid reallocations
    size_t estimatedShapes = 0;
    size_t estimatedBVHNodes = 0;
    size_t estimatedBVHTriangles = 0;

    for (auto *shape : shapes)
    {
        if (shape->getType() == MESH)
        {
            Mesh *mesh = static_cast<Mesh *>(shape);
            estimatedBVHTriangles += mesh->getTriangles().size();
            estimatedBVHNodes += log2(mesh->getTriangles().size());
        }
        else
        {
            estimatedShapes++;
        }
    }
    gpu_shapes.reserve(estimatedShapes);
    gpu_bvh_nodes.reserve(estimatedBVHNodes);
    gpu_bvh_triangles.reserve(estimatedBVHTriangles);

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
            GPUBVH bvh_gpu;
            bvh_gpu.material_index = mesh->getMaterial() ? mesh->getMaterial()->getMaterialId() : -1;

            // Store offsets and counts
            bvh_gpu.node_offset = static_cast<int>(gpu_bvh_nodes.size());
            bvh_gpu.triangle_offset = static_cast<int>(gpu_bvh_triangles.size());
            bvh_gpu.node_count = mesh->getBVH().nodes.size();
            bvh_gpu.triangle_count = mesh->getBVH().triangles.size();   

            // Append BVH nodes
            for (const auto &node : mesh->getBVH().nodes)
            {
                gpu_bvh_nodes.push_back(node.toGPU());
            }

            // Append BVH triangles (use BVH's reordered triangles, not original mesh triangles)
            for (const auto &tri : mesh->getBVH().triangles)
            {
                gpu_bvh_triangles.push_back(tri.toGPU());
            }

            gpu_shape.data.bvh = bvh_gpu;
            containsBVH = true;
            break;
        }
        default:
            std::cerr << "Unknown shape type encountered in setupShapesBuffer: " << type << std::endl;
            break;
        }

        gpu_shapes.push_back(gpu_shape);
    }

    size_t shape_buffer_size = gpu_shapes.size() * sizeof(GPUShape);
    size_t bvh_nodes_buffer_size = gpu_bvh_nodes.size() * sizeof(GPUBVHNode);
    size_t bvh_triangles_buffer_size = gpu_bvh_triangles.size() * sizeof(GPUTriangle);

    // Update shapesCount for kernel use
    shapesCount = static_cast<int>(gpu_shapes.size());
    bvhCount = static_cast<int>(gpu_bvh_nodes.size() > 0 ? 1 : 0); // For now, we consider one BVH if there are any nodes
    bvhTrianglesCount = static_cast<int>(gpu_bvh_triangles.size());

    if (shape_buffer_size > 0)
    {
        shapesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  shape_buffer_size,
                                  gpu_shapes.data());
        std::cout << "Buffer created or updated successfully! (" << shapesCount << " shapes)" << std::endl;
        if (containsBVH)
        {
            bvhNodesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                        bvh_nodes_buffer_size,
                                        gpu_bvh_nodes.data());
            bvhTrianglesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                            bvh_triangles_buffer_size,
                                            gpu_bvh_triangles.data());
            std::cout << "BVH Buffers created or updated successfully! (" << bvhCount << " BVH, " << bvhTrianglesCount << " triangles)" << std::endl;
        }
        else
        {
            // Create dummy BVH buffers to avoid null buffer issues
            GPUBVHNode dummyBVHNode = {};
            bvhNodesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                        sizeof(GPUBVHNode),
                                        &dummyBVHNode);
            bvhTrianglesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                            sizeof(GPUTriangle),
                                            nullptr);
            bvhCount = 0;
            bvhTrianglesCount = 0;
            std::cout << "No GPU BVH - created dummy buffers" << std::endl;
        }
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

        GPUBVHNode dummyBVHNode = {};
        bvhNodesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(GPUBVHNode),
                                    &dummyBVHNode);
        bvhTrianglesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                        sizeof(GPUTriangle),
                                        nullptr);
        bvhCount = 0;
        bvhTrianglesCount = 0;
        std::cout << "No GPU BVH - created dummy buffers" << std::endl;
    }
}

// setup the buffer that contain all the material
// Materials are stored at their material_id index for O(1) direct access on GPU
void RenderEngine::setupMaterialBuffer()
{
    SceneManager &sceneManager = SceneManager::getInstance();
    sceneManager.updateUniqueMaterials(); // update the materials list to have only unique materials
    const std::vector<Material *> &materials = sceneManager.getMaterials();
    cl::CommandQueue queue = deviceManager->getCommandQueue();
    cl::Context context = deviceManager->getContext();

    if (materials.empty())
    {
        // Create a dummy material buffer to avoid null buffer issues
        GPUMaterial dummyMaterial = {};
        dummyMaterial.material_id = -1;
        materialBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(GPUMaterial),
                                    &dummyMaterial);
        materialCount = 0;
        std::cout << "No GPU materials - created dummy buffer" << std::endl;
        return;
    }

    // Find the maximum material_id to determine array size
    int maxMaterialId = -1;
    for (auto *material : materials)
    {
        if (material && material->getMaterialId() > maxMaterialId)
        {
            maxMaterialId = material->getMaterialId();
        }
    }

    // Create array sized to hold all materials at their material_id index
    // This enables O(1) direct access: materials[materialIndex] instead of linear search
    std::vector<GPUMaterial> gpu_materials(maxMaterialId + 1);

    // Initialize all slots with invalid material_id (-1)
    for (auto &mat : gpu_materials)
    {
        mat.material_id = -1;
    }

    // Place each material at its material_id index
    for (auto *material : materials)
    {
        if (!material)
        {
            std::cerr << "Warning: Null material pointer in materials vector, skipping..." << std::endl;
            continue;
        }
        int matId = material->getMaterialId();
        if (matId >= 0 && matId <= maxMaterialId)
        {
            gpu_materials[matId] = material->toGPU();
        }
    }

    // Setup texture buffer and update texture offsets in gpu_materials
    setupTextureBuffer(gpu_materials);

    size_t buffer_size = gpu_materials.size() * sizeof(GPUMaterial);
    // Update materialCount for kernel use (now represents array size, not just count of valid materials)
    materialCount = static_cast<int>(gpu_materials.size());

    materialBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                buffer_size,
                                gpu_materials.data());
    std::cout << "Material buffer created or updated successfully! (" << materialCount << " material slots)" << std::endl;
}

// Setup texture buffer containing all texture image data
// gpu_materials is indexed by material_id, so we iterate through the actual materials
// and update the corresponding slot in gpu_materials
void RenderEngine::setupTextureBuffer(std::vector<GPUMaterial> &gpu_materials)
{
    SceneManager &sceneManager = SceneManager::getInstance();
    const std::vector<Material *> &materials = sceneManager.getMaterials();
    cl::Context context = deviceManager->getContext();

    // Pre-calculate total size needed for all texture data
    size_t totalSize = 0;
    for (auto *material : materials)
    {
        if (material)
        {
            const ppmLoader::ImageRGB &image = material->getImage();
            const ppmLoader::ImageRGB &normals = material->getNormals();

            if (!image.data.empty() && image.w > 0 && image.h > 0)
            {
                totalSize += image.data.size() * 3; // RGB: 3 bytes per pixel
            }

            if (material->hasNormalMap() && !normals.data.empty() && normals.w > 0 && normals.h > 0)
            {
                totalSize += normals.data.size() * 3; // RGB: 3 bytes per pixel
            }
        }
    }

    // Collect all texture data into a single buffer with pre-allocated space
    std::vector<unsigned char> allTextureData;
    allTextureData.reserve(totalSize);
    int currentOffset = 0;

    for (auto *material : materials)
    {
        if (!material)
        {
            std::cerr << "Warning: Null material pointer, skipping..." << std::endl;
            continue;
        }

        int matId = material->getMaterialId();
        if (matId < 0 || matId >= static_cast<int>(gpu_materials.size()))
        {
            std::cerr << "Warning: Invalid material_id " << matId << ", skipping..." << std::endl;
            continue;
        }

        try
        {
            const ppmLoader::ImageRGB &image = material->getImage();
            const ppmLoader::ImageRGB &normals = material->getNormals();

            // Add texture data if exists
            if (!image.data.empty() && image.w > 0 && image.h > 0)
            {
                gpu_materials[matId].texture_offset = currentOffset;

                // Optimized: Copy RGB data with direct buffer insertion (3 bytes per pixel)
                size_t pixelCount = image.data.size();
                size_t oldSize = allTextureData.size();
                allTextureData.resize(oldSize + pixelCount * 3);

                // Direct indexing for better performance - avoid push_back overhead
                unsigned char *dest = allTextureData.data() + oldSize;
                for (size_t i = 0; i < pixelCount; ++i)
                {
                    const auto &pixel = image.data[i];
                    dest[i * 3] = pixel.r;
                    dest[i * 3 + 1] = pixel.g;
                    dest[i * 3 + 2] = pixel.b;
                }

                currentOffset += pixelCount * 3; // 3 bytes per pixel (RGB)
            }
            else
            {
                gpu_materials[matId].texture_offset = -1; // No texture
            }

            // Add normal map data if exists
            if (material->hasNormalMap() && !normals.data.empty() && normals.w > 0 && normals.h > 0)
            {
                gpu_materials[matId].normal_map_offset = currentOffset;

                // Optimized: Copy RGB data for normal map with direct buffer insertion
                size_t pixelCount = normals.data.size();
                size_t oldSize = allTextureData.size();
                allTextureData.resize(oldSize + pixelCount * 3);

                // Direct indexing for better performance - avoid push_back overhead
                unsigned char *dest = allTextureData.data() + oldSize;
                for (size_t i = 0; i < pixelCount; ++i)
                {
                    const auto &pixel = normals.data[i];
                    dest[i * 3] = pixel.r;
                    dest[i * 3 + 1] = pixel.g;
                    dest[i * 3 + 2] = pixel.b;
                }

                currentOffset += pixelCount * 3; // 3 bytes per pixel (RGB)
            }
            else
            {
                gpu_materials[matId].normal_map_offset = -1; // No normal map
            }

            // Add metal map data if exists
            if (material->hasMetallicMap() && !material->getMetallic().data.empty())
            {
                gpu_materials[matId].metal_map_offset = currentOffset;

                // Copy RGB data for metal map
                for (const auto &pixel : material->getMetallic().data)
                {
                    allTextureData.push_back(pixel.r);
                    allTextureData.push_back(pixel.g);
                    allTextureData.push_back(pixel.b);
                }

                currentOffset += material->getMetallic().data.size() * 3; // 3 bytes per pixel (RGB)
            }
            else
            {
                gpu_materials[matId].metal_map_offset = -1; // No metal map
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error processing material " << matId << ": " << e.what() << std::endl;
            gpu_materials[matId].texture_offset = -1;
            gpu_materials[matId].normal_map_offset = -1;
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