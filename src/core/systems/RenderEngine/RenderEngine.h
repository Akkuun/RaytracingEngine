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
    const std::vector<float> &getImageData() const { return imageData; }
    inline SceneManager &getSceneManager() { return SceneManager::getInstance(); }
    Camera &getCamera() { return sceneCamera; } // Get camera reference for UI

    // Call functions
    void resetAccumulation() { frameCount = 0; } // Call when camera/scene changes
    void markShapesDirty()
    {
        shapesBufferDirty = true;
        frameCount = 0;
    } // Call when shapes are added/removed/modified
    void markCameraDirty()
    {
        cameraBufferDirty = true;
        frameCount = 0;
    } // Call when camera changes
    void markMaterialDirty()
    {
        materialBufferDirty = true;
        frameCount = 0;
    } // Call when a material is modified
    void notifySceneChanged()
    {
        shapesBufferDirty = true;
        materialBufferDirty = true;
        frameCount = 0;
    } // MANDATORY , called when the scene or a material has been changed (shapes or material added/removed/modified)
    void markBVHDirty()
    {
        bvhBufferDirty = true;
        frameCount = 0;
    } // Call when a mesh is added/removed/modified
    // TODO Later

private:
    KernelManager *kernelManager;
    DeviceManager *deviceManager;

    cl::Buffer outputBuffer;
    cl::Buffer accumBuffer;
    cl::Buffer shapesBuffer;
    cl::Buffer cameraBuffer;
    cl::Buffer materialBuffer;
    cl::Buffer textureBuffer;      // Buffer containing all texture data (RGB pixels)
    cl::Buffer bvhNodesBuffer;     // Buffer containing all flattened BVH nodes
    cl::Buffer bvhTrianglesBuffer; // Buffer containing all BVH triangles

    std::vector<float> imageData;

    int currentWidth = 0;
    int currentHeight = 0;
    int frameCount = 0;
    bool shapesBufferDirty = true; // Track if shapes buffer needs update
    bool cameraBufferDirty = true; // Track if camera buffer needs update
    int shapesCount = 0;           // Number of GPU shapes stored in shapesBuffer
    bool materialBufferDirty = true;
    int materialCount = 0;          // Number of GPU material stored in materialBuffer
    bool textureBufferDirty = true; // Track if texture buffer needs update
    bool bvhBufferDirty = true;     // Track if BVH buffer needs update (when a mesh is added/removed/modified)
    int bvhCount = 0;               // Number of BVH stored stored in bvhBuffer
    int bvhTrianglesCount = 0;     // Number of triangles stored in bvhTrianglesBuffer

    Camera sceneCamera;

    void setupBuffers(int width, int height);
    void setupShapesBuffer();
    void setupMaterialBuffer();
    void setupTextureBuffer(std::vector<GPUMaterial> &gpu_materials);
};