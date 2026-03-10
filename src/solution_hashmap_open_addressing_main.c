// I do not know why but without this, time.h is broken
#include <stddef.h>
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <fcntl.h>
#include <time.h>

#include "solution_hashmap_open_addressing.h"

int main(int argc, char **argv) {
    int input_fd;
    FILE *output_stream;

    if (argc == 2) {
        input_fd = open(argv[1], O_RDONLY);
        output_stream = stdout;
    } else if (argc == 3) {
        input_fd = open(argv[1], O_RDONLY);
        output_stream = fopen(argv[2], "w");
    } else {
        fputs("Usage:\none_billion_row [input_file] [output_file]\n", stderr);
        return EXIT_FAILURE;
    }

    if (input_fd == -1) {
        fputs("Could not open input_file as read", stderr);
        return EXIT_FAILURE;
    }
    if (output_stream == NULL) {
        fputs("Could not open input_file as write", stderr);
        return EXIT_FAILURE;
    }

    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    hashmap oneb_data;
    hashmap_init(&oneb_data);

    size_t nb_measurements = process_file(&oneb_data, input_fd);

    print_cities(&oneb_data, output_stream);

    clock_gettime(CLOCK_MONOTONIC, &t2);
    double duration = t2.tv_sec - t1.tv_sec + (t2.tv_nsec - t1.tv_nsec) / 1e9;
    fprintf(stderr,
            "%zu measurements processed in %lf s (%lf ns per measurement)\n",
            nb_measurements, duration, duration * 1e9 / nb_measurements);
    return EXIT_SUCCESS;
}
