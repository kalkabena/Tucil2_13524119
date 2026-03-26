#include "fenster.h"
#include "vertex_list.h"
#include "face_list.h"
#include <math.h>
#include <stdio.h>

#define W 640
#define H 480

const double PI = 3.14159265358979323846;

//compile: gcc -o test inputtest.c vertex.c face.c vertex_list.c face_list.c -mwindows

vertex_list vl;
face_list fl;

int comp(const void *a, const void *b) {
    return fmax(fmax(((face *)a)->i.z, ((face *)a)->j.z), ((face *)a)->k.z) - fmax(fmax(((face *)b)->i.z, ((face *)b)->j.z), ((face *)b)->k.z);
}

static void fenster_line(struct fenster *f, int x0, int y0, int x1, int y1,
                         uint32_t c) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = (dx > dy ? dx : -dy) / 2, e2;
  for (;;) {
    if (x0 >= 0 && x0 < W && y0 >= 0 && y0 < H) fenster_pixel(f, x0, y0) = c;
    if (x0 == x1 && y0 == y1) {
      break;
    }
    e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

static void fenster_line_4tri(struct fenster *f, int x0, int y0, int x1, int y1,
                         uint32_t c, char tempbuf[]) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = (dx > dy ? dx : -dy) / 2, e2;
  for (;;) {
    if (x0 >= 0 && x0 < W && y0 >= 0 && y0 < H) {
		fenster_pixel(f, x0, y0) = c;
		tempbuf[y0 * W + x0] = 1;
	}
    if (x0 == x1 && y0 == y1) {
      break;
    }
    e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

static void fenster_triangle(struct fenster *f, face tri, uint32_t c) {
	char tempbuf[W * H] = {0};
	fenster_line_4tri(f, (int)tri.i.x, (int)tri.i.y, (int)tri.j.x, (int)tri.j.y, 0xffffff, tempbuf);
	fenster_line_4tri(f, (int)tri.j.x, (int)tri.j.y, (int)tri.k.x, (int)tri.k.y, 0xffffff, tempbuf);
	fenster_line_4tri(f, (int)tri.k.x, (int)tri.k.y, (int)tri.i.x, (int)tri.i.y, 0xffffff, tempbuf);
	//flood fill
}

static void fenster_rect(struct fenster *f, int x, int y, int w, int h,
                         uint32_t c) {
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      fenster_pixel(f, x + col, y + row) = c;
    }
  }
}

// clang-format off
static uint16_t font5x3[] = {0x0000,0x2092,0x002d,0x5f7d,0x279e,0x52a5,0x7ad6,0x0012,0x4494,0x1491,0x017a,0x05d0,0x1400,0x01c0,0x0400,0x12a4,0x2b6a,0x749a,0x752a,0x38a3,0x4f4a,0x38cf,0x3bce,0x12a7,0x3aae,0x49ae,0x0410,0x1410,0x4454,0x0e38,0x1511,0x10e3,0x73ee,0x5f7a,0x3beb,0x624e,0x3b6b,0x73cf,0x13cf,0x6b4e,0x5bed,0x7497,0x2b27,0x5add,0x7249,0x5b7d,0x5b6b,0x3b6e,0x12eb,0x4f6b,0x5aeb,0x388e,0x2497,0x6b6d,0x256d,0x5f6d,0x5aad,0x24ad,0x72a7,0x6496,0x4889,0x3493,0x002a,0xf000,0x0011,0x6b98,0x3b79,0x7270,0x7b74,0x6750,0x95d6,0xb9ee,0x5b59,0x6410,0xb482,0x56e8,0x6492,0x5be8,0x5b58,0x3b70,0x976a,0xcd6a,0x1370,0x38f0,0x64ba,0x3b68,0x2568,0x5f68,0x54a8,0xb9ad,0x73b8,0x64d6,0x2492,0x3593,0x03e0};
// clang-format on
static void fenster_text(struct fenster *f, int x, int y, char *s, int scale,
                         uint32_t c) {
  while (*s) {
    char chr = *s++;
    if (chr > 32) {
      uint16_t bmp = font5x3[chr - 32];
      for (int dy = 0; dy < 5; dy++) {
        for (int dx = 0; dx < 3; dx++) {
          if (bmp >> (dy * 3 + dx) & 1) {
            fenster_rect(f, x + dx * scale, y + dy * scale, scale, scale, c);
          }
        }
      }
    }
    x = x + 4 * scale;
  }
}

/* ============================================================
 * A small example demostrating keymaps/keycodes:
 * - On all platforms keys usually correspond to upper-case ASCII
 * - Enter code is 10, Tab is 9, Backspace is 8, Escape is 27
 * - Delete is 127, Space is 32
 * - Modifiers are: Ctrl=1, Shift=2, Ctrl+Shift=3
 *
 * This demo prints currently pressed keys with modifiers.
 * ============================================================ */
static int run() {
  uint32_t buf[W * H] = {0};
  struct fenster f = {
      .title = "Press any key...",
      .width = W,
      .height = H,
      .buf = buf,
	  .wheel = 0
  };
  fenster_open(&f);
  
  OPENFILENAMEA ofn = {0};
	char szFile[MAX_PATH] = {0};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = f.hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "OBJ Files\0*.obj\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = "Select OBJ File";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	
	double avg_vx = 0;
	long long int count_vx = 0;
	double avg_vy = 0;
	long long int count_vy = 0;
	double avg_vz = 0;
	long long int count_vz = 0;

	if (GetOpenFileNameA(&ofn)) {
		FILE *objptr = fopen(szFile, "r");
		if (objptr != NULL) {
			char obj_line[200];
			create_vertex_list(&vl, 100);
			create_face_list(&fl, 20);
			
			while(fgets(obj_line, 200, objptr)) {
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
			int n = sizeof(fl.buffer) / sizeof(fl.buffer[0]);
			
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
					vl.buffer[i].x -= min_vx - 1;
				}
				for (int i = 0; i < face_list_length(fl); i++) {
					fl.buffer[i].i.x -= min_vx - 1;
					fl.buffer[i].j.x -= min_vx - 1;
					fl.buffer[i].k.x -= min_vx - 1;
				}
			}
			
			if (min_vy < 0) {
				max_vy -= min_vy - 1;
				for (int i = 0; i < vertex_list_length(vl); i++) {
					vl.buffer[i].y -= min_vy - 1;
				}
				for (int i = 0; i < face_list_length(fl); i++) {
					fl.buffer[i].i.y -= min_vy - 1;
					fl.buffer[i].j.y -= min_vy - 1;
					fl.buffer[i].k.y -= min_vy - 1;
				}
			}
			
			if (min_vz < 0) {
				max_vz -= min_vz - 1;
				for (int i = 0; i < vertex_list_length(vl); i++) {
					vl.buffer[i].z -= min_vz - 1;
				}
				for (int i = 0; i < face_list_length(fl); i++) {
					fl.buffer[i].i.z -= min_vz - 1;
					fl.buffer[i].j.z -= min_vz - 1;
					fl.buffer[i].k.z -= min_vz - 1;
				}
			}
			
			double max_d = fmax(fmax(max_vx, max_vy), max_vz);
			
			qsort(fl.buffer, n, sizeof(face), comp);
			for (int i = 0; i < face_list_length(fl); i++) {
				double sc = H / max_d;
				fl.buffer[i].i.x *= sc;
				fl.buffer[i].i.y *= sc;
				fl.buffer[i].i.z *= sc;
				fl.buffer[i].j.x *= sc;
				fl.buffer[i].j.y *= sc;
				fl.buffer[i].j.z *= sc;
				fl.buffer[i].k.x *= sc;
				fl.buffer[i].k.y *= sc;
				fl.buffer[i].k.z *= sc;
			}
			
			for (int i = 0; i < face_list_length(fl); i++) {
				avg_vx = (avg_vx * count_vx + fl.buffer[i].i.x) / (count_vx + 1);
				count_vx++;
				avg_vx = (avg_vx * count_vx + fl.buffer[i].j.x) / (count_vx + 1);
				count_vx++;
				avg_vx = (avg_vx * count_vx + fl.buffer[i].k.x) / (count_vx + 1);
				count_vx++;
				avg_vy = (avg_vy * count_vy + fl.buffer[i].i.y) / (count_vy + 1);
				count_vy++;
				avg_vy = (avg_vy * count_vy + fl.buffer[i].j.y) / (count_vy + 1);
				count_vy++;
				avg_vy = (avg_vy * count_vy + fl.buffer[i].k.y) / (count_vy + 1);
				count_vy++;
				avg_vz = (avg_vz * count_vz + fl.buffer[i].i.z) / (count_vz + 1);
				count_vz++;
				avg_vz = (avg_vz * count_vz + fl.buffer[i].j.z) / (count_vz + 1);
				count_vz++;
				avg_vz = (avg_vz * count_vz + fl.buffer[i].k.z) / (count_vz + 1);
				count_vz++;
			}
			
			for (int i = 0; i < face_list_length(fl); i++) {
				fl.buffer[i].i.x += W / 2 - avg_vx;
				fl.buffer[i].j.x += W / 2 - avg_vx;
				fl.buffer[i].k.x += W / 2 - avg_vx;
				fl.buffer[i].i.y += H / 2 - avg_vy;
				fl.buffer[i].j.y += H / 2 - avg_vy;
				fl.buffer[i].k.y += H / 2 - avg_vy;
				fl.buffer[i].i.z += - avg_vz;
				fl.buffer[i].j.z += - avg_vz;
				fl.buffer[i].k.z += - avg_vz;
			}
		} else {
			exit(EXIT_FAILURE);
		}
	} else {
		exit(EXIT_FAILURE);
	}
  
  int64_t now = fenster_time();
  double scaling = 1;
  double x_angle = 0;
  double y_angle = 0;
  double z_angle = 0;

  while (fenster_loop(&f) == 0) {
	fmod(x_angle, 2 * PI);
	fmod(y_angle, 2 * PI);
	fmod(z_angle, 2 * PI);
	face_list newfl;
	create_face_list(&newfl, face_list_length(fl));
	for (int i = 0; i < face_list_length(fl); i++) {
		insert_fl_last(&newfl, fl.buffer[i]);
	}
	
	if (f.keys[88]) {
		x_angle += 0.05;
	}
	if (f.keys[90]) {
		x_angle -= 0.05;
	}
	if (f.keys[83]) {
		y_angle += 0.05;
	}
	if (f.keys[65]) {
		y_angle -= 0.05;
	}
	if (f.keys[87]) {
		z_angle += 0.05;
	}
	if (f.keys[81]) {
		z_angle -= 0.05;
	}
	if (f.wheel > 0) {
		scaling += 0.2;
		f.wheel = 0;
	} else if (f.wheel < 0 && scaling > 0.21) {
		scaling -= 0.2;
		f.wheel = 0;
	}
	
	for (int i = 0; i < face_list_length(newfl); i++) {
		newfl.buffer[i].i.x -= W / 2;
		newfl.buffer[i].j.x -= W / 2;
		newfl.buffer[i].k.x -= W / 2;
		newfl.buffer[i].i.y -= H / 2;
		newfl.buffer[i].j.y -= H / 2;
		newfl.buffer[i].k.y -= H / 2;
		
		face temp;
		temp.i.x = newfl.buffer[i].i.x;
		temp.j.x = newfl.buffer[i].j.x;
		temp.k.x = newfl.buffer[i].k.x;
		temp.i.z = newfl.buffer[i].i.y * sin(x_angle) + newfl.buffer[i].i.z * cos(x_angle);
		temp.i.y = newfl.buffer[i].i.y * cos(x_angle) - newfl.buffer[i].i.z * sin(x_angle);
		temp.j.z = newfl.buffer[i].j.y * sin(x_angle) + newfl.buffer[i].j.z * cos(x_angle);
		temp.j.y = newfl.buffer[i].j.y * cos(x_angle) - newfl.buffer[i].j.z * sin(x_angle);
		temp.k.z = newfl.buffer[i].k.y * sin(x_angle) + newfl.buffer[i].k.z * cos(x_angle);
		temp.k.y = newfl.buffer[i].k.y * cos(x_angle) - newfl.buffer[i].k.z * sin(x_angle);
		
		newfl.buffer[i] = temp;
		
		newfl.buffer[i].i.x += W / 2;
		newfl.buffer[i].j.x += W / 2;
		newfl.buffer[i].k.x += W / 2;
		newfl.buffer[i].i.y += H / 2;
		newfl.buffer[i].j.y += H / 2;
		newfl.buffer[i].k.y += H / 2;
	}
	
	for (int i = 0; i < face_list_length(newfl); i++) {
		newfl.buffer[i].i.x -= W / 2;
		newfl.buffer[i].j.x -= W / 2;
		newfl.buffer[i].k.x -= W / 2;
		newfl.buffer[i].i.y -= H / 2;
		newfl.buffer[i].j.y -= H / 2;
		newfl.buffer[i].k.y -= H / 2;
		
		face temp;
		temp.i.y = newfl.buffer[i].i.y;
		temp.j.y = newfl.buffer[i].j.y;
		temp.k.y = newfl.buffer[i].k.y;
		temp.i.x = newfl.buffer[i].i.z * sin(y_angle) + newfl.buffer[i].i.x * cos(y_angle);
		temp.i.z = newfl.buffer[i].i.z * cos(y_angle) - newfl.buffer[i].i.x * sin(y_angle);
		temp.j.x = newfl.buffer[i].j.z * sin(y_angle) + newfl.buffer[i].j.x * cos(y_angle);
		temp.j.z = newfl.buffer[i].j.z * cos(y_angle) - newfl.buffer[i].j.x * sin(y_angle);
		temp.k.x = newfl.buffer[i].k.z * sin(y_angle) + newfl.buffer[i].k.x * cos(y_angle);
		temp.k.z = newfl.buffer[i].k.z * cos(y_angle) - newfl.buffer[i].k.x * sin(y_angle);
		newfl.buffer[i] = temp;
		
		newfl.buffer[i].i.x += W / 2;
		newfl.buffer[i].j.x += W / 2;
		newfl.buffer[i].k.x += W / 2;
		newfl.buffer[i].i.y += H / 2;
		newfl.buffer[i].j.y += H / 2;
		newfl.buffer[i].k.y += H / 2;
	}
	
	for (int i = 0; i < face_list_length(newfl); i++) {
		newfl.buffer[i].i.x -= W / 2;
		newfl.buffer[i].j.x -= W / 2;
		newfl.buffer[i].k.x -= W / 2;
		newfl.buffer[i].i.y -= H / 2;
		newfl.buffer[i].j.y -= H / 2;
		newfl.buffer[i].k.y -= H / 2;
		
		face temp;
		temp.i.z = newfl.buffer[i].i.z;
		temp.j.z = newfl.buffer[i].j.z;
		temp.k.z = newfl.buffer[i].k.z;
		temp.i.y = newfl.buffer[i].i.x * sin(z_angle) + newfl.buffer[i].i.y * cos(z_angle);
		temp.i.x = newfl.buffer[i].i.x * cos(z_angle) - newfl.buffer[i].i.y * sin(z_angle);
		temp.j.y = newfl.buffer[i].j.x * sin(z_angle) + newfl.buffer[i].j.y * cos(z_angle);
		temp.j.x = newfl.buffer[i].j.x * cos(z_angle) - newfl.buffer[i].j.y * sin(z_angle);
		temp.k.y = newfl.buffer[i].k.x * sin(z_angle) + newfl.buffer[i].k.y * cos(z_angle);
		temp.k.x = newfl.buffer[i].k.x * cos(z_angle) - newfl.buffer[i].k.y * sin(z_angle);
		newfl.buffer[i] = temp;
		
		newfl.buffer[i].i.x += W / 2;
		newfl.buffer[i].j.x += W / 2;
		newfl.buffer[i].k.x += W / 2;
		newfl.buffer[i].i.y += H / 2;
		newfl.buffer[i].j.y += H / 2;
		newfl.buffer[i].k.y += H / 2;
	}
	
	for (int i = 0; i < face_list_length(newfl); i++) {
		
		newfl.buffer[i].i.x = W / 2 + (newfl.buffer[i].i.x - W / 2) * scaling;
		newfl.buffer[i].i.y = H / 2 + (newfl.buffer[i].i.y - H / 2) * scaling;
		newfl.buffer[i].j.x = W / 2 + (newfl.buffer[i].j.x - W / 2) * scaling;
		newfl.buffer[i].j.y = H / 2 + (newfl.buffer[i].j.y - H / 2) * scaling;
		newfl.buffer[i].k.x = W / 2 + (newfl.buffer[i].k.x - W / 2) * scaling;
		newfl.buffer[i].k.y = H / 2 + (newfl.buffer[i].k.y - H / 2) * scaling;
	}
	  
    int has_keys = 0;
    char s[32];
    char *p = s;
    for (int i = 0; i < 128; i++) {
      if (f.keys[i]) {
        has_keys = 1;
        *p++ = i;
      }
    }
    *p = '\0';
	
    fenster_rect(&f, 0, 0, W, H, 0);
	for (int i = 0; i < face_list_length(newfl); i++) {
		fenster_triangle(&f, newfl.buffer[i], 0xdddddd);
	}
    /* draw mouse "pointer" */
    if (f.x > 5 && f.y > 5 && f.x < f.width - 5 && f.y < f.height - 5) {
      fenster_rect(&f, f.x - 3, f.y - 3, 6, 6, f.mouse ? 0xffffff : 0xff0000);
    }
	char newflc[200];
	sprintf(newflc, "%d", f.wheel);
    fenster_text(&f, 8, 8, s, 4, 0xffffff);
    if (has_keys) {
      if (f.mod & 1) {
        fenster_text(&f, 8, 40, "Ctrl", 4, 0xffffff);
      }
      if (f.mod & 2) {
        fenster_text(&f, 8, 80, "Shift", 4, 0xffffff);
      }
    }
    if (f.keys[27]) {
      break;
    }
    int64_t time = fenster_time();
    if (time - now < 1000 / 60) {
      fenster_sleep(time - now);
    }
    now = time;
	deallocate_face_list(&newfl);
  }
  deallocate_vertex_list(&vl);
  deallocate_face_list(&fl);
  fenster_close(&f);
  return 0;
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
                   int nCmdShow) {
  (void)hInstance, (void)hPrevInstance, (void)pCmdLine, (void)nCmdShow;
	return run();
}
#else
int main() { return run(); }
#endif