#pragma once
#include "../astd.hpp"

using std::sin;
using std::cos;
using std::tan;
using std::pow;

struct Point2 {
	double x = 0;
	double y = 0;
};

struct Point3 {
	double x = 0;
	double y = 0;
	double z = 0;
};

inline double dot(Point3 a, Point3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double dot(Point2 a, Point2 b) {
	return a.x * b.x + a.y * b.y;
}

inline Point3 cross(Point3 a, Point3 b) {
	return Point3{ a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x };
}

inline double toRadian(double degree) {
	return degree / 180 * PI;
}

inline double toFov(double degree) {
	return tan(toRadian((180 - degree) / 2)) / 2;
}

// Rotates point in 3D from the 0, 0, 0 origin
inline Point3 rotate3D(Point3 point, double s_yaw, double c_yaw, double s_pitch, double c_pitch, double s_roll, double c_roll) {
	double old_x = point.x;
	double old_y = point.y;

	// Rotation matrix for the yaw and pitch rotations
	point.x = point.x * c_yaw + point.z * s_yaw;
	point.z = point.z * c_yaw - old_x * s_yaw;
	point.y = point.y * c_pitch + point.z * s_pitch;
	point.z = point.z * c_pitch - old_y * s_pitch;

	old_x = point.x;
	old_y = point.y;

	// Rotates the roll rotation as in 2D
	point.x = old_x * c_roll - old_y * -s_roll;
	point.y = old_x * -s_roll + old_y * c_roll;
	return point;
}

// Rotates point in 2D from the origin 0, 0
inline Point2 rotate2D(Point2 point, double s, double c) {
	return Point2{ point.x * c + point.y * s, point.x * -s + point.y * c };
}

// Otherwise known as linear lerp
inline double remap(double x, double a_min, double a_max, double b_min, double b_max) {
	double dry = a_max - a_min; // Don't Repeat Yourself, a performance boost
	if (dry == 0)
		return b_min;
	return (x - a_min) * (b_max - b_min) / dry + b_min; // (x - amin) * (bmax - bmin) / (amax - amin) + bmin
}

// Check if the points are drawn clockwise
inline bool isClockwise(Point2 a, Point2 b, Point2 c) {
	return (c.y - a.y) * (b.x - a.x) < (b.y - a.y) * (c.x - a.x);
}

// Checks if 2 lines intersect, optional intersection_point pointer to get the intersection point if they did intersect
inline bool getLineIntersection(Point2 a, Point2 b, Point2 c, Point2 d, Point2* intersection_point = NULL) {
	Point2 s1{ b.x - a.x, b.y - a.y };
	Point2 s2{ d.x - c.x, d.y - c.y };

	// Don't Repeat Yourself x3, performance boost x3
	double dry1 = a.x - c.x;
	double dry2 = a.y - c.y;
	double dry3 = -s2.x * s1.y + s1.x * s2.y;

	// This happens if the points are the same
	if (dry3 == 0) {
		if (intersection_point != NULL)
			*intersection_point = Point2{ (a.x + b.x) / 2, (a.y + b.y) / 2 };
		return true;
	}

	double s = (-s1.y * dry1 + s1.x * dry2) / dry3;
	double t = (s2.x * dry2 - s2.y * dry1) / dry3;

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		if (intersection_point != NULL)
			*intersection_point = Point2{ a.x + (t * s1.x), a.y + (t * s1.y) };
		return true;
	}
	return false;
}
