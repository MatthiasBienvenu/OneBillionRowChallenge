#pragma once

#include <stddef.h>

struct vec {
    void *data;
    size_t element_size;
    size_t capacity;
    size_t len;
};

/* Intialize the vector. */
int vec_init(struct vec *v, size_t element_size);

/* Push a value at the end of the vector. */
int vec_push(struct vec *v, void *element);

/* Remove the last element from the vector and copy it to value.
 * /!\ Does not realloc to a smaller sized vec but still perform a copy.
 */
int vec_pop(struct vec *v, void *dest);

/* Create a vector from an array.
 * /!\ Performs a copy of the array.
 */
int vec_from_array(struct vec *v, void *array, size_t element_size, size_t len);
