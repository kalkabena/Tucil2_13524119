#include "face.h"

vertex cross_product(face f) {
	vertex v;
	v.x = f.i.y * f.j.z - f.i.z * f.j.y;
	v.y = f.i.z * f.j.x - f.i.x * f.j.z;
	v.z = f.i.x * f.j.y - f.i.y * f.j.x;
	return v;
}