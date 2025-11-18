#pragma once
#include "vec3.h"
#include <cmath>
#include <iostream>

class Mat4
{
public:
    // Constructeur par défaut (matrice identité)
    Mat4()
    {
        for (int i = 0; i < 16; ++i)
            vals[i] = 0.0f;
        vals[0] = vals[5] = vals[10] = vals[15] = 1.0f;
    }

    // Constructeur avec tous les éléments
    Mat4(float v0, float v1, float v2, float v3,
         float v4, float v5, float v6, float v7,
         float v8, float v9, float v10, float v11,
         float v12, float v13, float v14, float v15)
    {
        vals[0] = v0;   vals[1] = v1;   vals[2] = v2;   vals[3] = v3;
        vals[4] = v4;   vals[5] = v5;   vals[6] = v6;   vals[7] = v7;
        vals[8] = v8;   vals[9] = v9;   vals[10] = v10; vals[11] = v11;
        vals[12] = v12; vals[13] = v13; vals[14] = v14; vals[15] = v15;
    }

    // Constructeur de copie
    Mat4(const Mat4 &m)
    {
        for (int i = 0; i < 16; ++i)
            vals[i] = m.vals[i];
    }

    ////////  ACCÈS AUX ÉLÉMENTS  /////////
    float operator()(unsigned int i, unsigned int j) const { return vals[4 * i + j]; }
    float &operator()(unsigned int i, unsigned int j) { return vals[4 * i + j]; }

    ////////  MULTIPLICATION MATRICE * VEC3  /////////
    vec3 operator*(const vec3 &p) const
    {
        float w = (*this)(3, 0) * p.x + (*this)(3, 1) * p.y + (*this)(3, 2) * p.z + (*this)(3, 3);
        if (w == 0.0f) w = 1.0f; // Éviter division par zéro
        
        return vec3(
            ((*this)(0, 0) * p.x + (*this)(0, 1) * p.y + (*this)(0, 2) * p.z + (*this)(0, 3)) / w,
            ((*this)(1, 0) * p.x + (*this)(1, 1) * p.y + (*this)(1, 2) * p.z + (*this)(1, 3)) / w,
            ((*this)(2, 0) * p.x + (*this)(2, 1) * p.y + (*this)(2, 2) * p.z + (*this)(2, 3)) / w
        );
    }

    ////////  MULTIPLICATION MATRICE * MATRICE  /////////
    Mat4 operator*(const Mat4 &m2) const
    {
        Mat4 res;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                res(i, j) = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    res(i, j) += (*this)(i, k) * m2(k, j);
                }
            }
        }
        return res;
    }

    ////////  OPÉRATEURS  /////////
    void operator=(const Mat4 &m)
    {
        for (int i = 0; i < 16; ++i)
            vals[i] = m.vals[i];
    }

    Mat4 operator+(const Mat4 &m2) const
    {
        Mat4 res;
        for (int i = 0; i < 16; ++i)
            res.vals[i] = vals[i] + m2.vals[i];
        return res;
    }

    Mat4 operator-(const Mat4 &m2) const
    {
        Mat4 res;
        for (int i = 0; i < 16; ++i)
            res.vals[i] = vals[i] - m2.vals[i];
        return res;
    }

    Mat4 operator*(float s) const
    {
        Mat4 res;
        for (int i = 0; i < 16; ++i)
            res.vals[i] = vals[i] * s;
        return res;
    }

    ////////  MATRICES DE TRANSFORMATION  /////////
    
    // Matrice de translation
    static Mat4 translation(const vec3& t)
    {
        return Mat4(
            1, 0, 0, t.x,
            0, 1, 0, t.y,
            0, 0, 1, t.z,
            0, 0, 0, 1
        );
    }

    // Matrice de scale
    static Mat4 scale(const vec3& s)
    {
        return Mat4(
            s.x, 0,   0,   0,
            0,   s.y, 0,   0,
            0,   0,   s.z, 0,
            0,   0,   0,   1
        );
    }

    // Rotation autour de X (angle en radians)
    static Mat4 rotationX(float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return Mat4(
            1, 0,  0, 0,
            0, c, -s, 0,
            0, s,  c, 0,
            0, 0,  0, 1
        );
    }

    // Rotation autour de Y (angle en radians)
    static Mat4 rotationY(float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return Mat4(
            c,  0, s, 0,
            0,  1, 0, 0,
            -s, 0, c, 0,
            0,  0, 0, 1
        );
    }

    // Rotation autour de Z (angle en radians)
    static Mat4 rotationZ(float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return Mat4(
            c, -s, 0, 0,
            s,  c, 0, 0,
            0,  0, 1, 0,
            0,  0, 0, 1
        );
    }

    ////////  TRANSPOSE  /////////
    Mat4 getTranspose() const
    {
        Mat4 res;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                res(j, i) = (*this)(i, j);
        return res;
    }

private:
    float vals[16];
    // Organisation:
    // 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
};

// Opérateur de multiplication scalaire à gauche
inline Mat4 operator*(float s, const Mat4 &m)
{
    return m * s;
}

// Opérateur de sortie
inline std::ostream &operator<<(std::ostream &s, const Mat4 &m)
{
    s << m(0, 0) << "\t" << m(0, 1) << "\t" << m(0, 2) << "\t" << m(0, 3) << std::endl
      << m(1, 0) << "\t" << m(1, 1) << "\t" << m(1, 2) << "\t" << m(1, 3) << std::endl
      << m(2, 0) << "\t" << m(2, 1) << "\t" << m(2, 2) << "\t" << m(2, 3) << std::endl
      << m(3, 0) << "\t" << m(3, 1) << "\t" << m(3, 2) << "\t" << m(3, 3) << std::endl;
    return s;
}