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
        std::string kernelSource = loadKernel("test_sizes.cl");
        cl::Program program(context, kernelSource);
        try {
            program.build({device});
        } catch (const std::exception& e) {
            std::cerr << "Build error: " << e.what() << std::endl;
            std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            std::cerr << "Build log:\n" << buildLog << std::endl;
            return 1;
        }
        
        // Create kernel
        cl::Kernel kernel(program, "test_sizes");
        
        // Execute
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1), cl::NullRange);
        queue.finish();
        
        std::cout << "Kernel executed successfully. Check console for printf output." << std::endl;
        
    

    return EXIT_SUCCESS;
}
