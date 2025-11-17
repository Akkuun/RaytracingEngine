#include "vec3.h"
class Mat3
{
public:
    Mat3()
    {
        vals[0] = 0;
        vals[1] = 0;
        vals[2] = 0;
        vals[3] = 0;
        vals[4] = 0;
        vals[5] = 0;
        vals[6] = 0;
        vals[7] = 0;
        vals[8] = 0;
    }

    Mat3(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9)
    {
        vals[0] = v1;
        vals[1] = v2;
        vals[2] = v3;
        vals[3] = v4;
        vals[4] = v5;
        vals[5] = v6;
        vals[6] = v7;
        vals[7] = v8;
        vals[8] = v9;
    }

    Mat3(const Mat3 &m)
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                (*this)(i, j) = m(i, j);
    }

    // Multiplication de matrice avec un Vec3 : m.p
    //--> application d'un matrice de rotation à un point ou un vecteur
    vec3 operator*(const vec3 &p)
    {
        // Pour acceder a un element de la matrice (*this)(i,j) et du point p[i]
        vec3 res = vec3(
            (*this)(0, 0) * p[0] + (*this)(0, 1) * p[1] + (*this)(0, 2) * p[2],
            (*this)(1, 0) * p[0] + (*this)(1, 1) * p[1] + (*this)(1, 2) * p[2],
            (*this)(2, 0) * p[0] + (*this)(2, 1) * p[1] + (*this)(2, 2) * p[2]);
        return res;
    }

    Mat3 operator*(const Mat3 &m2)
    { // calcul du produit matriciel m1.m2
        // Pour acceder a un element de la premiere matrice (*this)(i,j) et de la deuxième m2(k,l)
        Mat3 res = Mat3(
            (*this)(0, 0) * m2(0, 0) + (*this)(0, 1) * m2(1, 0) + (*this)(0, 2) * m2(2, 0),
            (*this)(0, 0) * m2(0, 1) + (*this)(0, 1) * m2(1, 1) + (*this)(0, 2) * m2(2, 1),
            (*this)(0, 0) * m2(0, 2) + (*this)(0, 1) * m2(1, 2) + (*this)(0, 2) * m2(2, 2),
            (*this)(1, 0) * m2(0, 0) + (*this)(1, 1) * m2(1, 0) + (*this)(1, 2) * m2(2, 0),
            (*this)(1, 0) * m2(0, 1) + (*this)(1, 1) * m2(1, 1) + (*this)(1, 2) * m2(2, 1),
            (*this)(1, 0) * m2(0, 2) + (*this)(1, 1) * m2(1, 2) + (*this)(1, 2) * m2(2, 2),
            (*this)(2, 0) * m2(0, 0) + (*this)(2, 1) * m2(1, 0) + (*this)(2, 2) * m2(2, 0),
            (*this)(2, 0) * m2(0, 1) + (*this)(2, 1) * m2(1, 1) + (*this)(2, 2) * m2(2, 1),
            (*this)(2, 0) * m2(0, 2) + (*this)(2, 1) * m2(1, 2) + (*this)(2, 2) * m2(2, 2));
        return res;
    }

    bool isnan() const
    {
        return std::isnan(vals[0]) || std::isnan(vals[1]) || std::isnan(vals[2]) || std::isnan(vals[3]) || std::isnan(vals[4]) || std::isnan(vals[5]) || std::isnan(vals[6]) || std::isnan(vals[7]) || std::isnan(vals[8]);
    }

    void operator=(const Mat3 &m)
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                (*this)(i, j) = m(i, j);
    }

    void operator+=(const Mat3 &m)
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                (*this)(i, j) += m(i, j);
    }

    void operator-=(const Mat3 &m)
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                (*this)(i, j) -= m(i, j);
    }

    void operator/=(double s)
    {
        for (unsigned int c = 0; c < 9; ++c)
            vals[c] /= s;
    }

    Mat3 operator-(const Mat3 &m2)
    {
        return Mat3((*this)(0, 0) - m2(0, 0), (*this)(0, 1) - m2(0, 1), (*this)(0, 2) - m2(0, 2),
                    (*this)(1, 0) - m2(1, 0), (*this)(1, 1) - m2(1, 1), (*this)(1, 2) - m2(1, 2),
                    (*this)(2, 0) - m2(2, 0), (*this)(2, 1) - m2(2, 1), (*this)(2, 2) - m2(2, 2));
    }

    Mat3 operator+(const Mat3 &m2)
    {
        return Mat3((*this)(0, 0) + m2(0, 0), (*this)(0, 1) + m2(0, 1), (*this)(0, 2) + m2(0, 2),
                    (*this)(1, 0) + m2(1, 0), (*this)(1, 1) + m2(1, 1), (*this)(1, 2) + m2(1, 2),
                    (*this)(2, 0) + m2(2, 0), (*this)(2, 1) + m2(2, 1), (*this)(2, 2) + m2(2, 2));
    }

    Mat3 operator/(float s)
    {
        return Mat3((*this)(0, 0) / s, (*this)(0, 1) / s, (*this)(0, 2) / s, (*this)(1, 0) / s, (*this)(1, 1) / s,
                    (*this)(1, 2) / s, (*this)(2, 0) / s, (*this)(2, 1) / s, (*this)(2, 2) / s);
    }

    Mat3 operator*(float s)
    {
        return Mat3((*this)(0, 0) * s, (*this)(0, 1) * s, (*this)(0, 2) * s, (*this)(1, 0) * s, (*this)(1, 1) * s,
                    (*this)(1, 2) * s, (*this)(2, 0) * s, (*this)(2, 1) * s, (*this)(2, 2) * s);
    }

    ////////        ACCESS TO COORDINATES      /////////
    float operator()(unsigned int i, unsigned int j) const { return vals[3 * i + j]; }

    float &operator()(unsigned int i, unsigned int j) { return vals[3 * i + j]; }

    ////////        BASICS       /////////
    inline float sqrnorm()
    {
        return vals[0] * vals[0] + vals[1] * vals[1] + vals[2] * vals[2] + vals[3] * vals[3] + vals[4] * vals[4] + vals[5] * vals[5] + vals[6] * vals[6] + vals[7] * vals[7] + vals[8] * vals[8];
    }

    inline float norm() { return sqrt(sqrnorm()); }

    inline float determinant() const
    {
        return vals[0] * (vals[4] * vals[8] - vals[7] * vals[5]) - vals[1] * (vals[3] * vals[8] - vals[6] * vals[5]) + vals[2] * (vals[3] * vals[7] - vals[6] * vals[4]);
    }

    inline float trace() const { return vals[0] + vals[4] + vals[8]; }

    ////////        TRANSPOSE       /////////
    inline void transpose()
    {
        float xy = vals[1], xz = vals[2], yz = vals[5];
        vals[1] = vals[3];
        vals[3] = xy;
        vals[2] = vals[6];
        vals[6] = xz;
        vals[5] = vals[7];
        vals[7] = yz;
    }

    Mat3 getTranspose() const
    {
        return Mat3(vals[0], vals[3], vals[6], vals[1], vals[4], vals[7], vals[2], vals[5], vals[8]);
    }

    Mat3 operator-() const
    {
        return Mat3(-vals[0], -vals[1], -vals[2], -vals[3], -vals[4], -vals[5], -vals[6], -vals[7], -vals[8]);
    }

    inline static Mat3 rotationX(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);
        return Mat3(1, 0, 0,
                    0, c, -s,
                    0, s, c);
    }

    inline static Mat3 rotationY(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);
        return Mat3(c, 0, s,
                    0, 1, 0,
                    -s, 0, c);
    }

    inline static Mat3 rotationZ(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);
        return Mat3(c, -s, 0,
                    s, c, 0,
                    0, 0, 1);
    }

private:
    float vals[9];
    // will be noted as :
    // 0 1 2
    // 3 4 5
    // 6 7 8
};

inline static Mat3 operator*(float s, const Mat3 &m)
{
    return Mat3(m(0, 0) * s, m(0, 1) * s, m(0, 2) * s, m(1, 0) * s, m(1, 1) * s, m(1, 2) * s, m(2, 0) * s, m(2, 1) * s,
                m(2, 2) * s);
}

inline static std::ostream &operator<<(std::ostream &s, Mat3 const &m)
{
    s << m(0, 0) << " \t" << m(0, 1) << " \t" << m(0, 2) << std::endl
      << m(1, 0) << " \t" << m(1, 1) << " \t" << m(1, 2)
      << std::endl
      << m(2, 0) << " \t" << m(2, 1) << " \t" << m(2, 2) << std::endl;
    return s;
}

