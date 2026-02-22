#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "solution_vector.h"
#include "vector_generic.h"

#define MAX_LINE_LENGTH 32

IMPLEMENT_VEC(city);

size_t process_stream(city_vec *vec, FILE *input_stream) {
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

        vector_update(vec, buffer, temperature);
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

void print_cities(city_vec *vec, FILE *output_stream) {
    size_t i;
    fputs("[", output_stream);
    for (i = 0; i < vec->len - 1; i++) {
        print_city(output_stream, &vec->data[i]);
        fputs(",", output_stream);
    }
    print_city(output_stream, &vec->data[i]);
    fputs("]", output_stream);
}

int vector_update(city_vec *vec, const char city_name[CITY_NAME_MAX_LEN],
                  float temperature) {
    for (size_t i = 0; i < vec->len; i++) {
        city *city = &vec->data[i];

        if (strcmp(city->name, city_name) == 0) {
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
    strcpy(city.name, city_name);
    return city_vec_push(vec, &city);
}
