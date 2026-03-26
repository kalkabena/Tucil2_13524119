#ifndef VERTEX_H
#define VERTEX_H

typedef struct {
	double x;
	double y;
	double z;
} vertex;

typedef struct {
	vertex origin;
	vertex dir;
} line;

int abs_svo(int n);

vertex create_vertex(double x, double y, double z);

line create_line(vertex a, vertex b);

double dot(vertex a, vertex b);

vertex add(vertex a, vertex b);

vertex subtract(vertex a, vertex b);

vertex scalar_mult(vertex a, double n);

double magnitude_squared(vertex v);

double scalar_projection_2d(vertex a, vertex b);

vertex vector_projection_3d(line l, vertex v);

line perpendicular(line l, vertex v);

#endif