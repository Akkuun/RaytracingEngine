#pragma once
#include <vector>
#include <CL/opencl.hpp>
#include "../KernelManager/KernelManager.h"
#include "../DeviceManager/DeviceManager.h"
#include "../SceneManager/SceneManager.h"
#include "../../camera/Camera.h"

class RenderEngine
{
public:
    RenderEngine();
    ~RenderEngine() = default;
    
    void render(int width, int height);
    const std::vector<float>& getImageData() const { return imageData; }
    void resetAccumulation() { frameCount = 0; } // Call when camera/scene changes
    void markShapesDirty() { shapesBufferDirty = true; } // Call when shapes are added/removed/modified
    void markCameraDirty() { cameraBufferDirty = true; frameCount = 0; } // Call when camera changes
    Camera& getCamera() { return sceneCamera; } // Get camera reference for UI
    inline SceneManager& getSceneManager() { return SceneManager::getInstance(); }
    
private:
    KernelManager* kernelManager;
    DeviceManager* deviceManager;
    
    cl::Buffer outputBuffer;
    cl::Buffer accumBuffer;
    cl::Buffer shapesBuffer;
    cl::Buffer cameraBuffer; // Temporary for testing

    std::vector<float> imageData;

    int currentWidth = 0;
    int currentHeight = 0;
    int frameCount = 0;
    bool shapesBufferDirty = true; // Track if shapes buffer needs update
    bool cameraBufferDirty = true; // Track if camera buffer needs update

    Camera sceneCamera;

    
    void setupBuffers(int width, int height);
    void setupShapesBuffer();
    // setupCameraBuffer removed - camera params now passed directly
};