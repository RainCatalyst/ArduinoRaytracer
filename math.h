// Basic vector math operations
// Based on https://github.com/caosdoar/arduino-raytracer/blob/master/math.h by Oscar Sebio Cajaraville
#pragma once
#include "Arduino.h"

inline constexpr float EPSILON = 0.0001f;
inline constexpr float PI2 = 1.57079632679489661923f;
inline constexpr float PI4 = 0.78539816339744830961f;
inline constexpr float SQRT2 = 1.41421356237309504880f;

struct Vec3 
{ 
  float x,y,z;
  
  Vec3 operator +(const Vec3 &v) const
  {
    return {x + v.x, y + v.y, z + v.z};
  }
  
  Vec3 operator -(const Vec3 &v) const
  {
    return {x - v.x, y - v.y, z - v.z};
  }
  
  Vec3& operator +=(const Vec3 &v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  
  Vec3& operator -=(const Vec3 &v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }
  
  Vec3 operator -() const
  {
    return {-x, -y, -z};
  }
  
  Vec3 operator *(float c) const
  {
    return {x * c, y * c, z * c};
  }

  float length() {
    return sqrt(x * x + y * y + z * z);
  }

  float squaredLength()
  {
    return x * x + y * y + z * z;
  }
  
  Vec3& normalize()
  {
    float nd_rcp = 1.0 / length();
    x *= nd_rcp;
    y *= nd_rcp;
    z *= nd_rcp;
    return *this;
  }
};

float get_random01() {
  return random(0, 100000000) / 100000000.f;
}

float clamp(float x, float _min, float _max)
{
  return max(_min, min(_max, x));
}

// Vector functions
float dot(const Vec3 &v1, const Vec3 &v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 cross(const Vec3 &v1, const Vec3 &v2)
{
  Vec3 o = {
    v1.x * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x};
  return o;
}

Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
  float d = 2.0 * dot(v, n);
  Vec3 o;
  o.x = v.x - d * n.x;
  o.y = v.y - d * n.y;
  o.z = v.z - d * n.z;
  return o;
}

// Space transform
void buildOrthonormalBasis(const Vec3 &a, Vec3 &b, Vec3 &c) {
    if (abs(a.x) > abs(a.y)) {
        auto invLen = 1 / sqrt(a.x * a.x + a.z * a.z);
        c           = { a.z * invLen, 0, -a.x * invLen };
    } else {
        auto invLen = 1 / sqrt(a.y * a.y + a.z * a.z);
        c           = { 0, a.z * invLen, -a.y * invLen };
    }
    b = cross(c, a);
}

// Cosine weighted sampling
Vec3 squareToUniformDiskConcentric(float x, float y) {
    float r1 = 2 * x - 1;
    float r2 = 2 * y - 1;

    float phi, r;
    if (r1 == 0 && r2 == 0) {
        r   = 0;
        phi = 0;
    } else if (r1 * r1 > r2 * r2) {
        r   = r1;
        phi = PI4 * (r2 / r1);
    } else {
        r   = r2;
        phi = PI2 - PI4 * (r1 / r2);
    }

    float cosPhi = cos(phi);
    float sinPhi = sin(phi);
    return { r * cosPhi, r * sinPhi, 0.f };
}

Vec3 squareToUniformHemisphere() {
    float x = get_random01();
    float y = get_random01();

    Vec3 p = squareToUniformDiskConcentric(x, y);
    float z  = 1.0f - p.x * p.x - p.y * p.y;
    float s  = sqrt(z + 1.0f);
    return { s * p.x, s * p.y, z };
}