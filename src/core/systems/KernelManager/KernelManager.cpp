#include "KernelManager.h"
#include <fstream>
#include <iostream>
KernelManager *KernelManager::instance = nullptr;

KernelManager &KernelManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new KernelManager();
    }
    return *instance;
}

// preload all kernels at startup
void KernelManager::preloadAllKernels()
{
    loadKernel("hello", "kernels/hello.cl"); // <name, path>
    loadKernel("testKernel", "kernels/testKernel.cl");
}

void KernelManager::loadKernel(const std::string &name, const std::string &filePath)
{
    // Try to read kernel file from multiple possible locations
    std::ifstream kernelFile;
    std::string actualFilePath;
    
    // First try the relative path from current working directory
    kernelFile.open(filePath);
    if (kernelFile.is_open()) {
        actualFilePath = filePath;
    } else {
        // Try just the filename (for files copied to executable directory)
        size_t lastSlash = filePath.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string filename = filePath.substr(lastSlash + 1);
            kernelFile.open(filename);
            if (kernelFile.is_open()) {
                actualFilePath = filename;
            }
        }
    }
    
    if (!kernelFile.is_open())
    {
        throw std::runtime_error("Failed to open kernel file: " + filePath + " (also tried standalone filename)");
    }
    std::string sourceCode((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
    kernelFile.close();

    // Get OpenCL context and device from DeviceManager
    DeviceManager *deviceManager = DeviceManager::getInstance();
    cl::Context context = deviceManager->getContext();
    cl::Device device = deviceManager->getDevice();

    // Create program from source
    cl::Program::Sources sources;
    sources.push_back({sourceCode.c_str(), sourceCode.length()});
    cl::Program program(context, sources);

    // Build program

    program.build({device});

    // Create kernel
    cl::Kernel kernel(program, name.c_str());

    // Store program and kernel
    programs[name] = program;
    kernels[name] = kernel;

    std::cout << "Loaded and built kernel: " << name << " from " << actualFilePath << std::endl;
}