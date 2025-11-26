#pragma once

#include "../utils/imageLoader/ImageLoader.h"
#include "../math/vec3.h"
#include "../defines/Defines.h"

class MaterialId
{
public:
    static MaterialId &getInstance()
    {
        static MaterialId instance;
        return instance;
    }

private:
    int id = 0;
    MaterialId() {}

public:
    int getNewId() { return id++; }
};

class Material
{
public:
    Material();
    Material(const std::string &pathFileTexture);
    Material(const std::string &pathFileTexture, const std::string &pathFileNormalMap);
    Material(const vec3 &diffuse_color);
    ~Material()
    {
        image.data.clear();
        normals.data.clear();
        emissionMap.data.clear();
        metalicityMap.data.clear();
    }

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
    inline int getMaterialId() const { return material_id; }
    inline bool hasTexture() const { return  has_texture; }
    inline std::string getPathFileTexture() const { return pathFileTexture; }
    inline std::string getPathFileNormalMap() const { return pathFileNormalMap; }
    inline float getTextureScaleX() const { return texture_scale_x; }
    inline float getTextureScaleY() const { return texture_scale_y; }
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

    inline void set_texture(const ppmLoader::ImageRGB &img)
    {
        image = img;
    }

    inline void remove_texture()
    {
        image.data.clear();
        image.w = 0;
        image.h = 0;
    }
    inline void set_normals(const ppmLoader::ImageRGB &img)
    {
        normals = img;
        has_normal_map = true;
    }

    GPUMaterial toGPU() const;

private:
    float transparency = 0.;
    float index_medium;
    vec3 ambient_material;
    vec3 diffuse_material;
    vec3 specular_material;
    double shininess;
    bool emissive;
    vec3 light_color;
    float light_intensity;
    ppmLoader::ImageRGB image;
    ppmLoader::ImageRGB normals;
    ppmLoader::ImageRGB emissionMap;
    ppmLoader::ImageRGB metalicityMap;
    float texture_scale_x = 1.;
    float texture_scale_y = 1.;
    bool has_normal_map = false;
    bool has_texture = false;
    int material_id = MaterialId::getInstance().getNewId();
    std::string pathFileTexture = "";
    std::string pathFileNormalMap = "";
};