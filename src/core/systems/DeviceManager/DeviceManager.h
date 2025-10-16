#pragma once

#include <CL/opencl.hpp>

class DeviceManager
{
protected:
    DeviceManager();
    ~DeviceManager();

    static DeviceManager *deviceManagerInstance;

public:
    DeviceManager(const DeviceManager &) = delete;
    DeviceManager &operator=(const DeviceManager &) = delete;
    DeviceManager(DeviceManager &&) = delete;
    DeviceManager &operator=(DeviceManager &&) = delete;

    static DeviceManager *getInstance();
    static void destroyInstance();

    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;

    bool isInitialized = false;
    void initialize();

    
    
    inline cl::Context getContext() const { return context; };
    inline cl::Device getDevice() const { return device; };
    inline cl::CommandQueue getCommandQueue() const { return queue; };

    
};