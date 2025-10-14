#include "DeviceManager.h"
#include <iostream>


DeviceManager *DeviceManager::deviceManagerInstance = nullptr;


DeviceManager::DeviceManager() {}

DeviceManager::~DeviceManager(){}

DeviceManager *DeviceManager::getInstance()
{
    if (deviceManagerInstance == nullptr)
    {
        deviceManagerInstance = new DeviceManager();
    }
    return deviceManagerInstance;
}

void DeviceManager::destroyInstance()
{
    delete deviceManagerInstance;
    deviceManagerInstance = nullptr;
}

void DeviceManager::initialize()
{
    if (isInitialized) return;

    cl_int err;

    // Get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        throw std::runtime_error("No OpenCL platforms found. Check OpenCL installation!");
    }
    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    // Get default device of the default platform
    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
    if (all_devices.size() == 0) {
        throw std::runtime_error("No OpenCL devices found. Check OpenCL installation!");
    }
    device = all_devices[0];
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

    // Create context
    context = cl::Context({ device }, nullptr, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create OpenCL context.");
    }

    // Create command queue
    queue = cl::CommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create OpenCL command queue.");
    }

    isInitialized = true;
}
