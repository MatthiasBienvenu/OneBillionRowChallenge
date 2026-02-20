// I do not know why but without this, time.h is broken
#include <stddef.h>
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <time.h>

#include "naive_with_vector.h"

int main(int argc, char **argv) {
    FILE *input_stream;
    FILE *output_stream;

    if (argc == 1) {
        fprintf(stderr, "Warning: No input_file provided, default to stdin\n");
        input_stream = stdin;
        output_stream = stdout;
    } else if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        output_stream = stdout;
    } else if (argc == 3) {
        input_stream = fopen(argv[1], "r");
        output_stream = fopen(argv[2], "w");
    } else {
        fputs("Usage:\none_billion_row [input_file] [output_file]\n", stderr);
        return EXIT_FAILURE;
    }

    if (input_stream == NULL) {
        fputs("Could not open input_file as read", stderr);
        return EXIT_FAILURE;
    }
    if (output_stream == NULL) {
        fputs("Could not open input_file as write", stderr);
        return EXIT_FAILURE;
    }

    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    struct oneb_data oneb_data;
    oneb_challenge_init(&oneb_data);

    size_t nb_measurements = process_stream(&oneb_data, input_stream);

    print_cities(&oneb_data, output_stream);

    clock_gettime(CLOCK_MONOTONIC, &t2);
    double duration = t2.tv_sec - t1.tv_sec + (t2.tv_nsec - t1.tv_nsec) / 1e9;
    fprintf(stderr,
            "%zu measurements processed in %lf s (%lf ns per measurement)\n",
            nb_measurements, duration, duration * 1e9 / nb_measurements);
    return EXIT_SUCCESS;
}
