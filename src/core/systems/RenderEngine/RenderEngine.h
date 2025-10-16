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

private:
    KernelManager* kernelManager;
    DeviceManager* deviceManager;
    
    cl::Buffer outputBuffer;
    std::vector<float> imageData;

    int height;
    int width;

    
    void setupBuffers(int width, int height);
};