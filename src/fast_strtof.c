#include "fast_strtof.h"
#include <stdio.h>

float fast_strtof(const char *string, const char **end) {
    float sign = 1.0f;
    float result = 0.0f;
    float base = 1.0f;

    // sign
    if (*string == '-') {
        sign = -1.0f;
        string++;
    }

    // integer part
    for (; '0' <= *string && *string <= '9'; string++) {
        result *= 10;
        result += *string - '0';
    }

    // dot
    if (*string != '.') {
        *end = string;
        return sign * result;
    }
    string++;

    // decimal part
    for (; '0' <= *string && *string <= '9'; string++) {
        base *= 0.1f;
        result += (*string - '0') * base;
    }

    *end = string;
    return sign * result;
}
