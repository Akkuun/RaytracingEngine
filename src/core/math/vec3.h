#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>

class vec3;
static inline vec3 operator+(vec3 const &a, vec3 const &b);
static inline vec3 operator*(float a, vec3 const &b);

class vec3
{
public:
    float x, y, z;

    vec3() : x(0.f), y(0.f), z(0.f) {}
    vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    vec3(float f) : x(f), y(f), z(f) {}

    float &operator[](unsigned int c) {
        if (c == 0) return x;
        if (c == 1) return y;
        return z;
    }
    float operator[](unsigned int c) const {
        if (c == 0) return x;
        if (c == 1) return y;
        return z;
    }
    vec3 &operator=(vec3 const &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    float squareLength() const {
        return x * x + y * y + z * z;
    }
    float length() const { return sqrt(squareLength()); }
    inline float normalize() const { return length(); }
    inline float squareNorm() const { return squareLength(); }
    void normalize() {
        float L = length();
        x /= L;
        y /= L;
        z /= L;
    }
    static float dot(vec3 const &a, vec3 const &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static vec3 cross(vec3 const &a, vec3 const &b) {
        return vec3(a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x);
    }
    void operator+=(vec3 const &other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }
    void operator-=(vec3 const &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }
    void operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
    }
    void operator/=(float s) {
        x /= s;
        y /= s;
        z /= s;
    }
    static vec3 compProduct(vec3 const &a, vec3 const &b) {
        return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
    }

    unsigned int getMaxAbsoluteComponent() const {
        if (fabs(x) > fabs(y)) {
            if (fabs(x) > fabs(z)) {
                return 0;
            }
            return 2;
        }
        if (fabs(y) > fabs(z)) {
            return 1;
        }
        return 2;
    }
    vec3 getOrthogonal() const {
        unsigned int c1 = getMaxAbsoluteComponent();
        unsigned int c2 = (c1 + 1) % 3;
        vec3 res(0, 0, 0);
        if (c1 == 0 && c2 == 1) { res.x = y; res.y = -x; }
        else if (c1 == 1 && c2 == 2) { res.y = z; res.z = -y; }
        else if (c1 == 2 && c2 == 0) { res.z = x; res.x = -z; }
        return res;
    }
};

static inline vec3 operator+(vec3 const &a, vec3 const &b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
static inline vec3 operator-(vec3 const &a, vec3 const &b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
static inline vec3 operator*(float a, vec3 const &b) {
    return vec3(a * b.x, a * b.y, a * b.z);
}
static inline vec3 operator*(vec3 const &b, float a) {
    return vec3(a * b.x, a * b.y, a * b.z);
}
static inline vec3 operator/(vec3 const &a, float b) {
    return vec3(a.x / b, a.y / b, a.z / b);
}
static inline std::ostream &operator<<(std::ostream &s, vec3 const &p) {
    s << p.x << " " << p.y << " " << p.z;
    return s;
}
static inline std::istream &operator>>(std::istream &s, vec3 &p) {
    s >> p.x >> p.y >> p.z;
    return s;
}
