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
    if (image.data.empty())
    {
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
    if (image.data.empty())
    {
        image.w = 0;
        image.h = 0;
        has_texture = false;
    }
    else
    {
        has_texture = true;
    }

    // If normal map loading failed or no data, disable normal map
    if (normals.data.empty())
    {
        normals.w = 0;
        normals.h = 0;
        has_normal_map = false;
    }
    else
    {
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

/*
struct __attribute__((aligned(16))) GPUMaterial {
    Vec3 ambient;              // 16 bytes (offset 0)
    Vec3 diffuse;              // 16 bytes (offset 16)
    Vec3 specular;             // 16 bytes (offset 32)
    
    float shininess;           // 4 bytes (offset 48)
    float index_medium;        // 4 bytes (offset 52)
    float transparency;        // 4 bytes (offset 56)
    float texture_scale_x;     // 4 bytes (offset 60)
    
    float texture_scale_y;     // 4 bytes (offset 64)
    int emissive;              // 4 bytes (offset 68)
    float metalness;           // 4 bytes (offset 72)

    Vec3 light_color;          // 16 bytes (offset 76)
    float light_intensity;     // 4 bytes (offset 92)

    int has_texture;           // 4 bytes (offset 96)
    int texture_width;         // 4 bytes (offset 100)
    int texture_height;        // 4 bytes (offset 104)
    int texture_offset;        // 4 bytes (offset 108)

    int has_normal_map;        // 4 bytes (offset 112)
    int normal_map_width;      // 4 bytes (offset 116)
    int normal_map_height;     // 4 bytes (offset 120)
    int normal_map_offset;     // 4 bytes (offset 124) 

    int has_metal_map;         // 4 bytes (offset 128)
    int metal_map_width;       // 4 bytes (offset 132)
    int metal_map_height;      // 4 bytes (offset 136)
    int metal_map_offset;      // 4 bytes (offset 140)

    int has_emissive_map;      // 4 bytes (offset 144)
    int emissive_map_width;    // 4 bytes (offset 148)
    int emissive_map_height;   // 4 bytes (offset 152)
    int emissive_map_offset;   // 4 bytes (offset 156)

    int material_id;           // 4 bytes (offset 160)
};  // Total: 160 bytes
*/

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
    gpuMat.metalness = 1.0f; // Default metalness to 0.0f

    // Light properties
    gpuMat.light_color.x = light_color.x;
    gpuMat.light_color.y = light_color.y;
    gpuMat.light_color.z = light_color.z;
    gpuMat.light_color._padding = 0.0f;
    gpuMat.light_intensity = light_intensity;

    // Texture properties
    gpuMat.has_texture = (!image.data.empty()) ? 1 : 0;
    gpuMat.texture_width = image.w;
    gpuMat.texture_height = image.h;
    gpuMat.texture_offset = 0;

    // Texture offsets will be set by RenderEngine when building the texture buffer

    gpuMat.has_normal_map = has_normal_map ? 1 : 0;
    gpuMat.normal_map_width = normals.w;
    gpuMat.normal_map_height = normals.h;
    gpuMat.normal_map_offset = 0;

    gpuMat.has_metal_map = (!metalicityMap.data.empty()) ? 1 : 0;
    gpuMat.metal_map_height = metalicityMap.h;
    gpuMat.metal_map_width = metalicityMap.w;
    gpuMat.metal_map_offset = 0;

    gpuMat.has_emissive_map = (!emissionMap.data.empty()) ? 1 : 0;
    gpuMat.emissive_map_height = emissionMap.h;
    gpuMat.emissive_map_width = emissionMap.w;
    gpuMat.emissive_map_offset = 0;

    // Material ID
    gpuMat.material_id = material_id;

    return gpuMat;
}

Material::Material(const nlohmann::json &j) : Material()
{
    if (j.contains("ambient"))
    {
        ambient_material = vec3(j["ambient"][0], j["ambient"][1], j["ambient"][2]);
    }
    if (j.contains("diffuse"))
    {
        diffuse_material = vec3(j["diffuse"][0], j["diffuse"][1], j["diffuse"][2]);
    }
    if (j.contains("specular"))
    {
        specular_material = vec3(j["specular"][0], j["specular"][1], j["specular"][2]);
    }
    if (j.contains("shininess"))
    {
        shininess = j["shininess"];
    }
    if (j.contains("transparency"))
    {
        transparency = j["transparency"];
    }
    if (j.contains("index_medium"))
    {
        index_medium = j["index_medium"];
    }
    if (j.contains("emissive"))
    {
        emissive = j["emissive"];
    }
    if (j.contains("light_color"))
    {
        light_color = vec3(j["light_color"][0], j["light_color"][1], j["light_color"][2]);
    }
    if (j.contains("light_intensity"))
    {
        light_intensity = j["light_intensity"];
    }
    if (j.contains("has_normal_map"))
    {
        has_normal_map = j["has_normal_map"];
    }
    if (j.contains("texture_scale_x"))
    {
        texture_scale_x = j["texture_scale_x"];
    }
    if (j.contains("texture_scale_y"))
    {
        texture_scale_y = j["texture_scale_y"];
    }
    if (j.contains("material_id"))
    {
        material_id = j["material_id"];
    }
    if (j.contains("texture"))
    {
        std::string texturePath = j["texture"];
        if (!texturePath.empty())
        {
            ppmLoader::load_ppm(image, texturePath);
            this->pathFileTexture = texturePath;
            has_texture = !image.data.empty();
        }
        else
        {
            has_texture = false;
        }
    }
    if (j.contains("normal_map"))
    {
        std::string normalMapPath = j["normal_map"];
        if (!normalMapPath.empty())
        {
            ppmLoader::load_ppm(normals, normalMapPath);
            this->pathFileNormalMap = normalMapPath;
            has_normal_map = !normals.data.empty();
        }
        else
        {
            has_normal_map = false;
        }
    }
}