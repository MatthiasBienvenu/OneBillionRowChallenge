#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "1b_challenge.h"

/* print a city in json format */
void print_city(struct oneb_data *oneb_data, FILE *output_stream,
                struct city *city) {
    fprintf(output_stream,
            "{\"city\": \"%s\", \"min\": %.1f, \"max\": %.1f, \"mean\": %.1f}",
            city->name, city->min_temp, city->max_temp, city->mean_temp);
}

/* Write the list of cities and their data (in JSON) to output_file*/
void print_cities(struct oneb_data *oneb_data, const char *output_file) {
    /* TODO */
}

/* initialize a oneb_data structure */
void oneb_challenge_init(struct oneb_data *oneb_data) {
    oneb_data->nb_cities = 0;
    oneb_data->cities = NULL;
}

/* Add a new city to a onb_data structure.
 * Return a pointer to the newly created city
 */
struct city *oneb_challenge_add_city(struct oneb_data *oneb_data,
                                     const char *city_name, float temperature) {
    int index = oneb_data->nb_cities++;
    oneb_data->cities =
        realloc(oneb_data->cities, oneb_data->nb_cities * sizeof(struct city));

    /* TODO */

    return &oneb_data->cities[index];
}

/* Search for a city and update its temperature statistics
 * If the city is unknown, it is created
 * This function returns the struct city that was created/updated
 */
struct city *oneb_challenge_update_city(struct oneb_data *oneb_data,
                                        const char *city_name,
                                        float temperature) {
    /* TODO */

    return NULL;
}
