#include "RenderEngine.h"
#include <iostream>

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

        // Create or recreate output buffer
        cl::Context context = deviceManager->getContext();
        outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, imageSize);
        
        currentWidth = width;
        currentHeight = height;
    }
}

void RenderEngine::render(int width, int height)
{
    try
    {
        setupBuffers(width, height);

        cl::Kernel kernel = kernelManager->getKernel("render_kernel");
        cl::CommandQueue queue = deviceManager->getCommandQueue(); 

       // std::cout << "Setting kernel args: width=" << width << ", height=" << height << std::endl;
        
        kernel.setArg(0, outputBuffer);
        kernel.setArg(1, width);
        kernel.setArg(2, height);

        // Use optimal work-group size for better GPU performance
        size_t globalSize = width * height;
        size_t localSize = 256; // Typical optimal size for modern GPUs
        
        // Round up to nearest multiple of localSize
        size_t adjustedGlobalSize = ((globalSize + localSize - 1) / localSize) * localSize;

        queue.enqueueNDRangeKernel(kernel, cl::NullRange, 
                                   cl::NDRange(adjustedGlobalSize), 
                                   cl::NDRange(localSize)); 
        queue.finish();

       // std::cout << "Kernel executed successfully" << std::endl;

        queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0,
                                width * height * 3 * sizeof(float), // we allocate 3 floats per pixel (RGB)
                                imageData.data()); // that we put in imageData , a vector of float 

        //std::cout << "Frame rendered: " << width << "x" << height << std::endl;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred in render" << std::endl;
    }
}

