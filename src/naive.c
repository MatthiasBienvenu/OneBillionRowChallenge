#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "naive.h"

#define MAX_LINE_LENGTH 32

size_t process_stream(struct oneb_data *oneb_data, FILE *input_stream) {
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

        oneb_challenge_update_city(oneb_data, buffer, temperature);
    }

    return measurements;
}

void print_city(FILE *output_stream, struct city *city) {
    fprintf(output_stream,
            "{\"city\":\"%s\",\"min\":%.1f,\"max\":%.1f,\"mean\":%.1f,"
            "\"total\":%.1f,\"count\":%d}",
            city->name, city->min_temp, city->max_temp, city->mean_temp,
            city->total_temp, city->count);
}

void print_cities(struct oneb_data *oneb_data, FILE *output_stream) {
    size_t i;
    fputs("[", output_stream);
    for (i = 0; i < oneb_data->nb_cities - 1; i++) {
        print_city(output_stream, &oneb_data->cities[i]);
        fputs(",", output_stream);
    }
    print_city(output_stream, &oneb_data->cities[i]);
    fputs("]", output_stream);
}

void oneb_challenge_init(struct oneb_data *oneb_data) {
    oneb_data->nb_cities = 0;
    oneb_data->cities = NULL;
}

struct city *oneb_challenge_add_city(struct oneb_data *oneb_data,
                                     const char *city_name, float temperature) {
    int index = oneb_data->nb_cities++;
    oneb_data->cities =
        realloc(oneb_data->cities, oneb_data->nb_cities * sizeof(struct city));

    struct city *city = &oneb_data->cities[index];
    strcpy(city->name, city_name);
    city->count = 1;
    city->min_temp = city->max_temp = city->total_temp = city->mean_temp =
        temperature;

    return &oneb_data->cities[index];
}

struct city *oneb_challenge_update_city(struct oneb_data *oneb_data,
                                        const char *city_name,
                                        float temperature) {
    size_t city_index = SIZE_MAX;
    for (size_t i = 0; i < oneb_data->nb_cities; i++) {
        if (strcmp(oneb_data->cities[i].name, city_name) == 0) {
            city_index = i;
        }
    }

    if (city_index == SIZE_MAX) {
        oneb_challenge_add_city(oneb_data, city_name, temperature);
    } else {
        struct city *city = &oneb_data->cities[city_index];
        city->min_temp = fminf(city->min_temp, temperature);
        city->max_temp = fmaxf(city->max_temp, temperature);
        city->total_temp += temperature;
        city->count++;
        city->mean_temp = city->total_temp / city->count;
    }

    return NULL;
}
