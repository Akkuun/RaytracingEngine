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
    
    // Camera parameters are now passed directly to kernel, no buffer needed
    // Reset frame count if camera changed
    if (cameraBufferDirty)
    {
        frameCount = 0;  // Reset accumulation when camera changes
        cameraBufferDirty = false;
    }
}

void RenderEngine::render(int width, int height)
{
    try
    {
        setupBuffers(width, height);

        cl::Kernel kernel = kernelManager->getKernel("render_kernel");
        cl::CommandQueue queue = deviceManager->getCommandQueue(); 

        // Get camera parameters and create GPU buffer (for testing)
        GPUCamera gpu_camera = Camera::getInstance().toGPU();
        cl::Context context = deviceManager->getContext();
        cameraBuffer = cl::Buffer(context, 
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  sizeof(GPUCamera), 
                                  &gpu_camera);

        // Set kernel arguments with camera buffer (old method)
        kernel.setArg(0, outputBuffer);
        kernel.setArg(1, accumBuffer);
        kernel.setArg(2, width);
        kernel.setArg(3, height);
        kernel.setArg(4, frameCount);
        kernel.setArg(5, shapesBuffer);
    // Pass the actual number of GPU shapes stored in the shapes buffer
    kernel.setArg(6, shapesCount);
        kernel.setArg(7, cameraBuffer);  // Use camera buffer instead of direct parameters , somehow it's giving better performance

        // Use optimal work-group size for better GPU performance
        size_t globalSize = width * height;
        size_t localSize = 256; // Typical optimal size for modern GPUs
        
        // Round up to nearest multiple of localSize
        size_t adjustedGlobalSize = ((globalSize + localSize - 1) / localSize) * localSize;

        queue.enqueueNDRangeKernel(kernel, cl::NullRange, 
                                   cl::NDRange(adjustedGlobalSize), 
                                   cl::NDRange(localSize)); 
        

        
        // Map buffer for zero-copy read (faster than enqueueReadBuffer)
        float* mappedPtr = (float*)queue.enqueueMapBuffer(outputBuffer, CL_TRUE, CL_MAP_READ, 0,
                                                          width * height * 3 * sizeof(float));
        
        // Copy data from mapped memory
        std::memcpy(imageData.data(), mappedPtr, width * height * 3 * sizeof(float));
        
        // Unmap the buffer
        queue.enqueueUnmapMemObject(outputBuffer, mappedPtr);
        queue.finish(); 

        //std::cout << "Frame rendered: " << width << "x" << height << std::endl;
        
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
void RenderEngine::setupShapesBuffer(){
    SceneManager& sceneManager = SceneManager::getInstance();
    const std::vector<Shape*>& shapes = sceneManager.getShapes();
    cl::CommandQueue queue = deviceManager->getCommandQueue();
    cl::Context context = deviceManager->getContext();

    std::vector<GPUShape> gpu_shapes;

    for (auto* shape : shapes) {
        GPUShape gpu_shape;
        ShapeType type = shape->getType();
        gpu_shape.type = type;
        
        switch (type) {
            case SPHERE: {
                Sphere* sphere = static_cast<Sphere*>(shape);
                gpu_shape.data.sphere = sphere->toGPU();
                break;
            }
            case SQUARE: {
                Square* square = static_cast<Square*>(shape);
                gpu_shape.data.square = square->toGPU();
                break;
            }
            case TRIANGLE: {
                Triangle* triangle = static_cast<Triangle*>(shape);
                gpu_shape.data.triangle = triangle->toGPU();
                break;
            }
            case MESH: {
                Mesh* mesh = static_cast<Mesh*>(shape);
                int i = 0;
                for (const auto& tri : mesh->getTriangles()) {
                    gpu_shapes.push_back(GPUShape());
                    GPUShape& mesh_gpu_shape = gpu_shapes.back();
                    mesh_gpu_shape.data.triangle = tri.toGPU();
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

    if (buffer_size > 0) {
        shapesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  buffer_size,
                                  gpu_shapes.data());
        std::cout << "Buffer created or updated successfully! (" << shapesCount << " shapes)" << std::endl;
    } else {
        // No shapes: ensure shapesBuffer is reset and notify
        shapesBuffer = cl::Buffer();
        std::cout << "No GPU shapes to upload (shapesCount=0)" << std::endl;
    }
}


