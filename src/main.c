#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "vertex_list.h"
#include "face_list.h"
#include "voxel_octree.h"

//compile: gcc -O3 -o t main.c vertex.c face.c vertex_list.c face_list.c voxel_octree.c  

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
    #include <windows.h>
	
    void run_on_windows() {
		char path[MAX_PATH];
        DWORD result = GetCurrentDirectory(MAX_PATH, path);
		if (result > 0 && result < MAX_PATH) {
			printf("%s", path);
		} else {
			perror("Failed to get current directory.");
		}
    }
#elif defined(__linux__)
    #include <unistd.h>
	#include <linux/limits.h>
	
    void run_on_linux() {
		char path[PATH_MAX];
        if (getcwd(path, sizeof(path)) != NULL) {
			printf("%s", path);
		} else {
			perror("getcwd() error");
		}
    }
#elif defined(__APPLE__) && defined(__MACH__)
    #include <unistd.h>
	#include <sys/syslimits.h>
	
    void run_on_macos() {
		char path[PATH_MAX];
        if (getcwd(path, sizeof(path)) != NULL) {
			printf("%s", path);
		} else {
			perror("getcwd() error");
		}
    }
#else
    #error "Unsupported operating system"
#endif

int main() {
	FILE *objptr = NULL;
	char obj_name[100];
	char obj_line[200];
	vertex_list vl;
	face_list fl;
	create_vertex_list(&vl, 100);
	create_face_list(&fl, 20);
	
	while (objptr == NULL) {
		printf("Write absolute path to .obj file (or q to quit): ");
		scanf("%s", obj_name);
		if ((obj_name[0] == 'q' || obj_name[0] == 'Q') && obj_name[1] == '\0') {
			deallocate_vertex_list(&vl);
			deallocate_face_list(&fl);
			return 0;
		}
		objptr = fopen(obj_name, "r");
		if (objptr == NULL) printf("File not found.\n");
	}
	while(fgets(obj_line, 200, objptr)) {
		//printf("%s", obj_line);
		char *token;
		char *endptr;
		if (obj_line[0] == 'v' && obj_line[1] == ' ') {
			vertex v;
			token = strtok(obj_line, " ");
			token = strtok(NULL, " ");
			v.x = strtod(token, &endptr);
			if (endptr == token) {
				fprintf(stderr, "Error: No conversion could be performed on <%s>\n", token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (*endptr != '\0' && *endptr != '\n') {
				fprintf(stderr, "Error: Invalid character: <%c> in <%s>\n", *endptr, token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (errno == ERANGE) {
				fprintf(stderr, "Error: v%d.x is out of range\n", get_vl_last_idx(vl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			
			token = strtok(NULL, " ");
			v.y = strtod(token, &endptr);
			if (endptr == token) {
				fprintf(stderr, "Error: No conversion could be performed on <%s>\n", token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (*endptr != '\0' && *endptr != '\n') {
				fprintf(stderr, "Error: Invalid character: <%c> in <%s>\n", *endptr, token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (errno == ERANGE) {
				fprintf(stderr, "Error: v%d.x is out of range\n", get_vl_last_idx(vl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			
			token = strtok(NULL, " ");
			v.z = strtod(token, &endptr);
			if (endptr == token) {
				fprintf(stderr, "Error: No conversion could be performed on <%s>\n", token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (*endptr != '\0' && *endptr != '\n') {
				fprintf(stderr, "Error: Invalid character: <%d> in <%s>\n", (int)(*endptr), token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (errno == ERANGE) {
				fprintf(stderr, "Error: v%d.x is out of range\n", get_vl_last_idx(vl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			
			token = strtok(NULL, " ");
			if (token != NULL) {
				fprintf(stderr, "Error: v%d has more than 3 arguments\n", get_vl_last_idx(vl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			insert_vl_last(&vl, v);
		} else if (obj_line[0] == 'f' && obj_line[1] == ' ') {
			face f;
			int temp;
			token = strtok(obj_line, " ");
			token = strtok(NULL, " ");
			temp = strtol(token, &endptr, 10);
			if (endptr == token) {
				fprintf(stderr, "Error: No conversion could be performed on <%s>\n", token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (*endptr != '\0' && *endptr != '\n') {
				fprintf(stderr, "Error: Invalid character: <%c> in <%s>\n", *endptr, token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (errno == ERANGE) {
				fprintf(stderr, "Error: f%d.i is out of range\n", get_fl_last_idx(fl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (temp > 0) {
				f.i = vl.buffer[temp - 1];
			} else if (temp < 0) {
				f.i = vl.buffer[vertex_list_length(vl) + temp];
			} else {
				fprintf(stderr, "Error: f%d.i is out of range\n", get_fl_last_idx(fl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			
			token = strtok(NULL, " ");
			temp = strtol(token, &endptr, 10);
			if (endptr == token) {
				fprintf(stderr, "Error: No conversion could be performed on <%s>\n", token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (*endptr != '\0' && *endptr != '\n') {
				fprintf(stderr, "Error: Invalid character: <%c> in <%s>\n", *endptr, token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (errno == ERANGE) {
				fprintf(stderr, "Error: f%d.j is out of range\n", get_fl_last_idx(fl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (temp > 0) {
				f.j = vl.buffer[temp - 1];
			} else if (temp < 0) {
				f.j = vl.buffer[vertex_list_length(vl) + temp];
			} else {
				fprintf(stderr, "Error: f%d.j is out of range\n", get_fl_last_idx(fl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			
			token = strtok(NULL, " ");
			temp = strtol(token, &endptr, 10);
			if (endptr == token) {
				fprintf(stderr, "Error: No conversion could be performed on <%s>\n", token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (*endptr != '\0' && *endptr != '\n') {
				fprintf(stderr, "Error: Invalid character: <%c> in <%s>\n", *endptr, token);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (errno == ERANGE) {
				fprintf(stderr, "Error: f%d.i is out of range\n", get_fl_last_idx(fl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			if (temp > 0) {
				f.k = vl.buffer[temp - 1];
			} else if (temp < 0) {
				f.k = vl.buffer[vertex_list_length(vl) + temp];
			} else {
				fprintf(stderr, "Error: f%d.i is out of range\n", get_fl_last_idx(fl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			
			token = strtok(NULL, " ");
			if (token != NULL) {
				fprintf(stderr, "Error: f%d has more than 3 arguments\n", get_fl_last_idx(fl) + 1);
				fclose(objptr);
				deallocate_vertex_list(&vl);
				deallocate_face_list(&fl);
				return EXIT_FAILURE;
			}
			insert_fl_last(&fl, f);
		}
	}
	
	fclose(objptr);
	
	printf("\nvl length: %d\n", vertex_list_length(vl));
	
	double min_vx = vl.buffer[0].x;
	double min_vy = vl.buffer[0].y;
	double min_vz = vl.buffer[0].z;
	double max_vx = vl.buffer[0].x;
	double max_vy = vl.buffer[0].y;
	double max_vz = vl.buffer[0].z;
	for (int i = 0; i < vertex_list_length(vl); i++) {
		if (vl.buffer[i].x < min_vx) min_vx = vl.buffer[i].x;
		if (vl.buffer[i].x > max_vx) max_vx = vl.buffer[i].x;
		if (vl.buffer[i].y < min_vy) min_vy = vl.buffer[i].y;
		if (vl.buffer[i].y > max_vy) max_vy = vl.buffer[i].y;
		if (vl.buffer[i].z < min_vz) min_vz = vl.buffer[i].z;
		if (vl.buffer[i].z > max_vz) max_vz = vl.buffer[i].z;
	}
	
	if (min_vx < 0) {
		max_vx -= min_vx - 1;
		for (int i = 0; i < vertex_list_length(vl); i++) {
			vl.buffer[i].x -= min_vx - 0.01;
		}
		for (int i = 0; i < face_list_length(fl); i++) {
			fl.buffer[i].i.x -= min_vx - 0.01;
			fl.buffer[i].j.x -= min_vx - 0.01;
			fl.buffer[i].k.x -= min_vx - 0.01;
		}
	}
	
	if (min_vy < 0) {
		max_vy -= min_vy - 1;
		for (int i = 0; i < vertex_list_length(vl); i++) {
			vl.buffer[i].y -= min_vy - 0.01;
		}
		for (int i = 0; i < face_list_length(fl); i++) {
			fl.buffer[i].i.y -= min_vy - 0.01;
			fl.buffer[i].j.y -= min_vy - 0.01;
			fl.buffer[i].k.y -= min_vy - 0.01;
		}
	}
	
	if (min_vz < 0) {
		max_vz -= min_vz - 1;
		for (int i = 0; i < vertex_list_length(vl); i++) {
			vl.buffer[i].z -= min_vz - 0.01;
		}
		for (int i = 0; i < face_list_length(fl); i++) {
			fl.buffer[i].i.z -= min_vz - 0.01;
			fl.buffer[i].j.z -= min_vz - 0.01;
			fl.buffer[i].k.z -= min_vz - 0.01;
		}
	}
	
	int max_depth;
	printf("Maximum depth: ");
	scanf("%d", &max_depth);
	
	//print_fl_list(fl);
	
	double l = 1;
	while (l < max_vx || l < max_vy || l < max_vz) l *= 2;
	printf("length: %.2lf\n", l);
	
	voxel *v = create_voxel(fl, l, 0, create_vertex(0, 0, 0));
	clock_t start_time = clock();
	voxellize(v, fl, max_depth);
	clock_t end_time = clock();
	print_voxel_octree_stats();
	printf("Elapsed time: %.2lf ms\n\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);
	
	objptr = fopen("output.obj", "w");
	
	print_voxel_octree_to_file(*v, objptr);
	
	fclose(objptr);
	
	printf("Output path: ");
	#ifdef _WIN32
		run_on_windows();
		printf("\\");
	#elif defined(__linux__)
		run_on_linux();
		printf("/");
	#elif defined(__APPLE__) && defined(__MACH__)
		run_on_macos();
		printf("/");
	#endif
	printf("output.obj\n\n");
	
	deallocate_vertex_list(&vl);
	deallocate_face_list(&fl);
	return 0;
}