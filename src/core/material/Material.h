#pragma once

#include "../utils/imageLoader/ImageLoader.h"
#include "../math/vec3.h"
#include "../defines/Defines.h"

class Material
{
public:
    Material();
    Material(const std::string& pathFileTexture);
    Material(const std::string& pathFileTexture, const std::string& pathFileNormalMap);
    Material(const vec3 &diffuse_color);

    // Getters
    inline const vec3 &getAmbient() const { return ambient_material; }
    inline const vec3 &getDiffuse() const { return diffuse_material; }
    inline const vec3 &getSpecular() const { return specular_material; }
    inline double getShininess() const { return shininess; }
    inline float getIndexMedium() const { return index_medium; }
    inline float getTransparency() const { return transparency; }
    inline bool isEmissive() const { return emissive; }
    inline const vec3 &getLightColor() const { return light_color; }
    inline float getLightIntensity() const { return light_intensity; }
    inline bool hasNormalMap() const { return has_normal_map; }
    inline const ppmLoader::ImageRGB &getImage() const { return image; }
    inline const ppmLoader::ImageRGB &getNormals() const { return normals; }

    // Setters
    inline void setAmbient(const vec3 &v) { ambient_material = v; }
    inline void setDiffuse(const vec3 &v) { diffuse_material = v; }
    inline void setSpecular(const vec3 &v) { specular_material = v; }
    inline void setShininess(double s) { shininess = s; }
    inline void setIndexMedium(float i) { index_medium = i; }
    inline void setTransparency(float t) { transparency = t; }
    inline void setEmissive(bool e) { emissive = e; }
    inline void setLightColor(const vec3 &c) { light_color = c; }
    inline void setLightIntensity(float i) { light_intensity = i; }
    inline void setTextureScale(float sx, float sy)
    {
        texture_scale_x = sx;
        texture_scale_y = sy;
    }

    inline void set_texture(const ppmLoader::ImageRGB &img){
        image = img;
    }
    inline void set_normals(const ppmLoader::ImageRGB &img){
        normals = img;
        has_normal_map = true;
    }


    GPUMaterial toGPU() const;

    // bool operator==(const Material& other) const {
    //     return
    //         // if ambient mat is ==
    //         ambient_material.x == other.ambient_material.x &&
    //         ambient_material.y == other.ambient_material.y &&
    //         ambient_material.z == other.ambient_material.z &&
    //         // if diffuse mat is ==
    //         diffuse_material.x == other.diffuse_material.x &&
    //         diffuse_material.y == other.diffuse_material.y &&
    //         diffuse_material.z == other.diffuse_material.z &&
    //         // if specular mat is ==
    //         specular_material.x == other.specular_material.x &&
    //         specular_material.y == other.specular_material.y &&
    //         specular_material.z == other.specular_material.z &&
    //         // if texture scales are ==
    //         texture_scale_x == other.texture_scale_x &&
    //         texture_scale_y == other.texture_scale_y &&
    //         // if other properties are ==
    //         shininess == other.shininess &&
    //         index_medium == other.index_medium &&
    //         transparency == other.transparency &&
    //         emissive == other.emissive &&
    //         light_color.x == other.light_color.x &&
    //         light_color.y == other.light_color.y &&
    //         light_color.z == other.light_color.z &&
    //         light_intensity == other.light_intensity &&
    //         // image/normal map comparaison (only check paths) to avoid checking pointer comparison
    //         pathFileTexture == other.pathFileTexture &&
    //         pathFileNormalMap == other.pathFileNormalMap;
    // }

private:
    vec3 ambient_material;
    vec3 diffuse_material;
    vec3 specular_material;
    double shininess;

    float index_medium;
    float transparency = 0.;


    float texture_scale_x = 1.;
    float texture_scale_y = 1.;

    bool emissive;
    vec3 light_color;
    float light_intensity;

    ppmLoader::ImageRGB &image;
    ppmLoader::ImageRGB &normals;
    ppmLoader::ImageRGB &emissionMap;
    ppmLoader::ImageRGB &metalicityMap;

    bool has_normal_map = false;


    std::vector<int> shapeIDAssociated;



    // used for == verification
    std::string pathFileTexture = "";
    std::string pathFileNormalMap = "";
};