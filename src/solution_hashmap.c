#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "solution_hashmap.h"
#include "vector_generic.h"

#define MAX_LINE_LENGTH 32

IMPLEMENT_VEC(city);

int hashmap_init(hashmap *map) {
    map->buckets = malloc(sizeof(city_vec));
    if (map->buckets == NULL) {
        return 1;
    }
    map->len = 1;

    city_vec_init(map->buckets);

    return map->buckets == NULL;
}

int hashmap_update(hashmap *map, const char key[CITY_NAME_MAX_LEN],
                   float temperature) {
    size_t h = hash(key) % map->len;
    city_vec *bucket = &map->buckets[h];

    for (size_t i = 0; i < bucket->len; i++) {
        city *city = &bucket->data[i];

        if (strcmp(city->name, key) == 0) {
            // update the city
            city->min_temp = fminf(city->min_temp, temperature);
            city->max_temp = fmaxf(city->max_temp, temperature);
            city->total_temp += temperature;
            city->count++;
            city->mean_temp = city->total_temp / city->count;

            return 0;
        }
    }

    // add a new city
    city city = {.min_temp = temperature,
                 .max_temp = temperature,
                 .total_temp = temperature,
                 .mean_temp = temperature,
                 .count = 1};
    strcpy(city.name, key);
    return city_vec_push(bucket, &city);
}

size_t process_stream(hashmap *map, FILE *input_stream) {
    size_t measurements = 0;
    char buffer[MAX_LINE_LENGTH];
    char *endptr;
    float temperature;

    while (fgets(buffer, MAX_LINE_LENGTH, input_stream)) {
        measurements++;

        endptr = strchr(buffer, ';');
        if (endptr == NULL) {
            return 0;
        }

        // end the string at the separator so that it can be directly passed
        // to oneb_challenge_update_city
        *endptr = '\0';
        temperature = strtof(endptr + 1, &endptr);
        if (*(endptr - 1) == '\0') {
            // no valid float could be parsed
            return 0;
        }

        hashmap_update(map, buffer, temperature);
    }

    return measurements;
}

void print_city(FILE *output_stream, city *city) {
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
size_t hash(const char key[MAX_LINE_LENGTH]) {
    size_t h = 1469598103934665603; // FNV offset basis
    for (int i = 0; i < MAX_LINE_LENGTH; i++) {
        h ^= (unsigned char)key[i];
        h *= 1099511628211; // FNV prime
    }
    return h;
}
