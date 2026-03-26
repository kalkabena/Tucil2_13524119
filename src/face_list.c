#include <stdio.h>
#include <stdlib.h>
#include "face_list.h"

/* ********** KONSTRUKTOR ********** */
void create_face_list(face_list *l, int capacity) {
    l->buffer = (face *) malloc(capacity * sizeof(face));
    if (l->buffer != NULL) {
        l->capacity = capacity;
    } else {
        l->capacity = 0;  
    }
	l->nEff = 0;
}

void deallocate_face_list(face_list *l) {
    if (l->buffer != NULL) {
        free(l->buffer);
        l->buffer = NULL;
    }
    l->capacity = 0;
    l->nEff = 0;
}

/* ********** SELEKTOR (TAMBAHAN) ********** */
int face_list_length(face_list l) {
    return l.nEff;
}

int get_fl_first_idx(face_list l) {
    return (l.nEff > 0) ? 0 : IDX_UNDEF;
}

int get_fl_last_idx(face_list l) {
    return (l.nEff > 0) ? (l.nEff - 1) : IDX_UNDEF;
}

/* ********** Test Indeks yang valid ********** */
bool is_fl_idx_valid(face_list l, int i) {
    return (i >= 0 && i < l.capacity);
}

bool is_fl_idx_eff(face_list l, int i) {
    return (i >= 0 && i < l.nEff);
}

/* ********** TEST KOSONG/PENUH ********** */
bool is_fl_empty(face_list l) {
    return (l.nEff == 0);
}

bool is_fl_full(face_list l) {
    return (l.nEff == l.capacity);
}

/* ********** BACA dan TULIS ********** */
// void read_fl_list(face_list *l) {
    // int N;
    // do {
        // scanf("%d", &N);
    // } while (N < 0 || N > l->capacity);

    // l->nEff = N;
    // for (int i = 0; i < N; i++) {
        // scanf("%d", &(l->buffer[i]));
    // }
// }

void print_fl_list(face_list l) {
    printf("[=====> Faces <=====]\n");
    for (int i = 0; i < l.nEff; i++) {
        printf("Face %d: i:(%lf, %lf, %lf), j:(%lf, %lf, %lf), k:(%lf, %lf, %lf)\n", i + 1, l.buffer[i].i.x, l.buffer[i].i.y, l.buffer[i].i.z, l.buffer[i].j.x, l.buffer[i].j.y, l.buffer[i].j.z, l.buffer[i].k.x, l.buffer[i].k.y, l.buffer[i].k.z);
    }
    printf("[=====> <===> <=====]\n");
}

/* ********** MENAMBAH DAN MENGHAPUS ELEMEN DI AKHIR ********** */
void insert_fl_last(face_list *l, face val) {
	if (is_fl_full(*l)) {
		expand_fl_list(l, l->capacity);
	}
    if (!is_fl_full(*l)) {
        l->buffer[l->nEff] = val;
        l->nEff++;
    }
    // Jika penuh, bisa ditambahkan mekanisme expand_fl_list jika diinginkan
}

void delete_fl_last(face_list *l, face *val) {
    if (!is_fl_empty(*l)) {
        *val = l->buffer[l->nEff - 1];
        l->nEff--;
		if (2 * l->nEff < l->capacity) {
			shrink_fl_list(l, l->capacity / 2);
		}
    }
}

/* ********* MENGUBAH UKURAN ARRAY ********* */
void expand_fl_list(face_list *l, int num) {
    int newCapacity = l->capacity + num;
    face *newBuffer = (face *) realloc(l->buffer, newCapacity * sizeof(face));
    if (newBuffer != NULL) {
        l->buffer = newBuffer;
        l->capacity = newCapacity;
    }
    // Jika realloc gagal, kapasitas dan buffer tetap sama
}

void shrink_fl_list(face_list *l, int num) {
    if (num < l->capacity && l->nEff <= l->capacity - num) {
        int newCapacity = l->capacity - num;
        face *newBuffer = (face *) realloc(l->buffer, newCapacity * sizeof(face));
        if (newBuffer != NULL) {
            l->buffer = newBuffer;
            l->capacity = newCapacity;
        }
        // Jika realloc gagal, kapasitas dan buffer tetap sama
    }
}

void compress_fl_list(face_list *l) {
    if (l->nEff > 0) {
        face *newBuffer = (face *) realloc(l->buffer, l->nEff * sizeof(face));
        if (newBuffer != NULL) {
            l->buffer = newBuffer;
            l->capacity = l->nEff;
        }
        // Jika realloc gagal, kapasitas dan buffer tetap sama
    }
}