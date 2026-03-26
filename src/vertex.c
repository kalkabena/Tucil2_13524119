#include "vertex.h"
#include <math.h>

int abs_svo(int n) {
	if (n > 0) {
		return n;
	} else {
		return -n - 1;
	}
}

vertex create_vertex(double x, double y, double z) {
	vertex v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

line create_line(vertex a, vertex b) {
	line l;
	l.origin = a;
	l.dir = subtract(a, b);
	return l;
}

double dot(vertex a, vertex b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vertex add(vertex a, vertex b) {
	return create_vertex(a.x + b.x, a.y + b.y, a.z + b.z);
}

vertex subtract(vertex a, vertex b) {
	return create_vertex(a.x - b.x, a.y - b.y, a.z - b.z);
}

vertex scalar_mult(vertex v, double n) {
	return create_vertex(v.x * n, v.y * n, v.z * n);
}

double magnitude_squared(vertex v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

double scalar_projection_2d(vertex a, vertex b) {
	return dot(a, b) / sqrt(magnitude_squared(a));
}

vertex vector_projection_3d(line l, vertex v) {
	vertex ap = subtract(v, l.origin);
	double coef = dot(ap, l.dir) / magnitude_squared(l.dir);
	return add(l.origin, scalar_mult(l.dir, coef));
}

line perpendicular(line l, vertex v) {
	line out;
	out.origin = v;
	out.dir = subtract(v, vector_projection_3d(l, v));
	return out;
}