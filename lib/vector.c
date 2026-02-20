#include "vector.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int vec_init(struct vec *v, size_t element_size) {
    v->data = malloc(element_size);
    v->element_size = element_size;
    v->capacity = 1;
    v->len = 0;

    return v->data == NULL;
}

int vec_push(struct vec *v, void *value) {
    if (v->capacity < ++v->len) {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity);

        if (v->data == NULL) {
            return 1;
        }
    }

    memcpy(v->data + v->element_size * (v->len - 1), value, v->element_size);

    return 0;
}

int vec_pop(struct vec *v, void *dest) {
    if (v->len == 0) {
        return 1;
    }

    v->len--;
    memcpy(dest, v->data + v->element_size * v->len, v->element_size);

    return 0;
}

int vec_from_array(struct vec *v, void *array, size_t element_size,
                   size_t len) {
    v->data = malloc(element_size * len);
    v->element_size = element_size;
    memcpy(v->data, array, element_size * len);
    v->len = len;
    v->capacity = len;

    return v->data == NULL;
}
