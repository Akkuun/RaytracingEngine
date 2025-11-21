#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

enum InterPolationType
{
    LINEAR,
    COS,
    EXP,
    LOG,
    SQUARE,
    SQRT,
    CUBIC,
    SQRT3
};

class Camera_Helper
{
public:
    static glm::vec3 quatToEuler(const glm::quat &quat);
    static void computeFinalView(glm::mat4 &outProjectionMatrix, glm::mat4 &outviewMatrix,
                                 const glm::vec3 &position, const glm::quat &rotation,
                                 float &fovDegree, float &near, float &far);
    static glm::vec3 projectVectorOnPlan(const glm::vec3 &vector, const glm::vec3 &normal);
    static double clipAnglePI(const double &angle);
    static double clamp(const double &value, const double &min, const double &max);
    static double interpolation(const double &ratio, const InterPolationType &type);
};
