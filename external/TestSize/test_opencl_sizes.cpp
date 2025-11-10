#include <CL/opencl.hpp>
#include <iostream>
#include <fstream>
#include <vector>

std::string loadKernel(const std::string& filename) {
    std::ifstream file(filename);
    return std::string(std::istreambuf_iterator<char>(file), 
                      std::istreambuf_iterator<char>());
}

int main() {

        // Get platform
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        cl::Platform platform = platforms[0];
        
        // Get device
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        cl::Device device = devices[0];
        
        // Create context and queue
        cl::Context context(device);
        cl::CommandQueue queue(context, device);
        
        // Load and compile kernel
        std::string kernelSource = loadKernel("kernels/test_sizes.cl");
        cl::Program program(context, kernelSource);
        program.build({device});
        
        // Create kernel
        cl::Kernel kernel(program, "test_sizes");
        
        // Execute
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1), cl::NullRange);
        queue.finish();
        
    

    return EXIT_SUCCESS;
}
