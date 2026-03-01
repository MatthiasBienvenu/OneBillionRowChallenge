#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DECLARE_VEC(type)                                                      \
    typedef struct {                                                           \
        type *data;                                                            \
        size_t capacity;                                                       \
        size_t len;                                                            \
    } type##_vec;                                                              \
                                                                               \
    /* Intialize the vector. */                                                \
    int type##_vec##_init(type##_vec *v);                                      \
                                                                               \
    /* Push a value at the end of the vector. */                               \
    int type##_vec##_push(type##_vec *v, const type *value);                   \
                                                                               \
    /* Remove the last element from the                                        \
     * vector and copy it to value.                                            \
     * /!\ Does not realloc to a smaller                                       \
     * sized vec but still perform a copy.                                     \
     */                                                                        \
    int type##_vec##_pop(type##_vec *v, type *dest);                           \
                                                                               \
    /* Create a vector from an array.                                          \
     * /!\ Performs a copy of the array.                                       \
     */                                                                        \
    int type##_vec##_from_array(type##_vec *v, const void *array, size_t len);

#define IMPLEMENT_VEC(type)                                                    \
    int type##_vec##_init(type##_vec *v) {                                     \
        v->data = malloc(sizeof(type));                                        \
        v->capacity = 1;                                                       \
        v->len = 0;                                                            \
                                                                               \
        return v->data == NULL;                                                \
    }                                                                          \
                                                                               \
    int type##_vec##_push(type##_vec *v, const type *value) {                  \
        if (v->capacity < ++v->len) {                                          \
            v->capacity *= 2;                                                  \
            v->data = realloc(v->data, v->capacity * sizeof(type));            \
                                                                               \
            if (v->data == NULL) {                                             \
                return 1;                                                      \
            }                                                                  \
        }                                                                      \
                                                                               \
        memcpy(&v->data[v->len - 1], value, sizeof(type));                     \
                                                                               \
        return 0;                                                              \
    }                                                                          \
                                                                               \
    int type##_vec##_pop(type##_vec *v, type *dest) {                          \
        if (v->len == 0) {                                                     \
            return 1;                                                          \
        }                                                                      \
                                                                               \
        v->len--;                                                              \
        memcpy(dest, &v->data[v->len], sizeof(type));                          \
                                                                               \
        return 0;                                                              \
    }                                                                          \
                                                                               \
    int type##_vec##_from_array(type##_vec *v, const void *array,              \
                                size_t len) {                                  \
        v->data = malloc(sizeof(type) * len);                                  \
        memcpy(v->data, array, sizeof(type) * len);                            \
        v->len = len;                                                          \
        v->capacity = len;                                                     \
                                                                               \
        return v->data == NULL;                                                \
    }
