#include <iostream>
#include "src/core/defines/Defines.h"

int main() {
    std::cout << "=== CPU Side Structure Sizes ===" << std::endl;
    std::cout << "sizeof(ShapeType): " << sizeof(ShapeType) << " bytes" << std::endl;
    std::cout << "sizeof(Vec3): " << sizeof(Vec3) << " bytes" << std::endl;
    std::cout << "sizeof(GPUSphere): " << sizeof(GPUSphere) << " bytes" << std::endl;
    std::cout << "sizeof(GPUSquare): " << sizeof(GPUSquare) << " bytes" << std::endl;
    std::cout << "sizeof(GPUShape): " << sizeof(GPUShape) << " bytes" << std::endl;
    std::cout << "sizeof(int): " << sizeof(int) << " bytes" << std::endl;
    std::cout << "sizeof(float): " << sizeof(float) << " bytes" << std::endl;
    
    std::cout << "\n=== Expected OpenCL Sizes ===" << std::endl;
    std::cout << "sizeof(float3): 16 bytes (with padding)" << std::endl;
    std::cout << "sizeof(int): 4 bytes" << std::endl;
    
    // Test alignement
    GPUShape test_shape;
    std::cout << "\n=== Memory Layout ===" << std::endl;
    std::cout << "Offset of type in GPUShape: " << offsetof(GPUShape, type) << std::endl;
    std::cout << "Offset of data in GPUShape: " << offsetof(GPUShape, data) << std::endl;
    
    return 0;
}
