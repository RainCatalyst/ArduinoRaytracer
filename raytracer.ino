#include <MicroView.h>
#include "core.h"

inline constexpr int MAX_SAMPLES = 32;
inline constexpr int MAX_BOUNCES = 3;
inline constexpr int WIDTH = 64;
inline constexpr int HEIGHT = 48;
inline constexpr int BLOCK_SIZE = 4;
inline constexpr Vec3 CAMERA_ORIGIN = {0.f, 0.f, 0.f};
inline constexpr float BACKGROUND_LIGHT = 0.6f;

inline constexpr int BLOCKS_X = WIDTH / BLOCK_SIZE;
inline constexpr int BLOCKS_Y = WIDTH / BLOCK_SIZE;
inline constexpr float FWIDTH = (float) WIDTH;
inline constexpr float FHEIGHT = (float) HEIGHT;
inline constexpr float HW_RATIO = FHEIGHT / FWIDTH;

// Scene
Sphere spheres[] = {
	{ { 0.0f, 0.0f, 2.f }, 0.3f, 0.9f, 0.f }, // Sphere P, R, Color, Emission
	// { { 0.2f, 0.2f, 1.3f }, 0.1f, 0.92f, 0.f }
};

Plane planes[] = {
	{ { 0.0f, 0.3f, 0.f }, { 0.0f, -1.f, 0.0f }, 0.9f }  // Plane P, N, Color
};

Scene scene = {
  1,
  spheres,
  1,
  planes
};

// Core
bool in_operation = true;
int block_x = 0;
int block_y = 0;

void setup() {
	// REPLACE THIS TO USE A DIFFERENT SCREEN
	uView.begin();
	uView.clear(PAGE);
}

void drawPixel(int x, int y) {
	// REPLACE THIS TO USE A DIFFERENT SCREEN
	uView.pixel(x, y); // Screen drawing logic goes here
}

// Camera
Ray getCameraRay(int x, int y)
{
	Ray ray;
	ray.o = CAMERA_ORIGIN;
	float px = (float) x + get_random01();
	float py = (float) y + get_random01();
	ray.d = { px / FWIDTH - 0.5f, (py / FHEIGHT - 0.5f) * HW_RATIO, 1.5f };
	ray.d.normalize();
	return ray;
}

// Space
Vec3 worldToLocal(const Vec3& world, const Intersection& its) {
	return { dot(world, its.tangent), dot(world, its.bitangent), dot(world, its.normal) };
}

Vec3 localToWorld(const Vec3& local, const Intersection& its) {
	return its.tangent * local.x + its.bitangent * local.y + its.normal * local.z;
}

// Intersection
bool intersectPlane(const Ray &ray, Intersection &its, const Plane &plane)
{
	float d = dot(plane.n, plane.p - ray.o) / dot(ray.d, plane.n);
	if (d > 0.0 && d <= its.t)
	{
		its.t = d;
		its.normal = plane.n;
		its.color = plane.color;
		its.emission = 0.f;
		return true;
	}
	return false;
}

bool intersectSphere(const Ray &ray, Intersection &its, const Sphere &sphere)
{
	Vec3 rs = ray.o - sphere.p;
	float b = 2.0f * dot(ray.d, rs);
	float c = rs.squaredLength() - sphere.r * sphere.r;
	float a = b * b - 4.f * c;

	if (a < 0.f)
		return false;

	float d = sqrt(a);
	float t0 = 0.5 * (-b + d);
	float t1 = 0.5 * (-b - d);
	float t = t1 > 0.0 ? t1 : t0;

	if (t > 0.f && t <= its.t)
	{
		its.t = t;
		its.normal = ray(t) - sphere.p;
		its.normal.normalize();
		its.color = sphere.color;
		its.emission = sphere.emission;

		return true;
	}

	return false;
}

bool intersectScene(const Ray &ray, Intersection &its)
{
	bool any = false;
	for (int i = 0; i < scene.sphereCount; i++)
	{
		Intersection newIts = its;
		if (intersectSphere(ray, newIts, scene.spheres[i]))
		{
			its = newIts;
			any = true;
		}
	}
	for (int i = 0; i < scene.planeCount; i++)
	{
		Intersection newIts = its;
		if (intersectPlane(ray, newIts, scene.planes[i]))
		{
			its = newIts;
			any = true;
		}
	}

	if (any) {
		buildOrthonormalBasis(its.normal, its.tangent, its.bitangent);
	}

	return any;
}

// Raytracer core loop
void raytrace(int fx, int fy, int tx, int ty) {
	for (int x = fx; x < tx; x++)
	{
		for (int y = fy; y < ty; y++)
		{
			float pixel_value = 0.f;
			for (int sample = 0; sample < MAX_SAMPLES; sample++) {
				float L = 0.f;
				float throughput = 1.0f;
				Ray ray = getCameraRay(x, y);
				for (int depth = 0; depth < MAX_BOUNCES; depth++) {
					Intersection its;
					its.t = 999999.f; // Infinite distance
					bool intersected = intersectScene(ray, its);
					if (intersected) {
						L += throughput * its.emission;
						throughput = throughput * its.color;
						Vec3 wi = squareToUniformHemisphere();
						Vec3 bounce = localToWorld(wi, its);
						ray.o = ray(its.t - EPSILON);
						ray.d = bounce.normalize();
					} else {
					  if (depth == 0) {
						L += 0.f;
					  } else {
						L += throughput * BACKGROUND_LIGHT;
					  }
					  break;
					}
				}
				pixel_value += L;
			}

			pixel_value /= (float) MAX_SAMPLES;
			if (pixel_value > 0.5f) {
			  drawPixel(x, y);
			}
		}
	}
}

void loop() {
	if (in_operation) {
		// Render block by block
		raytrace(block_x * BLOCK_SIZE, block_y * BLOCK_SIZE, (block_x + 1) * BLOCK_SIZE, (block_y + 1) * BLOCK_SIZE);
		uView.display();
		block_x += 1;
		if (block_x > BLOCKS_X - 1) {
			block_x = 0;
			block_y += 1;
			if (block_y > BLOCKS_Y - 1) {
			in_operation = false;
			// Render is complete
			}
		}
	} else {
		// Do nothing, we are done
	}
}