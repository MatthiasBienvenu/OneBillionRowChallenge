// I do not know why but without this, time.h is broken
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <time.h>

#include "1b_challenge.h"

int main(int argc, char **argv) {
    char *input_file = "measurements.csv";
    char *output_file = "output_file.json";
    if (argc > 1) {
        input_file = argv[1];
    }

    int nb_measurements = 0;
    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    struct oneb_data oneb_data;
    oneb_challenge_init(&oneb_data);
    /* TODO: process input_file */

    print_cities(&oneb_data, output_file);

    clock_gettime(CLOCK_MONOTONIC, &t2);
    double duration = t2.tv_sec - t1.tv_sec + (t2.tv_nsec - t1.tv_nsec) / 1e9;
    printf("%d measurements processed in %lf s (%lf ns per measurement)\n",
           nb_measurements, duration, duration * 1e9 / nb_measurements);
    return EXIT_SUCCESS;
}
