#include <stdio.h>
#include <stdlib.h>
#include "vertex_list.h"

/* ********** KONSTRUKTOR ********** */
void create_vertex_list(vertex_list *l, int capacity) {
    l->buffer = (vertex *) malloc(capacity * sizeof(vertex));
    if (l->buffer != NULL) {
        l->capacity = capacity;
    } else {
        l->capacity = 0;  
    }
	l->nEff = 0;
}

void deallocate_vertex_list(vertex_list *l) {
    if (l->buffer != NULL) {
        free(l->buffer);
        l->buffer = NULL;
    }
    l->capacity = 0;
    l->nEff = 0;
}

/* ********** SELEKTOR (TAMBAHAN) ********** */
int vertex_list_length(vertex_list l) {
    return l.nEff;
}

int get_vl_first_idx(vertex_list l) {
    return (l.nEff > 0) ? 0 : IDX_UNDEF;
}

int get_vl_last_idx(vertex_list l) {
    return (l.nEff > 0) ? (l.nEff - 1) : IDX_UNDEF;
}

/* ********** Test Indeks yang valid ********** */
bool is_vl_idx_valid(vertex_list l, int i) {
    return (i >= 0 && i < l.capacity);
}

bool is_vl_idx_eff(vertex_list l, int i) {
    return (i >= 0 && i < l.nEff);
}

/* ********** TEST KOSONG/PENUH ********** */
bool is_vl_empty(vertex_list l) {
    return (l.nEff == 0);
}

bool is_vl_full(vertex_list l) {
    return (l.nEff == l.capacity);
}

/* ********** BACA dan TULIS ********** */
// void read_vl_list(vertex_list *l) {
    // int N;
    // do {
        // scanf("%d", &N);
    // } while (N < 0 || N > l->capacity);

    // l->nEff = N;
    // for (int i = 0; i < N; i++) {
        // scanf("%d", &(l->buffer[i]));
    // }
// }

void print_vl_list(vertex_list l) {
    printf("[====> Vertices <====]\n");
    for (int i = 0; i < l.nEff; i++) {
        printf("Vertex %d: x:%.2lf y:%.2lf z:%.2lf\n", i + 1, l.buffer[i].x, l.buffer[i].y, l.buffer[i].z);
    }
    printf("[====> <======> <====]\n");
}

/* ********** MENAMBAH DAN MENGHAPUS ELEMEN DI AKHIR ********** */
void insert_vl_last(vertex_list *l, vertex val) {
	if (is_vl_full(*l)) {
		expand_vl_list(l, l->capacity);
	}
    if (!is_vl_full(*l)) {
        l->buffer[l->nEff] = val;
        l->nEff++;
    }
    // Jika penuh, bisa ditambahkan mekanisme expand_vl_list jika diinginkan
}

void delete_vl_last(vertex_list *l, vertex *val) {
    if (!is_vl_empty(*l)) {
        *val = l->buffer[l->nEff - 1];
        l->nEff--;
		if (2 * l->nEff < l->capacity) {
			shrink_vl_list(l, l->capacity / 2);
		}
    }
}

/* ********* MENGUBAH UKURAN ARRAY ********* */
void expand_vl_list(vertex_list *l, int num) {
    int newCapacity = l->capacity + num;
    vertex *newBuffer = (vertex *) realloc(l->buffer, newCapacity * sizeof(vertex));
    if (newBuffer != NULL) {
        l->buffer = newBuffer;
        l->capacity = newCapacity;
    }
    // Jika realloc gagal, kapasitas dan buffer tetap sama
}

void shrink_vl_list(vertex_list *l, int num) {
    if (num < l->capacity && l->nEff <= l->capacity - num) {
        int newCapacity = l->capacity - num;
        vertex *newBuffer = (vertex *) realloc(l->buffer, newCapacity * sizeof(vertex));
        if (newBuffer != NULL) {
            l->buffer = newBuffer;
            l->capacity = newCapacity;
        }
        // Jika realloc gagal, kapasitas dan buffer tetap sama
    }
}

void compress_vl_list(vertex_list *l) {
    if (l->nEff > 0) {
        vertex *newBuffer = (vertex *) realloc(l->buffer, l->nEff * sizeof(vertex));
        if (newBuffer != NULL) {
            l->buffer = newBuffer;
            l->capacity = l->nEff;
        }
        // Jika realloc gagal, kapasitas dan buffer tetap sama
    }
}