#pragma once

/* hahsmap lib */

#include "vector.h"
#include <stddef.h>

struct hashmap {
    vector *data;
    size_t element_size;
    size_t (*hash)(void *);
}
