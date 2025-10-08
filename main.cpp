#define CL_HPP_TARGET_OPENCL_VERSION 300
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <CL/opencl.hpp>
#include <iostream>
#include <fstream>
#include <vector>

int main()
{

    // Lire le code source
    std::ifstream file("hello.cl");
    std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    cl::Program::Sources sources(1, {src.c_str(), src.size()});

    // Plateforme et device
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    auto platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    auto device = devices.front();

    std::cout << "→ Plateforme : " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    std::cout << "→ Device     : " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

    // Contexte + queue
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // Programme et kernel
    cl::Program program(context, sources);
    program.build({device});
    cl::Kernel kernel(program, "hello");

    // Buffer pour le message (change to int)
    cl::Buffer buffer(context, CL_MEM_WRITE_ONLY, sizeof(int));
    kernel.setArg(0, buffer);

    // Lancer le kernel
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1));
    queue.finish();

    // Lire la sortie (change to int)
    int result = 0;
    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(result), &result);
    std::cout << "Résultat du kernel : " << result << std::endl;

    return 0;
}
