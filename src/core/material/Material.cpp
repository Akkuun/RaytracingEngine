#include "Material.h"
#include "../utils/imageLoader/ImageLoader.h"
#include "../math/vec3.h"

// constructors with default values
Material::Material()
    : transparency(0.0),
      index_medium(1.0),
      ambient_material(vec3(0., 0., 0.)),
      image(*(new ppmLoader::ImageRGB())),
      normals(*(new ppmLoader::ImageRGB())),
      emissionMap(*(new ppmLoader::ImageRGB())),
      metalicityMap(*(new ppmLoader::ImageRGB())),
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
}

// Constructor with texture and normal map
Material::Material(const std::string &pathFileTexture, const std::string &pathFileNormalMap) : Material()
{
    ppmLoader::load_ppm(image, pathFileTexture);     // load texture and set result to image
    ppmLoader::load_ppm(normals, pathFileNormalMap); // load normal map and set result to normals
    has_normal_map = true;
    this->pathFileTexture = pathFileTexture;
    this->pathFileNormalMap = pathFileNormalMap;
}

// Constructor for only diffuse color
Material::Material(const vec3 &diffuse_color)
    : transparency(0.0),
      index_medium(1.0),
      ambient_material(vec3(0., 0., 0.)),
      diffuse_material(diffuse_color),
      image(*(new ppmLoader::ImageRGB())),
      normals(*(new ppmLoader::ImageRGB())),
      emissionMap(*(new ppmLoader::ImageRGB())),
      metalicityMap(*(new ppmLoader::ImageRGB())),
      texture_scale_x(1.0),
      texture_scale_y(1.0),
      has_normal_map(false)
{
}

GPUMaterial Material::toGPU() const
{
    GPUMaterial gpuMat;
    gpuMat.diffuse.x = diffuse_material.x;
    gpuMat.diffuse.y = diffuse_material.y;
    gpuMat.diffuse.z = diffuse_material.z;

    return gpuMat;
}