// filepath: /home/mathis/Programming/RaytracingEngine/src/core/math/transformUtils.h
#pragma once

#include "vec3.h"
#include "mat4.h"
#include "mat3.h"

class TransformUtils
{
public:
    // Transform matrix -> Scale → Rotate (Z→Y→X) → Translate
    static Mat4 createTransformMatrix(const vec3& position, const vec3& rotation, const vec3& scale)
    {
        Mat4 T = Mat4::translation(position);
        Mat4 Rx = Mat4::rotationX(rotation.x);
        Mat4 Ry = Mat4::rotationY(rotation.y);
        Mat4 Rz = Mat4::rotationZ(rotation.z);
        Mat4 S = Mat4::scale(scale);
        return T * Rz * Ry * Rx * S;
    }

    // Apply full transform to a point: Scale → Rotate (Z→Y→X) → Translate
    static vec3 applyFullTransform(const vec3& point, const vec3& position, const vec3& rotation, const vec3& scale)
    {
        Mat4 transform = createTransformMatrix(position, rotation, scale);
        return transform * point;
    }

    // Apply rotation only to a vector (no translation or scale)
    static vec3 applyRotation(const vec3& vector, const vec3& rotation)
    {
        Mat3 Rx = Mat3::rotationX(rotation.x);
        Mat3 Ry = Mat3::rotationY(rotation.y);
        Mat3 Rz = Mat3::rotationZ(rotation.z);

        // Order: Z → Y → X
        return Rz * (Ry * (Rx * vector));
    }

    // Apply rotation and scale to a vector (no translation)
    static vec3 applyRotationAndScale(const vec3& vector, const vec3& rotation, const vec3& scale)
    {
        // First scale
        vec3 scaled = vec3(vector.x * scale.x, vector.y * scale.y, vector.z * scale.z);

        // Then rotation
        return applyRotation(scaled, rotation);
    }

    // rotation degree to radian
    static float degToRad(float degrees)
    {
        return degrees * M_PI / 180.0f;
    }

    // radian to rotation degree
    static float radToDeg(float radians)
    {
        return radians * 180.0f / M_PI;
    }

    // vec3 version of degToRad
    static vec3 degToRad(const vec3& degrees)
    {
        return vec3(degToRad(degrees.x), degToRad(degrees.y), degToRad(degrees.z));
    }
};