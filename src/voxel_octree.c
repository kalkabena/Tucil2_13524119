#include "voxel_octree.h"
#include <math.h>
#include <stdlib.h>

int global_counter = 0;
int global_nodes[100] = {0};
int global_null_nodes[100] = {0};

voxel* create_voxel(face_list f, double length, int depth, vertex pos) {
	voxel* v = (voxel*)malloc(sizeof(voxel));
	v->length = length;
	v->depth = depth;
	v->subtree = 0;
	v->pos = pos;
	create_face_list(&(v->faces), 20);
	for (int i = 0; i < face_list_length(f); i++) {
		insert_fl_last(&(v->faces), f.buffer[i]);
	}
	v->filled = 0;
	for (int i = 0; i < 8; i++) {
		v->pointers[i] = NULL;
	}
	return v;
}

int sat_check_2d(int axis, face f, vertex bottom_left, double side_length) {
	if (axis == 1) {// yz
		f.i.x = f.i.y;
		f.j.x = f.j.y;
		f.k.x = f.k.y;
		bottom_left.x = bottom_left.y;
		f.i.y = f.i.z;
		f.j.y = f.j.z;
		f.k.y = f.k.z;
		bottom_left.y = bottom_left.z;
	} else if (axis == 2) { //xz
		f.i.y = f.i.z;
		f.j.y = f.j.z;
		f.k.y = f.k.z;
		bottom_left.y = bottom_left.z;
	} else if (axis == 3) { //xy
		//do nothing
	} else {
		return 2;
	}
	f.i.z = 0;
	f.j.z = 0;
	f.k.z = 0;
	bottom_left.z = 0;
	
	vertex bottom_right = create_vertex(bottom_left.x + side_length, bottom_left.y, 0);
	vertex top_left = create_vertex(bottom_left.x, bottom_left.y + side_length, 0);
	vertex top_right = create_vertex(bottom_left.x + side_length, bottom_left.y + side_length, 0);
	
	double fxmax = f.i.x;
	double fxmin = f.i.x;
	double fymax = f.i.y;
	double fymin = f.i.y;
	
	if (f.j.x > fxmax) fxmax = f.j.x;
	if (f.j.x < fxmin) fxmin = f.j.x;
	if (f.k.x > fxmax) fxmax = f.k.x;
	if (f.k.x < fxmin) fxmin = f.k.x;
	if (f.j.y > fymax) fymax = f.j.y;
	if (f.j.y < fymin) fymin = f.j.y;
	if (f.k.y > fymax) fymax = f.k.y;
	if (f.k.y < fymin) fymin = f.k.y;
	
	if (fxmin > top_right.x || fxmax < bottom_left.x) return 1;
	if (fymin > top_right.y || fymax < bottom_left.y) return 1;
	
	vertex v;
	double ftrimax;
	double ftrimin;
	double fsqmax;
	double fsqmin;
	v.x = 1;
	v.y = -(f.j.x - f.i.x) / (f.j.y - f.i.y);
	ftrimax = scalar_projection_2d(v, f.i);
	ftrimin = ftrimax;
	if (scalar_projection_2d(v, f.j) > ftrimax) ftrimax = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.j) < ftrimin) ftrimin = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.k) > ftrimax) ftrimax = scalar_projection_2d(v, f.k);
	if (scalar_projection_2d(v, f.k) < ftrimin) ftrimin = scalar_projection_2d(v, f.k);
	
	fsqmax = scalar_projection_2d(v, bottom_left);
	fsqmin = fsqmax;
	if (scalar_projection_2d(v, bottom_right) > fsqmax) fsqmax = scalar_projection_2d(v, bottom_right);
	if (scalar_projection_2d(v, bottom_right) < fsqmin) fsqmin = scalar_projection_2d(v, bottom_right);
	if (scalar_projection_2d(v, top_left) > fsqmax) fsqmax = scalar_projection_2d(v, top_left);
	if (scalar_projection_2d(v, top_left) < fsqmin) fsqmin = scalar_projection_2d(v, top_left);
	if (scalar_projection_2d(v, top_right) > fsqmax) fsqmax = scalar_projection_2d(v, top_right);
	if (scalar_projection_2d(v, top_right) < fsqmin) fsqmin = scalar_projection_2d(v, top_right);
	
	if (ftrimax < fsqmin || ftrimin > fsqmax) return 1;
	
	v.y = -(f.k.x - f.j.x) / (f.k.y - f.j.y);
	ftrimax = scalar_projection_2d(v, f.i);
	ftrimin = ftrimax;
	if (scalar_projection_2d(v, f.j) > ftrimax) ftrimax = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.j) < ftrimin) ftrimin = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.k) > ftrimax) ftrimax = scalar_projection_2d(v, f.k);
	if (scalar_projection_2d(v, f.k) < ftrimin) ftrimin = scalar_projection_2d(v, f.k);
	
	fsqmax = scalar_projection_2d(v, bottom_left);
	fsqmin = fsqmax;
	if (scalar_projection_2d(v, bottom_right) > fsqmax) fsqmax = scalar_projection_2d(v, bottom_right);
	if (scalar_projection_2d(v, bottom_right) < fsqmin) fsqmin = scalar_projection_2d(v, bottom_right);
	if (scalar_projection_2d(v, top_left) > fsqmax) fsqmax = scalar_projection_2d(v, top_left);
	if (scalar_projection_2d(v, top_left) < fsqmin) fsqmin = scalar_projection_2d(v, top_left);
	if (scalar_projection_2d(v, top_right) > fsqmax) fsqmax = scalar_projection_2d(v, top_right);
	if (scalar_projection_2d(v, top_right) < fsqmin) fsqmin = scalar_projection_2d(v, top_right);
	
	if (ftrimax < fsqmin || ftrimin > fsqmax) return 1;
	
	v.y = -(f.i.x - f.k.x) / (f.i.y - f.k.y);
	ftrimax = scalar_projection_2d(v, f.i);
	ftrimin = ftrimax;
	if (scalar_projection_2d(v, f.j) > ftrimax) ftrimax = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.j) < ftrimin) ftrimin = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.k) > ftrimax) ftrimax = scalar_projection_2d(v, f.k);
	if (scalar_projection_2d(v, f.k) < ftrimin) ftrimin = scalar_projection_2d(v, f.k);
	
	fsqmax = scalar_projection_2d(v, bottom_left);
	fsqmin = fsqmax;
	if (scalar_projection_2d(v, bottom_right) > fsqmax) fsqmax = scalar_projection_2d(v, bottom_right);
	if (scalar_projection_2d(v, bottom_right) < fsqmin) fsqmin = scalar_projection_2d(v, bottom_right);
	if (scalar_projection_2d(v, top_left) > fsqmax) fsqmax = scalar_projection_2d(v, top_left);
	if (scalar_projection_2d(v, top_left) < fsqmin) fsqmin = scalar_projection_2d(v, top_left);
	if (scalar_projection_2d(v, top_right) > fsqmax) fsqmax = scalar_projection_2d(v, top_right);
	if (scalar_projection_2d(v, top_right) < fsqmin) fsqmin = scalar_projection_2d(v, top_right);
	
	if (ftrimax < fsqmin || ftrimin > fsqmax) return 1;
	
	return 0;
}

int sat_check_3d_cube(face f, vertex bottom_left, double side_length) {
	if (sat_check_2d(1, f, bottom_left, side_length) == 1) return 1;
	if (sat_check_2d(2, f, bottom_left, side_length) == 1) return 1;
	if (sat_check_2d(3, f, bottom_left, side_length) == 1) return 1;
	return 0;
}

int sat_check_3d_triangle(face f, vertex bottom_left, double side_length) {
	face f1;
	f1.i = subtract(f.j, f.i);
	f1.j = subtract(f.k, f.j);
	f1.k = subtract(f.i, f.k);
	vertex v = cross_product(f1);
	double ftrimax;
	double ftrimin;
	double fsqmax;
	double fsqmin;
	
	vertex v001 = create_vertex(bottom_left.x + side_length, bottom_left.y, bottom_left.z);
	vertex v010 = create_vertex(bottom_left.x, bottom_left.y + side_length, bottom_left.z);
	vertex v011 = create_vertex(bottom_left.x + side_length, bottom_left.y + side_length, bottom_left.z);
	vertex v100 = create_vertex(bottom_left.x, bottom_left.y, bottom_left.z + side_length);
	vertex v101 = create_vertex(bottom_left.x + side_length, bottom_left.y, bottom_left.z + side_length);
	vertex v110 = create_vertex(bottom_left.x, bottom_left.y + side_length, bottom_left.z + side_length);
	vertex v111 = create_vertex(bottom_left.x + side_length, bottom_left.y + side_length, bottom_left.z + side_length);
	
	ftrimax = scalar_projection_2d(v, f.i);
	ftrimin = ftrimax;
	if (scalar_projection_2d(v, f.j) > ftrimax) ftrimax = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.j) < ftrimin) ftrimin = scalar_projection_2d(v, f.j);
	if (scalar_projection_2d(v, f.k) > ftrimax) ftrimax = scalar_projection_2d(v, f.k);
	if (scalar_projection_2d(v, f.k) < ftrimin) ftrimin = scalar_projection_2d(v, f.k);
	
	fsqmax = scalar_projection_2d(v, bottom_left);
	fsqmin = fsqmax;
	if (scalar_projection_2d(v, v001) > fsqmax) fsqmax = scalar_projection_2d(v, v001);
	if (scalar_projection_2d(v, v001) < fsqmin) fsqmin = scalar_projection_2d(v, v001);
	if (scalar_projection_2d(v, v010) > fsqmax) fsqmax = scalar_projection_2d(v, v010);
	if (scalar_projection_2d(v, v010) < fsqmin) fsqmin = scalar_projection_2d(v, v010);
	if (scalar_projection_2d(v, v011) > fsqmax) fsqmax = scalar_projection_2d(v, v011);
	if (scalar_projection_2d(v, v011) < fsqmin) fsqmin = scalar_projection_2d(v, v011);
	if (scalar_projection_2d(v, v100) > fsqmax) fsqmax = scalar_projection_2d(v, v100);
	if (scalar_projection_2d(v, v100) < fsqmin) fsqmin = scalar_projection_2d(v, v100);
	if (scalar_projection_2d(v, v101) > fsqmax) fsqmax = scalar_projection_2d(v, v101);
	if (scalar_projection_2d(v, v101) < fsqmin) fsqmin = scalar_projection_2d(v, v101);
	if (scalar_projection_2d(v, v110) > fsqmax) fsqmax = scalar_projection_2d(v, v110);
	if (scalar_projection_2d(v, v110) < fsqmin) fsqmin = scalar_projection_2d(v, v110);
	if (scalar_projection_2d(v, v111) > fsqmax) fsqmax = scalar_projection_2d(v, v111);
	if (scalar_projection_2d(v, v111) < fsqmin) fsqmin = scalar_projection_2d(v, v111);
	
	if (ftrimax < fsqmin || ftrimin > fsqmax) return 1;
	
	return 0;
}

void voxellize(voxel *v, face_list fl, int max_depth) {
	global_nodes[v->depth]++;
	double half_side_length = v->length / 2;
	vertex va[8];
	va[0] = v->pos;
	va[1] = create_vertex((v->pos).x + half_side_length, (v->pos).y, (v->pos).z);
	va[2] = create_vertex((v->pos).x, (v->pos).y + half_side_length, (v->pos).z);
	va[3] = create_vertex((v->pos).x + half_side_length, (v->pos).y + half_side_length, (v->pos).z);
	va[4] = create_vertex((v->pos).x, (v->pos).y, (v->pos).z + half_side_length);
	va[5] = create_vertex((v->pos).x + half_side_length, (v->pos).y, (v->pos).z + half_side_length);
	va[6] = create_vertex((v->pos).x, (v->pos).y + half_side_length, (v->pos).z + half_side_length);
	va[7] = create_vertex((v->pos).x + half_side_length, (v->pos).y + half_side_length, (v->pos).z + half_side_length);
	
	if (v->depth == max_depth) {
		for (int i = 0; i < face_list_length(fl); i++) {
			face f = fl.buffer[i];
			v->subtree = (!(sat_check_3d_cube(f, v->pos, v->length)) && !(sat_check_3d_triangle(f, v->pos, v->length)));
			if (v->subtree > 0) {
				v->filled = 1;
				break;
			}
		}
		return;
	} else if (v->depth < max_depth) {
		for (int i = 0; i < 8; i++) {
			face_list temp_list;
			create_face_list(&temp_list, 20);
			for (int j = 0; j < face_list_length(fl); j++) {
				int temp = (!(sat_check_3d_cube(fl.buffer[j], va[i], half_side_length)) && !(sat_check_3d_triangle(fl.buffer[j], va[i], half_side_length)));
				if (temp > 0) insert_fl_last(&temp_list, fl.buffer[j]);
				v->subtree |= temp << i;
			}
			if (face_list_length(temp_list) > 0) {
				v->pointers[i] = create_voxel(temp_list, half_side_length, v->depth + 1, va[i]);
				voxellize(v->pointers[i], temp_list, max_depth);
			} else {
				global_null_nodes[v->depth + 1]++;
				v->pointers[i] = NULL;
			}
			deallocate_face_list(&temp_list);
		}
		
		v->filled = 1;
		for (int i = 0; i < 8; i++) {
			if (v->pointers[i] == NULL) {
				v->filled = 0;
				break;
			}
			if (v->pointers[i]->filled == 0) {
				v->filled = 0;
				break;
			}
		}
	}
}

void print_voxel_octree(voxel v) {
	if (v.filled == 1) {
		printf("pos:(%.2lf, %.2lf, %.2lf), length:%.2lf d:%d\n", v.pos.x, v.pos.y, v.pos.z, v.length, v.depth);
	} else {
		//printf("pos:(%.2lf, %.2lf, %.2lf), length:%.2lf d:%d F:%d\n", v.pos.x, v.pos.y, v.pos.z, v.length, v.depth, v.filled);
		for (int i = 0; i < 8; i++) {
			if (v.pointers[i] != NULL) {
				print_voxel_octree(*(v.pointers[i]));
			}
		}
	}
}

void print_voxel_octree_stats() {
	long long int sum_gnodes = 0;
	for (int i = 0; i< 100; i++) {
		sum_gnodes += global_nodes[i];
	}
	printf("\nStatistics:\n");
	printf("Nodes: %lld\n", sum_gnodes);
	printf("Vertices: %lld\n", sum_gnodes * 8);
	printf("Faces: %lld\n", sum_gnodes * 12);
	printf("Nodes:\n");
	for (int i = 1; i < 100; i++) {
		if (global_nodes[i] == 0) break;
		printf("%d : %lld\n", i, global_nodes[i]);
	}
	printf("\nUnused nodes:\n");
	for (int i = 1; i < 100; i++) {
		if (global_null_nodes[i] == 0) break;
		printf("%d : %lld\n", i, global_null_nodes[i]);
	}
	printf("\n");
}

void print_voxel_octree_to_file(voxel v, FILE *fptr) {
	//printf("pos:(%.2lf, %.2lf, %.2lf), length:%.2lf d:%d F:%d\n", v.pos.x, v.pos.y, v.pos.z, v.length, v.depth, v.filled);
	if (v.filled == 1) {
		fprintf(fptr, "v %lf %lf %lf\n", v.pos.x, v.pos.y, v.pos.z);
		fprintf(fptr, "v %lf %lf %lf\n", v.pos.x + v.length, v.pos.y, v.pos.z);
		fprintf(fptr, "v %lf %lf %lf\n", v.pos.x, v.pos.y + v.length, v.pos.z);
		fprintf(fptr, "v %lf %lf %lf\n", v.pos.x + v.length, v.pos.y + v.length, v.pos.z);
		
		fprintf(fptr, "v %lf %lf %lf\n", v.pos.x, v.pos.y, v.pos.z + v.length);
		fprintf(fptr, "v %lf %lf %lf\n", v.pos.x + v.length, v.pos.y, v.pos.z + v.length);
		fprintf(fptr, "v %lf %lf %lf\n", v.pos.x, v.pos.y + v.length, v.pos.z + v.length);
		fprintf(fptr, "v %lf %lf %lf\n\n", v.pos.x + v.length, v.pos.y + v.length, v.pos.z + v.length);
		
		//xy
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 1, global_counter * 8 + 2, global_counter * 8 + 4);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 1, global_counter * 8 + 4, global_counter * 8 + 3);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 5, global_counter * 8 + 6, global_counter * 8 + 8);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 5, global_counter * 8 + 8, global_counter * 8 + 7);
		
		//xz
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 1, global_counter * 8 + 5, global_counter * 8 + 6);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 1, global_counter * 8 + 6, global_counter * 8 + 2);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 3, global_counter * 8 + 7, global_counter * 8 + 8);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 3, global_counter * 8 + 8, global_counter * 8 + 4);
		
		//yz
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 1, global_counter * 8 + 3, global_counter * 8 + 7);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 1, global_counter * 8 + 7, global_counter * 8 + 5);
		fprintf(fptr, "f %d %d %d\n", global_counter * 8 + 2, global_counter * 8 + 4, global_counter * 8 + 8);
		fprintf(fptr, "f %d %d %d\n\n", global_counter * 8 + 2, global_counter * 8 + 8, global_counter * 8 + 6);
		global_counter++;
	} else {
		for (int i = 0; i < 8; i++) {
			if (v.pointers[i] != NULL) {
				print_voxel_octree_to_file(*(v.pointers[i]), fptr);
			}
		}
	}
}