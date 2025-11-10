#pragma once
#include <vector>
#include <CL/opencl.hpp>
#include "../KernelManager/KernelManager.h"
#include "../DeviceManager/DeviceManager.h"

class RenderEngine
{
public:
    RenderEngine();
    ~RenderEngine() = default;
    
    void render(int width, int height);
    std::vector<float> getImageData() const { return imageData; }
    void resetAccumulation() { frameCount = 0; } // Call when camera/scene changes

private:
    KernelManager* kernelManager;
    DeviceManager* deviceManager;
    
    cl::Buffer outputBuffer;
    cl::Buffer accumBuffer;
    std::vector<float> imageData;

    int height;
    int width;
    int frameCount = 0;
    int lastWidth = 0;
    int lastHeight = 0;

    
    void setupBuffers(int width, int height);
};