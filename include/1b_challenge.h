#ifndef ONEB_CHALLENGE_H
#define ONEB_CHALLENGE_H

#include <stdio.h>
#define CITY_NAME_MAX_LEN 32

struct city {
    char name[CITY_NAME_MAX_LEN];
    float min_temp;
    float max_temp;
    float total_temp;
    float mean_temp;
    int count;
};

struct oneb_data {
    size_t nb_cities;
    struct city *cities;
};

/* process the file and store the result in oneb_data */
size_t process_stream(struct oneb_data *oneb_data, FILE *input_stream);

/* print a city in json format */
void print_city(FILE *output_stream, struct city *city);

/* write the list of cities and their data (in JSON) to output_file*/
void print_cities(struct oneb_data *oneb_data, FILE *output_stream);

/* initialize a oneb_data structure */
void oneb_challenge_init(struct oneb_data *oneb_data);

/* Add a new city to a onb_data structure.
 * Return a pointer to the newly created city
 */
struct city *oneb_challenge_add_city(struct oneb_data *oneb_data,
                                     const char *city_name, float temperature);

/* Search for a city and update its temperature statistics
 * If the city is unknown, it is created
 * This function returns the struct city that was created/updated
 */
struct city *oneb_challenge_update_city(struct oneb_data *oneb_data,
                                        const char *city_name,
                                        float temperature);

#endif /* ONEB_CHALLENGE_H */
