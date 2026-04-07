#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fast_strtof.h"
#include "solution_hashmap_open_addressing.h"

const float max_load_factor = 0.5;

int hashmap_init(hashmap *map) {
    map->cities = calloc(1, sizeof(city));
    if (map->cities == NULL) {
        return 1;
    }
    map->len = 1;
    map->count = 0;

    return 0;
}

int hashmap_update(hashmap *map, const char key[MAX_LINE_LENGTH],
                   size_t key_len, size_t hash, float temperature) {

    size_t offset = 0;
    city *city = &map->cities[(hash + offset) & (map->len - 1)];

    while (city->name_len != 0) {
        if (city->name_len == key_len && city->hash == hash &&
            memcmp(city->name, key, key_len) == 0) {

            city->min_temp = fminf(city->min_temp, temperature);
            city->max_temp = fmaxf(city->max_temp, temperature);
            city->total_temp += temperature;
            city->count++;

            return 0;
        }

        offset++;
        city = &map->cities[(hash + offset) & (map->len - 1)];
    }

    // an empty space was found
    // so we need to create a new city

    // maybe increase the size of the hashmap
    if (++map->count > map->len * max_load_factor) {
        if (hashmap_double_size(map)) {
            return 1;
        }
        // the offset is no longer valid
        return hashmap_update(map, key, key_len, hash, temperature);
    }

    city->name_len = key_len;
    city->hash = hash;
    city->min_temp = temperature;
    city->max_temp = temperature;
    city->total_temp = temperature;
    city->count = 1;
    strncpy(city->name, key, MAX_LINE_LENGTH);

    return 0;
}

int hashmap_double_size(hashmap *map) {
    city *old_cities = map->cities;
    const size_t new_len = map->len * 2;

    map->cities = calloc(new_len, sizeof(city));
    if (map->cities == NULL) {
        return 1;
    }

    for (size_t i = 0; i < map->len; i++) {
        const city *old_city = &old_cities[i];

        if (old_city->name_len == 0) {
            continue;
        }

        size_t offset = 0;
        city *new_city =
            &map->cities[(old_city->hash + offset) & (new_len - 1)];

        while (new_city->name_len != 0) {
            offset++;
            new_city = &map->cities[(old_city->hash + offset) & (new_len - 1)];
        }

        memcpy(new_city, old_city, sizeof(city));
    }

    map->len = new_len;
    free(old_cities);
    return 0;
}

int hashmap_update_with_city(hashmap *map, city *old_city) {
    size_t hash = old_city->hash;
    size_t key_len = old_city->name_len;
    char *key = old_city->name;

    size_t offset = 0;
    city *city = &map->cities[(hash + offset) & (map->len - 1)];

    while (city->name_len != 0) {
        if (city->name_len == key_len && city->hash == hash &&
            memcmp(city->name, key, key_len) == 0) {

            city->min_temp = fminf(city->min_temp, old_city->min_temp);
            city->max_temp = fmaxf(city->max_temp, old_city->max_temp);
            city->total_temp += old_city->total_temp;
            city->count += old_city->count;

            return 0;
        }

        offset++;
        city = &map->cities[(hash + offset) & (map->len - 1)];
    }

    // an empty space was found
    // so we need to create a new city

    // maybe increase the size of the hashmap
    if (++map->count > map->len * max_load_factor) {
        if (hashmap_double_size(map)) {
            return 1;
        }
        // the offset is no longer valid
        return hashmap_update_with_city(map, old_city);
    }

    memcpy(city, old_city, sizeof(*old_city));

    return 0;
}

int hashmap_merge(hashmap *map1, hashmap *map2) {
    for (size_t i = 0; i < map2->len; i++) {
        city *city = &map2->cities[i];

        if (city->name_len == 0) {
            continue;
        }

        if (hashmap_update_with_city(map1, city)) {
            return 1;
        }
    }

    return 0;
}

size_t process_file(hashmap *map, int fd) {
    pthread_t threads[THREAD_COUNT];
    uint64_t thread_measurements[THREAD_COUNT];
    hashmap thread_maps[THREAD_COUNT];
    process_chunk_thread_arg thread_args[THREAD_COUNT];

    struct stat st;
    fstat(fd, &st);
    off_t file_size = st.st_size;

    // + 1 just to make sure the end of the file is parsed
    off_t chunk_size = file_size / THREAD_COUNT + 1;

    // launch the threads
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        thread_args[i].fd = fd;
        thread_args[i].len = chunk_size;
        thread_args[i].offset = chunk_size * i;
        thread_args[i].map = &thread_maps[i];
        thread_args[i].measurements = &thread_measurements[i];

        hashmap_init(&thread_maps[i]);

        pthread_create(&threads[i], NULL, process_chunk_thread,
                       &thread_args[i]);
    }

    uint64_t measurements = 0;
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);

        measurements += thread_measurements[i];
        hashmap_merge(map, &thread_maps[i]);
    }

    return measurements;
}

void *process_chunk_thread(void *_arg) {
    process_chunk_thread_arg *arg = _arg;

    *arg->measurements = 0;

    char buffer[IO_BUFFER_SIZE];
    size_t left_over = 0;
    ssize_t bytes_read;
    off_t total_treated = 0;
    off_t offset = arg->offset;

    char *cur_ptr;

    if (offset != 0) {
        // the first thread should start at 0
        // but the other threads should look for the first line

        // small read just to know how much we have to skip
        // it's small so even if we will read it again later that's fine
        pread(arg->fd, buffer, MAX_LINE_LENGTH, offset);
        char *nl = memchr(buffer, '\n', MAX_LINE_LENGTH);
        if (nl == NULL) {
            // no \n found ?
            return NULL;
        }

        offset += nl - buffer + 1;
    }

    char *key_start;

    while ((bytes_read = pread(arg->fd, buffer + left_over,
                               IO_BUFFER_SIZE - left_over, offset)) > 0) {
        size_t total = left_over + bytes_read;
        char *parse_end = buffer + total;

        offset += bytes_read;

        // if not the last block, we take a margin
        if (total == IO_BUFFER_SIZE) {
            parse_end -= MAX_LINE_LENGTH;
        }

        cur_ptr = buffer;

        // parse and treat the lines
        while (cur_ptr < parse_end) {
            if (total_treated >= arg->len) {
                // thread job over
                return NULL;
            }

            key_start = cur_ptr;

            size_t hash = hash_fn(cur_ptr, &cur_ptr);

            size_t key_len = cur_ptr - key_start;

            // end the string at the separator
            cur_ptr++;
            float temperature = fast_strtof(cur_ptr, &cur_ptr);

            if (*cur_ptr != '\n') {
                // no valid float could be parsed
                printf("ok cool no \\n at %ld", *(arg->measurements));

                arg->measurements = 0;
                return NULL;
            }
            cur_ptr++;

            total_treated += cur_ptr - key_start;

            hashmap_update(arg->map, key_start, key_len, hash, temperature);

            (*(arg->measurements))++;
        }

        // copy the left over to the beginning of the buffer
        left_over = buffer + total - cur_ptr;
        memcpy(buffer, cur_ptr, left_over);
    }

    return NULL;
}

void print_city(FILE *output_stream, const city *city) {
    fprintf(output_stream,
            "{\"city\":\"%.*s\",\"min\":%.1f,\"max\":%.1f,\"mean\":%.1f,"
            "\"total\":%.1f,\"count\":%zu}",
            (int)city->name_len, city->name, city->min_temp, city->max_temp,
            city->total_temp / city->count, city->total_temp, city->count);
}

void print_cities(hashmap *map, FILE *output_stream) {
    fputs("[", output_stream);

    int first = 1; // Track if this is the first city

    for (size_t i = 0; i < map->len; i++) {
        city *city = &map->cities[i];

        if (city->name_len == 0) {
            continue;
        }

        if (!first) {
            fputs(",", output_stream);
        }
        first = 0;

        print_city(output_stream, city);
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
