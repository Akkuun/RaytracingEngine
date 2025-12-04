#include "Camera_Helper.h"
#include <cmath>

glm::vec3 Camera_Helper::quatToEuler(const glm::quat& quat)
{
    // Opengl quat to euler function give yaw between -90 and 90
    // If you want correct pitch and yaw you can use this 
    // Src http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
    // But roll will be between -90 and 90 here

    glm::vec3 eulerAngles;

    float test = quat.x * quat.y + quat.z * quat.w;

    if (test > 0.499f) { // singularity at north pole
        eulerAngles.y = 2.f * atan2(quat.x, quat.w);
        eulerAngles.z = M_PI / 2.f;
        eulerAngles.x = 0.f;
        return eulerAngles;
    }
    if (test < -0.499f) { // singularity at south pole
        eulerAngles.y = -2.f * atan2(quat.x, quat.w);
        eulerAngles.z = -M_PI / 2.f;
        eulerAngles.x = 0.f;
        return eulerAngles;
    }
    float sqx = quat.x * quat.x;
    float sqy = quat.y * quat.y;
    float sqz = quat.z * quat.z;

    eulerAngles.y = atan2(2.f * quat.y * quat.w - 2.f * quat.x * quat.z, 1.f - 2.f * sqy - 2.f * sqz);
    eulerAngles.z = asin(2.f * test);
    eulerAngles.x = atan2(2.f * quat.x * quat.w - 2.f * quat.y * quat.z, 1.f - 2.f * sqx - 2.f * sqz);

    return eulerAngles;
}

void Camera_Helper::computeFinalView(glm::mat4& outProjectionMatrix, glm::mat4& outviewMatrix, 
                                      const glm::vec3& position, const glm::quat& rotation, 
                                      float &fovDegree, float &near, float &far)
{
    // Projection matrix : FOV, 16:9 ratio, display range : near <-> far units
    outProjectionMatrix = glm::perspective(glm::radians(fovDegree), 16.0f / 9.0f, near, far);

    const glm::vec3 front = normalize(rotation * glm::vec3(0.f, 0.f, 1.f));
    const glm::vec3 up = normalize(rotation * glm::vec3(0.f, 1.f, 0.f));

    // Camera matrix
    outviewMatrix = glm::lookAt(position, position + front, up);
}

glm::vec3 Camera_Helper::projectVectorOnPlan(const glm::vec3& vector, const glm::vec3& normal) {
    return vector - glm::dot(vector, normal) * normal;
}

double Camera_Helper::clipAnglePI(const double &angle) {
    double clippedAngle = angle;
    while(clippedAngle < -M_PI) {
        clippedAngle += 2*M_PI;
    }
    while(clippedAngle > M_PI) {
        clippedAngle -= 2*M_PI;
    }
    return clippedAngle;
}

double Camera_Helper::clamp(const double &value, const double &min, const double &max) {
    if(value < min) {
        return min;
    }
    if(value > max) {
        return max;
    }
    return value;
}

double Camera_Helper::interpolation(const double &ratio, const InterPolationType &type) {
    switch(type) {
        case LINEAR:
            return ratio;
        case COS:
            return 0.5 - 0.5 * cos(ratio * M_PI);
        case EXP:
            return (exp(ratio)-1) / (exp(1)-1);
        case LOG:
            return log(ratio + 1) / log(2);
        case SQRT:
            return sqrt(ratio);
        case CUBIC:
            return ratio * ratio * ratio;
        case SQRT3:
            return pow(ratio, 1. / 3.);
    }
    return 0;
}
