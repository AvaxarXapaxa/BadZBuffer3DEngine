#include "renderer3d.hpp"

Render3DLayer::Render3DLayer(U32 w, U32 h, double camera_fov, double far_clip, double near_clip) {
	// Default constructor, assigns variables and allocate render_buffer
	this->width = w;
	this->height = h;
	this->fov = camera_fov;
	this->far_clipping = far_clip;
	this->near_clipping = near_clip;
	this->render_buffer = new U32[(size_t)w * (size_t)h];
}

Render3DLayer::~Render3DLayer() {
	// Deallocate render_buffer
	delete[] this->render_buffer;
}

double Render3DLayer::clear(U32 color) {
	double start = astd::getTime();
	size_t size = (size_t)this->width * (size_t)this->height;
	// Set every pixel into opaque black
	for (size_t p = 0; p < size; p++)
		this->render_buffer[p] = color;
	return astd::getTime() - start; // Returns total process time
}

double Render3DLayer::render() {
	double start = astd::getTime();
	// TRANSLATIONS RELATIVE TO THE CAMERA //

	// Precalculate the sine and cosine of the camera angles
	double s_yaw_cam = sin(toRadian(-this->rotation.x));
	double c_yaw_cam = cos(toRadian(-this->rotation.x));

	double s_pitch_cam = sin(toRadian(-this->rotation.y));
	double c_pitch_cam = cos(toRadian(-this->rotation.y));

	double s_roll_cam = sin(toRadian(-this->rotation.z));
	double c_roll_cam = cos(toRadian(-this->rotation.z));

	std::vector<SolidTri> solid_tris;
	for (Mesh& mesh : this->meshes) {
		// Precalculate the offset from the mesh
		Point3 position_offset = Point3{ mesh.position.x - this->position.x, 
			mesh.position.y - this->position.y, 
			mesh.position.z - this->position.z };

		// Precalculate the sine and cosine of the mesh's rotation 
		double s_yaw = sin(toRadian(mesh.rotation.x));
		double c_yaw = cos(toRadian(mesh.rotation.x));

		double s_pitch = sin(toRadian(mesh.rotation.y));
		double c_pitch = cos(toRadian(mesh.rotation.y));

		double s_roll = sin(toRadian(mesh.rotation.z));
		double c_roll = cos(toRadian(mesh.rotation.z));

		// Converts the triangles' positions relative to the camera
		for (SolidTri absolute_tri : mesh.solid_tris) {
			// Rotates the triangle's points by the mesh's rotation
			absolute_tri.a = rotate3D(absolute_tri.a, s_yaw, c_yaw, s_pitch, c_pitch, s_roll, c_roll);
			absolute_tri.b = rotate3D(absolute_tri.b, s_yaw, c_yaw, s_pitch, c_pitch, s_roll, c_roll);
			absolute_tri.c = rotate3D(absolute_tri.c, s_yaw, c_yaw, s_pitch, c_pitch, s_roll, c_roll);

			// Adds the offset
			absolute_tri.a = Point3{ absolute_tri.a.x + position_offset.x,
				absolute_tri.a.y + position_offset.y,
				absolute_tri.a.z + position_offset.z, };
			absolute_tri.b = Point3{ absolute_tri.b.x + position_offset.x,
				absolute_tri.b.y + position_offset.y,
				absolute_tri.b.z + position_offset.z, };
			absolute_tri.c = Point3{ absolute_tri.c.x + position_offset.x,
				absolute_tri.c.y + position_offset.y,
				absolute_tri.c.z + position_offset.z, };

			// Rotates the triangles as if the camera is rotated
			absolute_tri.a = rotate3D(absolute_tri.a, s_yaw_cam, c_yaw_cam, s_pitch_cam, c_pitch_cam, s_roll_cam, c_roll_cam);
			absolute_tri.b = rotate3D(absolute_tri.b, s_yaw_cam, c_yaw_cam, s_pitch_cam, c_pitch_cam, s_roll_cam, c_roll_cam);
			absolute_tri.c = rotate3D(absolute_tri.c, s_yaw_cam, c_yaw_cam, s_pitch_cam, c_pitch_cam, s_roll_cam, c_roll_cam);

			absolute_tri.color = absolute_tri.color;
			solid_tris.push_back(absolute_tri);
		}
	}

	// TRANSLATIONS ON PROJECTION AND SCREEN COORDINATES //

	std::vector<SolidTri> projected_solid_tris;
	std::vector<SolidTri> passed_solid_tris;

	double max_ratio;
	if (this->width > this->height)
		max_ratio = this->width;
	else
		max_ratio = this->height;

	double converted_fov = toFov(this->fov) * max_ratio;
	for (SolidTri tri : solid_tris) {
		// Eliminate triangles that are behind the near clipping or farther than the far clipping
		if (tri.a.z <= this->near_clipping && tri.b.z <= this->near_clipping && tri.c.z <= this->near_clipping)
			continue;
		if (tri.a.z >= this->far_clipping && tri.b.z >= this->far_clipping && tri.c.z >= this->far_clipping)
			continue;

		// Avoid high division values later on
		if (tri.a.z < 0.001 && tri.a.z > -0.001) tri.a.z = 0.001;
		if (tri.b.z < 0.001 && tri.b.z > -0.001) tri.b.z = 0.001;
		if (tri.c.z < 0.001 && tri.c.z > -0.001) tri.c.z = 0.001;

		SolidTri original_tri = tri;

		// Transforming stuff
		tri.a.x = (tri.a.x / tri.a.z) * converted_fov + (this->width / 2);
		tri.a.y = -(tri.a.y / tri.a.z) * converted_fov + (this->height / 2);
		tri.b.x = (tri.b.x / tri.b.z) * converted_fov + (this->width / 2);
		tri.b.y = -(tri.b.y / tri.b.z) * converted_fov + (this->height / 2);
		tri.c.x = (tri.c.x / tri.c.z) * converted_fov + (this->width / 2);
		tri.c.y = -(tri.c.y / tri.c.z) * converted_fov + (this->height / 2);

		// Ignore triangles that are completely outside the viewport
		if (tri.a.x < 0 && tri.b.x < 0 && tri.c.x < 0) continue;
		if (tri.a.x >= this->width && tri.b.x >= this->width && tri.c.x >= this->width) continue;
		if (tri.a.y < 0 && tri.b.y < 0 && tri.c.y < 0) continue;
		if (tri.a.y >= this->height && tri.b.y >= this->height && tri.c.y >= this->height) continue;

		projected_solid_tris.push_back(tri);
		passed_solid_tris.push_back(original_tri);
	}

	// RENDERING //

	// Create depth buffer and set it all to infinity
	size_t buffer_size = (size_t)this->width * (size_t)this->height;
	double* depth_buffer = new double[buffer_size];
	for (size_t pix = 0; pix < buffer_size; pix++)
		depth_buffer[pix] = INF;

	// Lambda function for checking if a number is in a 1D line
	auto inRange = [](I64 point, I64 a, I64 b) -> bool {
		if (a > b) {
			I64 temp = a;
			a = b;
			b = temp;
		}
		if (point >= a && point <= b)
			return true;
		return false;
	};

	// For each triangle
	for (U64 i = 0; i < projected_solid_tris.size(); i++) {
		SolidTri& projected_tri = projected_solid_tris[i];
		SolidTri& original_tri = passed_solid_tris[i];

		debug_text =
			"ABC A... X:" + std::to_string(projected_tri.a.x) +
			" Y:" + std::to_string(projected_tri.a.y) +
			" Z:" + std::to_string(projected_tri.a.z) + '\n' +

			"ABC B... X:" + std::to_string(projected_tri.b.x) +
			" Y:" + std::to_string(projected_tri.b.y) +
			" Z:" + std::to_string(projected_tri.b.z) + '\n' + 

			"ABC C... X:" + std::to_string(projected_tri.c.x) +
			" Y:" + std::to_string(projected_tri.c.y) +
			" Z:" + std::to_string(projected_tri.c.z) + '\n';

		// Get the highest and lowest Y line in the triangle
		I64 top_line = projected_tri.a.y;
		I64 bottom_line = projected_tri.c.y;
		if (projected_tri.b.y < top_line)
			top_line = projected_tri.b.y;
		if (projected_tri.c.y < top_line)
			top_line = projected_tri.c.y;
		if (projected_tri.a.y > bottom_line)
			bottom_line = projected_tri.a.y;
		if (projected_tri.b.y > bottom_line)
			bottom_line = projected_tri.b.y;

		debug_text +=
			"bottom_line: " + std::to_string(bottom_line) + '\n' +
			"top_line: " + std::to_string(top_line) + "\n\n";

		// Iterate in the range of the lines
		for (I64 line = top_line; line < bottom_line; line++) {
			if (line < 0) {
				line = -1;
				continue;
			}
			if (line >= this->height)
				break;

			// Get triangle lines' intersection with the scanning line
			std::vector<Point3> line_points;

			if (inRange(line, projected_tri.a.y, projected_tri.b.y)) {
				line_points.push_back(projected_tri.a);
				line_points.push_back(projected_tri.b);
			}
			if (inRange(line, projected_tri.b.y, projected_tri.c.y)) {
				line_points.push_back(projected_tri.b);
				line_points.push_back(projected_tri.c);
			}
			if (inRange(line, projected_tri.c.y, projected_tri.a.y)) {
				line_points.push_back(projected_tri.c);
				line_points.push_back(projected_tri.a);
			}
			if (line_points.size() < 4)
				continue;

			Point3& a1 = line_points[0];
			Point3& a2 = line_points[1];
			Point3& b1 = line_points[2];
			Point3& b2 = line_points[3];

			// Map to get the X coordinate of the scanned line
			I64 ax = remap(line, a1.y, a2.y, a1.x, a2.x);
			I64 bx = remap(line, b1.y, b2.y, b1.x, b2.x);

			// Also... depth/z
			double az = remap(line, a1.y, a2.y, a1.z, a2.z);
			double bz = remap(line, b1.y, b2.y, b1.z, b2.z);

			// Swap if ax is greater than bx
			if (ax > bx) {
				I64 cx = ax;
				double cz = az;
				ax = bx;
				az = bz;
				bx = cx;
				bz = cz;
			}

			// Calculate depth step in each pixel of the line
			double step = 0;
			if (bx - ax != 0)
				step = (bz - az) / (double)(bx - ax);
			double depth_steps = az - step;
			U32 line_dry = line * this->width;

			debug_text +=
				"line: " + std::to_string(line) + '\n' +
				"ax: " + std::to_string(ax) + '\n' +
				"bx: " + std::to_string(bx) + '\n' +
				"az: " + std::to_string(az) + '\n' +
				"bz: " + std::to_string(bz) + '\n' +
				"step: " + std::to_string(step) + "\n\n";

			// Each pixel of the scanned line
			for (I64 x = ax; x < bx; x++) {
				// Add the depth step
				depth_steps += step;
				if (x < 0) { // The pixel is to the left of the screen, skip to the X 0
					depth_steps += step * -x;
					x = -1;
					continue;
				}
				if (x >= this->width) // The pixel is to the right, skip to the next line
					break;

				// If it's in front of the previous pixel's depth, render it
				if (depth_buffer[x + line_dry] >= depth_steps) {
					depth_buffer[x + line_dry] = depth_steps;
					this->render_buffer[x + line_dry] = projected_tri.color;
				}
			}
		}
	}

	delete[] depth_buffer;
	return astd::getTime() - start;
}
