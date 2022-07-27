#pragma once
#include <stdlib.h>
#include "unja_vector.h"

/* create a new unja_vector of the given capacity */
struct unja_vector* unja_vector_new(int cap) {
    struct unja_vector *l = malloc(sizeof *l);
    l->size = 0;
    l->cap = cap;
    l->values = malloc(l->cap * sizeof *l->values);
    return l;
}

/* push a new value to the end of the unja_vector's memory */
int unja_vector_push(struct unja_vector *vec, void *value) {
    vec->values[vec->size++] = value;
    return vec->size - 1;
}

/* free unja_vector related memory */
void unja_vector_free(struct unja_vector *l) {
    free(l->values);
    free(l);
}
