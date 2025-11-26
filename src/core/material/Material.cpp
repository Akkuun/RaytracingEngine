#include "Material.h"
#include "../utils/imageLoader/ImageLoader.h"
#include "../math/vec3.h"

// constructors with default values
Material::Material()
    : transparency(0.0),
      index_medium(1.0),
      ambient_material(vec3(0., 0., 0.)),
      diffuse_material(vec3(1., 1., 1.)),
      specular_material(vec3(0., 0., 0.)),
      shininess(32.0),
      emissive(false),
      light_color(vec3(1., 1., 1.)),
      light_intensity(1.0),
      image(),
      normals(),
      emissionMap(),
      metalicityMap(),
      texture_scale_x(1.0),
      texture_scale_y(1.0),
      has_normal_map(false)
{
}

// Constructor with texture only
Material::Material(const std::string &pathFileTexture) : Material()
{
    ppmLoader::load_ppm(image, pathFileTexture); // load texture and set result to image
    this->pathFileTexture = pathFileTexture;
    
    // If texture loading failed, ensure image dimensions are 0
    if (image.data.empty()) {
        image.w = 0;
        image.h = 0;
    }
}

// Constructor with texture and normal map
Material::Material(const std::string &pathFileTexture, const std::string &pathFileNormalMap) : Material()
{
    ppmLoader::load_ppm(image, pathFileTexture);     // load texture and set result to image
    ppmLoader::load_ppm(normals, pathFileNormalMap); // load normal map and set result to normals
    
    // If texture loading failed, ensure image dimensions are 0
    if (image.data.empty()) {
        image.w = 0;
        image.h = 0;
        has_texture = false;
    }else{
        has_texture = true;
    }
    
    // If normal map loading failed or no data, disable normal map
    if (normals.data.empty()) {
        normals.w = 0;
        normals.h = 0;
        has_normal_map = false;
    } else {
        has_normal_map = true;
    }
    
    this->pathFileTexture = pathFileTexture;
    this->pathFileNormalMap = pathFileNormalMap;
}

// Constructor for only diffuse color
Material::Material(const vec3 &diffuse_color)
    : transparency(0.0),
      index_medium(1.0),
      ambient_material(vec3(0., 0., 0.)),
      diffuse_material(diffuse_color),
      specular_material(vec3(0., 0., 0.)),
      shininess(32.0),
      emissive(false),
      light_color(vec3(1., 1., 1.)),
      light_intensity(1.0),
      image(),
      normals(),
      emissionMap(),
      metalicityMap(),
      texture_scale_x(1.0),
      texture_scale_y(1.0),
      has_normal_map(false)
{
}

GPUMaterial Material::toGPU() const
{
    GPUMaterial gpuMat;
    
    // Ambient
    gpuMat.ambient.x = ambient_material.x;
    gpuMat.ambient.y = ambient_material.y;
    gpuMat.ambient.z = ambient_material.z;
    gpuMat.ambient._padding = 0.0f;
    
    // Diffuse
    gpuMat.diffuse.x = diffuse_material.x;
    gpuMat.diffuse.y = diffuse_material.y;
    gpuMat.diffuse.z = diffuse_material.z;
    gpuMat.diffuse._padding = 0.0f;
    
    // Specular
    gpuMat.specular.x = specular_material.x;
    gpuMat.specular.y = specular_material.y;
    gpuMat.specular.z = specular_material.z;
    gpuMat.specular._padding = 0.0f;
    
    // Material properties
    gpuMat.shininess = static_cast<float>(shininess);
    gpuMat.index_medium = index_medium;
    gpuMat.transparency = transparency;
    gpuMat.texture_scale_x = texture_scale_x;
    
    gpuMat.texture_scale_y = texture_scale_y;
    gpuMat.emissive = emissive ? 1 : 0;
    gpuMat._padding1[0] = 0.0f;
    gpuMat._padding1[1] = 0.0f;
    
    gpuMat.light_color.x = light_color.x;
    gpuMat.light_color.y = light_color.y;
    gpuMat.light_color.z = light_color.z;
    gpuMat.light_color._padding = 0.0f;
    
    gpuMat.light_intensity = light_intensity;
    
    // Texture properties
    gpuMat.has_texture = (!image.data.empty()) ? 1 : 0;
    gpuMat.has_normal_map = has_normal_map ? 1 : 0;
    gpuMat.texture_width = image.w;
    gpuMat.texture_height = image.h;
    
    // Texture offsets will be set by RenderEngine when building the texture buffer
    gpuMat.texture_offset = 0;
    gpuMat.normal_map_offset = 0;
    
    // Material ID
    gpuMat.material_id = material_id;

    return gpuMat;
}