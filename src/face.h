#ifndef FACE_H
#define FACE_H

#include "vertex.h"

typedef struct {
	vertex i;
	vertex j;
	vertex k;
} face;

vertex cross_product(face f);

#endif