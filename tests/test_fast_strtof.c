#include <cmocka.h>

#include "fast_strtof.h"

#define EPS 0.01f

/* ========== POSITIVE TESTS ========== */

static void test_simple_positive_float(void **state) {
    (void)state;
    char *input = "12.5";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 12.5f, EPS);
    assert_ptr_equal(end, input + 4);
}

static void test_simple_negative_float(void **state) {
    (void)state;
    char *input = "-3.4";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, -3.4f, EPS);
    assert_ptr_equal(end, input + 4);
}

static void test_zero_values(void **state) {
    (void)state;
    char *end = NULL;

    float result = fast_strtof("0.0", &end);
    assert_float_equal(result, 0.0f, EPS);

    result = fast_strtof("-0.0", &end);
    assert_float_equal(result, -0.0f, EPS);
}

static void test_small_decimal(void **state) {
    (void)state;
    char *input = "0.586";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 0.586f, EPS);
}

static void test_large_number(void **state) {
    (void)state;
    char *input = "9999.999";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 9999.999f, EPS);
}

static void test_multiple_decimal_places(void **state) {
    (void)state;
    char *input = "123.456789";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 123.456789f, EPS);
}

static void test_end_pointer(void **state) {
    (void)state;
    char *input = "42.75abc";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 42.75f, EPS);
    assert_ptr_equal(end, input + 5);
    assert_int_equal(*end, 'a');
}

static void test_no_decimal_point(void **state) {
    (void)state;
    char *input = "42";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 42.0f, EPS);
}

static void test_decimal_only(void **state) {
    (void)state;
    char *input = ".3";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    /* Should return 0.0 since no digit before decimal */
    assert_float_equal(result, 0.3f, EPS);
}

static void test_no_fractional_part(void **state) {
    (void)state;
    char *input = "2.";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    /* Should handle trailing decimal point */
    assert_float_equal(result, 2.0f, EPS);
}

static void test_no_decimal(void **state) {
    (void)state;
    char *input = "-10";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    /* Should return 0.0 since format requires decimal point */
    assert_float_equal(result, -10.0f, EPS);
}

/* ========== NEGATIVE TESTS ========== */

static void test_empty_string(void **state) {
    (void)state;
    char *input = "";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 0.0f, EPS);
    assert_ptr_equal(end, input);
}

static void test_nan(void **state) {
    (void)state;
    char *input = "nan";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 0.0f, EPS);
    assert_ptr_equal(end, input);
}

static void test_infinity(void **state) {
    (void)state;
    char *input = "infinity";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 0.0f, EPS);
    assert_ptr_equal(end, input);
}

static void test_random_text(void **state) {
    (void)state;
    char *input = "abcdef";
    char *end = NULL;

    float result = fast_strtof(input, &end);

    assert_float_equal(result, 0.0f, EPS);
    assert_ptr_equal(end, input);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_simple_positive_float),
        cmocka_unit_test(test_simple_negative_float),
        cmocka_unit_test(test_zero_values),
        cmocka_unit_test(test_small_decimal),
        cmocka_unit_test(test_large_number),
        cmocka_unit_test(test_multiple_decimal_places),
        cmocka_unit_test(test_end_pointer),
        cmocka_unit_test(test_no_decimal_point),
        cmocka_unit_test(test_decimal_only),
        cmocka_unit_test(test_no_fractional_part),
        cmocka_unit_test(test_no_decimal),
        cmocka_unit_test(test_empty_string),
        cmocka_unit_test(test_nan),
        cmocka_unit_test(test_infinity),
        cmocka_unit_test(test_random_text),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
