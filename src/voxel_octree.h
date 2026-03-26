#ifndef VOXEL_OCTREE_H
#define VOXEL_OCTREE_H

#include "face_list.h"
#include <stdio.h>

// pointers(xyz) = 000 -> 100 -> 010 -> 110 -> 001 -> 101 -> 011 -> 111
typedef struct vxl* voxel_address;
typedef struct vxl {
	char subtree;
	face_list faces;
	double length;
	vertex pos;
	int depth;
	voxel_address pointers[8];
	char filled;
} voxel;

voxel* create_voxel(face_list f, double length, int depth, vertex pos);

int sat_check_2d(int axis, face f, vertex bottom_left, double side_length);

int sat_check_3d_cube(face f, vertex bottom_left, double side_length);

int sat_check_3d_triangle(face f, vertex bottom_left, double side_length);

void voxellize(voxel *v, face_list fl, int max_depth);

void print_voxel_octree(voxel v);

void print_voxel_octree_stats();

void print_voxel_octree_to_file(voxel v, FILE *fptr);

#endif