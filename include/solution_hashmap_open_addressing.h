#pragma once

// #include <stddef.h>
#include <stdio.h>

#define MAX_LINE_LENGTH ((size_t)32)
#define IO_BUFFER_SIZE ((size_t)4096)

typedef struct {
    char name[MAX_LINE_LENGTH];
    size_t name_len;
    size_t hash;
    size_t count;
    float min_temp;
    float max_temp;
    float total_temp;
} city;

typedef struct {
    city *cities; // array of elements
    size_t len;   // length of the cities array
    size_t count; // number of elements stored in the hashmap
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
 */
int hashmap_update(hashmap *map, const char key[MAX_LINE_LENGTH],
                   size_t key_len, size_t hash, float temperature);

/* Merge map2 into map1.
 * map2 is not modified.
 */
int hashmap_merge(hashmap *map1, hashmap *map2);

/* Search for a city and update its temperature statistics.
 * If the city is unknown, it is created.
 */
int hashmap_update_with_city(hashmap *map, city *old_city);

/* Process the file and store the result in map.
 * Internally this function will launch multiple executions of
 * process_chunk_thread and then merge the hashmaps into map
 */
size_t process_file(hashmap *map, int fd);

typedef struct {
    hashmap *map;
    size_t *measurements;
    off_t len;
    off_t offset;
    int fd;
} process_chunk_thread_arg;

/* Process chunk of the file and store the result in map. */
void *process_chunk_thread(void *_arg);

/* Print a city in json format. */
void print_city(FILE *output_stream, const city *city);

/* write the list of cities and their data (in JSON) to output_file. */
void print_cities(hashmap *map, FILE *output_stream);
