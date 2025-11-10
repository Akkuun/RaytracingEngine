#pragma once
#include <vector>
#include <CL/opencl.hpp>
#include "../KernelManager/KernelManager.h"
#include "../DeviceManager/DeviceManager.h"
#include "../SceneManager/SceneManager.h"
class RenderEngine
{
public:
    RenderEngine();
    ~RenderEngine() = default;
    
    void render(int width, int height);
    const std::vector<float>& getImageData() const { return imageData; }
    void resetAccumulation() { frameCount = 0; } // Call when camera/scene changes
    inline SceneManager* getSceneManager() { return sceneManager; }
    
private:
    KernelManager* kernelManager;
    DeviceManager* deviceManager;
    SceneManager* sceneManager;
    
    cl::Buffer outputBuffer;
    cl::Buffer accumBuffer;
    std::vector<float> imageData;

    int currentWidth = 0;
    int currentHeight = 0;
    int height;
    int width;
    int frameCount = 0;
    int lastWidth = 0;
    int lastHeight = 0;

    
    void setupBuffers(int width, int height);
};