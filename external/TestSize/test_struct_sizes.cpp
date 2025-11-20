#include <iostream>
#include "src/core/defines/Defines.h"
#include "src/core/camera/Camera.h"

int main() {
    std::cout << "=== CPU Side Structure Sizes ===" << std::endl;
    std::cout << "sizeof(int): " << sizeof(int) << " bytes" << std::endl;
    std::cout << "sizeof(float): " << sizeof(float) << " bytes" << std::endl;
    std::cout << "sizeof(Vec3): " << sizeof(Vec3) << " bytes (expected: 16)" << std::endl;
    std::cout << "sizeof(GPUSphere): " << sizeof(GPUSphere) << " bytes (expected: 48)" << std::endl;
    std::cout << "sizeof(GPUSquare): " << sizeof(GPUSquare) << " bytes (expected: 80)" << std::endl;
    std::cout << "sizeof(GPUTriangle): " << sizeof(GPUTriangle) << " bytes (expected: 64)" << std::endl;
    std::cout << "sizeof(GPUMaterial): " << sizeof(GPUMaterial) << " bytes (expected: 128)" << std::endl;
    std::cout << "sizeof(GPUShape): " << sizeof(GPUShape) << " bytes" << std::endl;
    std::cout << "sizeof(GPUCamera): " << sizeof(GPUCamera) << " bytes (expected: 64)" << std::endl;

    std::cout << "\n=== GPUSphere Memory Layout ===" << std::endl;
    std::cout << "Offset of radius: " << offsetof(GPUSphere, radius) << std::endl;
    std::cout << "Offset of pos: " << offsetof(GPUSphere, pos) << std::endl;
    std::cout << "Offset of materialIndex: " << offsetof(GPUSphere, materialIndex) << std::endl;
    
    std::cout << "\n=== GPUSquare Memory Layout ===" << std::endl;
    std::cout << "Offset of pos: " << offsetof(GPUSquare, pos) << std::endl;
    std::cout << "Offset of u_vec: " << offsetof(GPUSquare, u_vec) << std::endl;
    std::cout << "Offset of v_vec: " << offsetof(GPUSquare, v_vec) << std::endl;
    std::cout << "Offset of normal: " << offsetof(GPUSquare, normal) << std::endl;
    std::cout << "Offset of materialIndex: " << offsetof(GPUSquare, materialIndex) << std::endl;
    
    std::cout << "\n=== GPUTriangle Memory Layout ===" << std::endl;
    std::cout << "Offset of v0: " << offsetof(GPUTriangle, v0) << std::endl;
    std::cout << "Offset of v1: " << offsetof(GPUTriangle, v1) << std::endl;
    std::cout << "Offset of v2: " << offsetof(GPUTriangle, v2) << std::endl;
    std::cout << "Offset of materialIndex: " << offsetof(GPUTriangle, materialIndex) << std::endl;
    
    std::cout << "\n=== GPUMaterial Memory Layout ===" << std::endl;
    std::cout << "Offset of ambient: " << offsetof(GPUMaterial, ambient) << std::endl;
    std::cout << "Offset of diffuse: " << offsetof(GPUMaterial, diffuse) << std::endl;
    std::cout << "Offset of specular: " << offsetof(GPUMaterial, specular) << std::endl;
    std::cout << "Offset of shininess: " << offsetof(GPUMaterial, shininess) << std::endl;
    std::cout << "Offset of texture_scale_y: " << offsetof(GPUMaterial, texture_scale_y) << std::endl;
    std::cout << "Offset of emissive: " << offsetof(GPUMaterial, emissive) << std::endl;
    std::cout << "Offset of light_color: " << offsetof(GPUMaterial, light_color) << std::endl;
    std::cout << "Offset of light_intensity: " << offsetof(GPUMaterial, light_intensity) << std::endl;
    std::cout << "Offset of has_texture: " << offsetof(GPUMaterial, has_texture) << std::endl;
    std::cout << "Offset of texture_height: " << offsetof(GPUMaterial, texture_height) << std::endl;
    std::cout << "Offset of material_id: " << offsetof(GPUMaterial, material_id) << std::endl;

    return 0;
}
