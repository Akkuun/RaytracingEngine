#pragma once

#include "../utils/imageLoader/ImageLoader.h"
#include "../math/vec3.h"
#include "../defines/Defines.h"
#include "../../../external/json/single_include/nlohmann/json.hpp"
#include <QString>

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
    Material(const nlohmann::json &j);
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
    inline float getMetalness() const { return metalness; }
    inline bool isEmissive() const { return emissive; }
    inline const vec3 &getLightColor() const { return light_color; }
    inline float getLightIntensity() const { return light_intensity; }
    inline bool hasNormalMap() const { return has_normal_map; }
    inline bool hasEmissiveMap() const { return has_emissive_map; }
    inline bool hasMetalMap() const { return has_metal_map; }
    inline bool hasMetallicMap() const { return has_metal_map; }
    inline const ppmLoader::ImageRGB &getImage() const { return image; }
    inline const ppmLoader::ImageRGB &getNormals() const { return normals; }
    inline const ppmLoader::ImageRGB &getMetallic() const { return metalicityMap; }
    inline const ppmLoader::ImageRGB &getEmissive() const { return emissionMap; }
    inline void setDiffuseFromRGB(int r, int g, int b)
    {
        float fr = r / 255.0f;
        float fg = g / 255.0f;
        float fb = b / 255.0f;
        diffuse_material = vec3(fr, fg, fb);
    }
    inline int getMaterialId() const { return material_id; }
    inline bool hasTexture() const { return  has_texture; }
    inline std::string getPathFileTexture() const { return pathFileTexture; }
    inline std::string getPathFileNormalMap() const { return pathFileNormalMap; }
    inline std::string getPathFileMetalMap() const { return pathFileMetalMap; }
    inline std::string getPathFileEmissiveMap() const { return pathFileEmissiveMap; }
    inline float getTextureScaleX() const { return texture_scale_x; }
    inline float getTextureScaleY() const { return texture_scale_y; }
    // Setters
    inline void setAmbient(const vec3 &v) { ambient_material = v; }
    inline void setDiffuse(const vec3 &v) { diffuse_material = v; }
    inline void setSpecular(const vec3 &v) { specular_material = v; }
    inline void setShininess(double s) { shininess = s; }
    inline void setIndexMedium(float i) { index_medium = i; }
    inline void setTransparency(float t) { transparency = t; }
    inline void setMetalness(float m) { metalness = m; }
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
        has_texture = false;
    }
    inline void removeNormals()
    {
        normals.data.clear();
        normals.w = 0;
        normals.h = 0;
        has_normal_map = false;
    }
    inline void setNormals(const ppmLoader::ImageRGB &img)
    {
        normals = img;
        has_normal_map = true;
    }
    inline void setNormalsFromPath(const std::string &path)
    {
        ppmLoader::load_ppm(normals, path);
        if (!normals.data.empty())
        {
            has_normal_map = true;
            pathFileNormalMap = path;
        }
    }
    inline void removeMetallic()
    {
        metalicityMap.data.clear();
        metalicityMap.w = 0;
        metalicityMap.h = 0;
        has_metal_map = false;
    }
    inline void removeEmissive()
    {
        emissionMap.data.clear();
        emissionMap.w = 0;
        emissionMap.h = 0;
        has_emissive_map = false;
    }
    inline void setMetallic(const ppmLoader::ImageRGB &img)
    {
        metalicityMap = img;
        has_metal_map = true;
    }
    inline void setMetallicFromPath(const std::string &path)
    {
        ppmLoader::load_ppm(metalicityMap, path);
        if (!metalicityMap.data.empty())
        {
            has_metal_map = true;
            pathFileMetalMap = path;
        }
    }
    inline void setEmissive(const ppmLoader::ImageRGB &img)
    {
        emissionMap = img;
        has_emissive_map = true;
    }
    inline void setEmissiveFromPath(const std::string &path)
    {
        ppmLoader::load_ppm(emissionMap, path);
        if (!emissionMap.data.empty())
        {
            has_emissive_map = true;
            pathFileEmissiveMap = path;
        }
    }


    void setPathFileTexture(const std::string &path) { pathFileTexture = path; }
    void setPathFileNormalMap(const std::string &path) { pathFileNormalMap = path; }
    void setPathFileEmissiveMap(const std::string &path) { pathFileEmissiveMap = path; }
    void setPathFileMetalMap(const std::string &path) { pathFileMetalMap = path; }
    void setHasNormalMap(bool has) { has_normal_map = has; }
    void setHasMetalMap(bool has) { has_metal_map = has; }
    void setHasEmissiveMap(bool has) { has_emissive_map = has; }

    GPUMaterial toGPU() const;

private:
    float transparency = 0.;
    float index_medium;
    vec3 ambient_material;
    vec3 diffuse_material = vec3(0.0f);
    vec3 specular_material;
    double shininess;
    bool emissive;
    float metalness;
    vec3 light_color;
    float light_intensity = 1.0f;
    ppmLoader::ImageRGB image;
    ppmLoader::ImageRGB normals;
    ppmLoader::ImageRGB emissionMap;
    ppmLoader::ImageRGB metalicityMap;
    float texture_scale_x = 1.;
    float texture_scale_y = 1.;
    bool has_normal_map = false;
    bool has_texture = false;
    bool has_emissive_map = false;
    bool has_metal_map = false;
    int material_id = MaterialId::getInstance().getNewId();
    std::string pathFileTexture = "";
    std::string pathFileNormalMap = "";
    std::string pathFileEmissiveMap = "";
    std::string pathFileMetalMap = "";
};