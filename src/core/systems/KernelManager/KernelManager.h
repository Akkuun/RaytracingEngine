#pragma once
#include "../DeviceManager/DeviceManager.h"
#include <CL/opencl.hpp>
#include <unordered_map>
#include <string>

class KernelManager
{
private:
    static KernelManager *instance;
    std::unordered_map<std::string, cl::Kernel> kernels; // <name, kernel>
    std::unordered_map<std::string, cl::Program> programs; // <name, program>

    void loadKernel(const std::string &name, const std::string &filePath);

    KernelManager() { preloadAllKernels(); }

public:
    static KernelManager &getInstance();

    // get specific kernel by name to work with
    inline cl::Kernel &getKernel(const std::string &name)
    {
        return kernels.at(name);
    }

private:
    void preloadAllKernels();
};