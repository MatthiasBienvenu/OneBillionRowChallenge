#include <assert.h>
#include <cmocka.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "fast_strtof.h"
#include "solution_hashmap.h"
#include "vector_generic.h"

const float max_load_factor = 0.75;

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

int hashmap_update(hashmap *map, const char key[MAX_LINE_LENGTH],
                   size_t key_len, size_t hash, float temperature) {

    city_vec *bucket = &map->buckets[hash % map->len];

    for (size_t i = 0; i < bucket->len; i++) {
        city *city = &bucket->data[i];

        if (city->hash == hash && memcmp(city->name, key, key_len) == 0) {
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
    strncpy(city.name, key, MAX_LINE_LENGTH);

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

size_t process_file(hashmap *map, int fd) {
    uint64_t measurements = 0;

    char buffer[IO_BUFFER_SIZE];
    size_t left_over = 0;
    ssize_t bytes_read;

    char *cur_ptr = buffer;
    char *key_start;

    while ((bytes_read =
                read(fd, buffer + left_over, IO_BUFFER_SIZE - left_over)) > 0) {
        size_t total = left_over + bytes_read;
        char *parse_end = buffer + total;

        // if not the last block, we take a margin
        if (total == IO_BUFFER_SIZE) {
            parse_end -= MAX_LINE_LENGTH;
        }

        // parse and treat the lines
        for (cur_ptr = buffer; cur_ptr < parse_end; measurements++) {
            key_start = cur_ptr;

            size_t hash = hash_fn(cur_ptr, &cur_ptr);

            size_t key_len = cur_ptr - key_start + 1;

            // end the string at the separator
            //*cur_ptr = '\0';
            cur_ptr++;
            float temperature = fast_strtof(cur_ptr, &cur_ptr);

            if (*cur_ptr != '\n') {
                // no valid float could be parsed
                printf("ok cool no \\n at %ld", measurements);

                return 0;
            }
            cur_ptr++;

            hashmap_update(map, key_start, key_len, hash, temperature);
        }

        // copy the left over to the beginning of the buffer
        left_over = buffer + total - cur_ptr;
        memcpy(buffer, cur_ptr, left_over);
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

    for (c = key; *c != ';'; c++) {
        h ^= (unsigned char)*c;
        h *= 1099511628211; // FNV prime
    }

    *sepptr = (char *)c;
    return h;
}
