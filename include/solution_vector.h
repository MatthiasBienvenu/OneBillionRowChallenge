#ifndef ONEB_CHALLENGE_H
#define ONEB_CHALLENGE_H

#include "vector_generic.h"
#include <stdio.h>

#define CITY_NAME_MAX_LEN 32

typedef struct {
    char name[CITY_NAME_MAX_LEN];
    float min_temp;
    float max_temp;
    float total_temp;
    float mean_temp;
    int count;
} city;

DECLARE_VEC(city);

/* process the file and store the result in vec */
size_t process_stream(city_vec *vec, FILE *input_stream);

/* print a city in json format */
void print_city(FILE *output_stream, city *city);

/* write the list of cities and their data (in JSON) to output_file*/
void print_cities(city_vec *vec, FILE *output_stream);

/* Search for a city and update its temperature statistics
 * If the city is unknown, it is created
 * This function returns the city that was created/updated
 */
int oneb_challenge_update_city(city_vec *vec, const char *city_name,
                               float temperature);

#endif /* ONEB_CHALLENGE_H */
