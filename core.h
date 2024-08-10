// Raytracer core

#pragma once
#include "math.h"

// Structs
struct Ray 
{ 
  Vec3 o, d;

  Vec3 operator() (float t) const { return o + d * t; }
};

struct Intersection
{
    float t;
    Vec3 normal;
    Vec3 tangent;
    Vec3 bitangent;
    float color;
    float emission;
    bool reflect;
};

struct Sphere 
{ 
  Vec3 p;
  float r;
  float color;
  float emission;
};

struct Plane
{
  Vec3 p, n;
  float color;
};

struct Scene
{
  int sphereCount;
  Sphere* spheres;
  int planeCount;
  Plane* planes;
};