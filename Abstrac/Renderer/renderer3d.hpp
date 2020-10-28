#pragma once
#include "../astd.hpp"
#include "math.hpp"

extern sf::String debug_text;

struct SolidTri {
	Point3 a, b, c;
	U32 color = 0xFFFFFFFF;
};

struct Mesh {
	Point3 position, rotation;
	std::vector<SolidTri> solid_tris;
};

class Render3DLayer {
public:
	U32 width, height;
	U32* render_buffer;
	double fov, far_clipping, near_clipping;
	Point3 position = Point3{ 0, 0, 0 };
	Point3 rotation = Point3{ 0, 0, 0 };
	std::vector<Mesh> meshes;

	Render3DLayer(U32 w, U32 h, double camera_fov, double far_clip, double near_clip);
	~Render3DLayer();
	double clear(U32 color = 0xFF000000);
	double render();
};