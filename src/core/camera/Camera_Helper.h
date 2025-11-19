#ifndef C1D9E61C_80B1_4BF9_9107_36B9C5CA497D
#define C1D9E61C_80B1_4BF9_9107_36B9C5CA497D

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

enum InterPolationType
{
    LINEAR, COS, EXP, LOG, SQUARE, SQRT, CUBIC, SQRT3
};

class Camera_Helper
{
public: 
    static glm::vec3 quatToEuler(glm::quat quat);
    static void computeFinalView(glm::mat4& outProjectionMatrix, glm::mat4& outviewMatrix, 
                                  glm::vec3& position, glm::quat rotation, 
                                  float fovDegree, float near, float far);
    static glm::vec3 projectVectorOnPlan(glm::vec3 vector, glm::vec3 normal);
    static double clipAnglePI(double angle);
    static double clamp(double value, double min, double max);
    static double interpolation(double ratio, InterPolationType type);
};


#endif /* C1D9E61C_80B1_4BF9_9107_36B9C5CA497D */
