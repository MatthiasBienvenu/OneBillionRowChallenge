#pragma once

#include "vector_generic.h"
#include <stddef.h>
#include <stdio.h>

#define MAX_LINE_LENGTH (size_t)32
#define IO_BUFFER_SIZE (size_t)4096

typedef struct {
    char name[MAX_LINE_LENGTH];
    size_t hash;
    float min_temp;
    float max_temp;
    float total_temp;
    float mean_temp;
    int count;
} city;

// city_vec
// need to call IMPLEMENT_VEC(city) in a .c file later
DECLARE_VEC(city);

typedef struct {
    city_vec *buckets; // vectors that store elements with keys of same hash
    size_t len;        // length of the buckets array
    size_t count;      // number of elements stored in the hashmap
} hashmap;

/* Hash the key until finding a ';'.
 * Then, set *sepptr to be a reference of that separator.
 */
size_t hash_fn(const char key[MAX_LINE_LENGTH], char **sepptr);

/* Initialize the hashmap. */
int hashmap_init(hashmap *map);

/* Doubles the size of the map */
int hashmap_double_size(hashmap *map);

/* Search for a city and update its temperature statistics.
 * If the city is unknown, it is created.
 * This function returns the struct city that was created/updated.
 */
int hashmap_update(hashmap *map, const char key[MAX_LINE_LENGTH],
                   size_t key_len, size_t hash, float temperature);

/* Process the file and store the result in map. */
size_t process_file(hashmap *map, int fd);

/* Print a city in json format. */
void print_city(FILE *output_stream, const city *city);

/* write the list of cities and their data (in JSON) to output_file. */
void print_cities(hashmap *map, FILE *output_stream);
