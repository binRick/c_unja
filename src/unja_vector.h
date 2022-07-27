#pragma once
#include <stdlib.h>

struct unja_vector {
    void **values;
    int size;
    int cap;
};

struct unja_vector* unja_vector_new(int cap);
int unja_vector_push(struct unja_vector *vec, void *value);
void unja_vector_free(struct unja_vector *vec);
