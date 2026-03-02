#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fast_strtof.h"
#include "solution_hashmap.h"
#include "vector_generic.h"

IMPLEMENT_VEC(city);

int hashmap_init(hashmap *map) {
    map->buckets = malloc(sizeof(city_vec));
    if (map->buckets == NULL) {
        return 1;
    }
    map->len = 1;
    map->count = 0;

    city_vec_init(map->buckets);

    return map->buckets == NULL;
}

int hashmap_update(hashmap *map, const char key[MAX_LINE_LENGTH], size_t hash,
                   float temperature) {

    city_vec *bucket = &map->buckets[hash % map->len];

    for (size_t i = 0; i < bucket->len; i++) {
        city *city = &bucket->data[i];

        if (city->hash == hash && strcmp(city->name, key) == 0) {
            // update the city
            city->min_temp = fminf(city->min_temp, temperature);
            city->max_temp = fmaxf(city->max_temp, temperature);
            city->total_temp += temperature;
            city->count++;
            city->mean_temp = city->total_temp / city->count;

            return 0;
        }
    }

    //  add a new city
    city city = {.hash = hash,
                 .min_temp = temperature,
                 .max_temp = temperature,
                 .total_temp = temperature,
                 .mean_temp = temperature,
                 .count = 1};
    strcpy(city.name, key);

    // maybe increase the hashmap
    if (++map->count > map->len * max_load_factor) {
        if (hashmap_double_size(map)) {
            return 1;
        }
        bucket = &map->buckets[hash % map->len];
    }

    return city_vec_push(bucket, &city);
}

int hashmap_double_size(hashmap *map) {
    city_vec *old_buckets = map->buckets;
    const size_t new_len = map->len * 2;

    map->buckets = malloc(new_len * sizeof(city_vec));
    if (map->buckets == NULL) {
        return 1;
    }

    for (size_t i = 0; i < new_len; i++) {
        if (city_vec_init(&map->buckets[i])) {
            return 1;
        }
    }

    for (size_t i = 0; i < map->len; i++) {
        const city_vec *old_bucket = &old_buckets[i];

        for (size_t j = 0; j < old_bucket->len; j++) {
            const city *city = &old_bucket->data[j];
            city_vec *new_bucket = &map->buckets[city->hash % new_len];

            if (city_vec_push(new_bucket, city)) {
                return 1;
            };
        }
    }

    map->len = new_len;

    free(old_buckets);
    return 0;
}

size_t process_stream(hashmap *map, FILE *input_stream) {
    size_t measurements = 0;
    char buffer[MAX_LINE_LENGTH];
    char *endptr;
    float temperature;
    size_t hash;

    while (fgets(buffer, MAX_LINE_LENGTH, input_stream)) {
        measurements++;

        hash = hash_fn(buffer, &endptr);
        if (*endptr == '\0') {
            return 0;
        }

        // end the string at the separator so that it can be directly passed
        *endptr = '\0';
        endptr++;
        temperature = fast_strtof(endptr, &endptr);

        if (*endptr != '\n') {
            // no valid float could be parsed
            return 0;
        }

        hashmap_update(map, buffer, hash, temperature);
    }

    return measurements;
}

void print_city(FILE *output_stream, const city *city) {
    fprintf(output_stream,
            "{\"city\":\"%s\",\"min\":%.1f,\"max\":%.1f,\"mean\":%.1f,"
            "\"total\":%.1f,\"count\":%d}",
            city->name, city->min_temp, city->max_temp, city->mean_temp,
            city->total_temp, city->count);
}

void print_cities(hashmap *map, FILE *output_stream) {
    fputs("[", output_stream);

    int first = 1; // Track if this is the first city

    for (size_t b = 0; b < map->len; b++) {
        city_vec *bucket = &map->buckets[b];

        for (size_t i = 0; i < bucket->len; i++) {
            city *city = &bucket->data[i];

            if (!first) {
                fputs(",", output_stream);
            }
            first = 0;

            print_city(output_stream, city);
        }
    }

    fputs("]", output_stream);
}

/* fnv1a_64 */
size_t hash_fn(const char key[MAX_LINE_LENGTH], char **sepptr) {
    size_t h = 1469598103934665603; // FNV offset basis
    const char *c;

    for (c = key; *c && *c != ';'; c++) {
        h ^= (unsigned char)*c;
        h *= 1099511628211; // FNV prime
    }

    *sepptr = (char *)c;
    return h;
}
