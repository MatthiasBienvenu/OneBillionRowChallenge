#include <cmocka.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "vector_generic.h"

DECLARE_VEC(int);
IMPLEMENT_VEC(int);

DECLARE_VEC(char);
IMPLEMENT_VEC(char);

DECLARE_VEC(double);
IMPLEMENT_VEC(double);

DECLARE_VEC(long);
IMPLEMENT_VEC(long);

typedef struct {
    int x;
    int y;
} Point;

DECLARE_VEC(Point)
IMPLEMENT_VEC(Point)

/* ========== POSITIVE TESTS ========== */

static void test_vec_init(void **state) {
    (void)state;
    int_vec v;
    int ret = int_vec_init(&v);

    assert_int_equal(ret, 0);
    assert_int_equal(v.capacity, 1);
    assert_int_equal(v.len, 0);
    assert_non_null(v.data);

    free(v.data);
}

static void test_vec_init_different_sizes(void **state) {
    (void)state;
    int ret;

    char_vec cv;
    ret = char_vec_init(&cv);
    assert_int_equal(ret, 0);
    free(cv.data);

    double_vec dv;
    ret = double_vec_init(&dv);
    assert_int_equal(ret, 0);
    free(dv.data);

    long_vec llv;
    ret = long_vec_init(&llv);
    assert_int_equal(ret, 0);
    free(llv.data);
}

static void test_vec_from_array_ints(void **state) {
    (void)state;
    int_vec v;
    int values[] = {10, 20, 30, 40, 50};
    int count = 5;

    int ret = int_vec_from_array(&v, values, count);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, count);
    assert_int_equal(v.capacity, count);

    for (int i = 0; i < count; i++) {
        assert_int_equal(v.data[i], values[i]);
    }

    free(v.data);
}

static void test_vec_from_array_doubles(void **state) {
    (void)state;
    double_vec v;
    double values[] = {1.5, 2.7, 3.14159, 4.0};
    int count = 4;

    int ret = double_vec_from_array(&v, values, count);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, count);
    assert_int_equal(v.capacity, count);

    for (int i = 0; i < count; i++) {
        assert_double_equal(v.data[i], values[i], 0.000001);
    }

    free(v.data);
}

static void test_vec_from_array_struct(void **state) {
    (void)state;
    Point_vec v;
    Point points[] = {{1, 2}, {3, 4}, {5, 6}};
    int count = 3;

    int ret = Point_vec_from_array(&v, points, count);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, count);

    for (int i = 0; i < count; i++) {
        assert_int_equal(v.data[i].x, points[i].x);
        assert_int_equal(v.data[i].y, points[i].y);
    }

    free(v.data);
}

static void test_vec_push_single(void **state) {
    (void)state;
    int_vec v;
    int_vec_init(&v);
    int value = 42;

    int ret = int_vec_push(&v, &value);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, 1);
    assert_int_equal(v.data[0], 42);

    free(v.data);
}

static void test_vec_push_growth(void **state) {
    (void)state;
    int_vec v;
    int_vec_init(&v);
    int value = 7;

    assert_int_equal(v.capacity, 1);

    assert_int_equal(int_vec_push(&v, &value), 0);
    assert_int_equal(v.len, 1);
    assert_int_equal(v.capacity, 1);

    assert_int_equal(int_vec_push(&v, &value), 0);
    assert_int_equal(v.len, 2);
    assert_int_equal(v.capacity, 2);

    assert_int_equal(int_vec_push(&v, &value), 0);
    assert_int_equal(v.len, 3);
    assert_int_equal(v.capacity, 4);

    assert_int_equal(int_vec_push(&v, &value), 0);
    assert_int_equal(int_vec_push(&v, &value), 0);
    assert_int_equal(v.len, 5);
    assert_int_equal(v.capacity, 8);

    free(v.data);
}

static void test_vec_pop_from_array(void **state) {
    (void)state;
    int_vec v;
    int values[] = {10, 20, 30, 40, 50};
    int_vec_from_array(&v, values, 5);

    int result = 0;
    int ret = int_vec_pop(&v, &result);

    assert_int_equal(ret, 0);
    assert_int_equal(result, 50);
    assert_int_equal(v.len, 4);

    ret = int_vec_pop(&v, &result);
    assert_int_equal(ret, 0);
    assert_int_equal(result, 40);
    assert_int_equal(v.len, 3);

    free(v.data);
}

static void test_vec_pop_lifo(void **state) {
    (void)state;
    int_vec v;
    int values[] = {10, 20, 30, 40, 50};
    int_vec_from_array(&v, values, 5);

    int result = 0;
    for (int i = 4; i >= 0; i--) {
        int ret = int_vec_pop(&v, &result);
        assert_int_equal(ret, 0);
        assert_int_equal(result, values[i]);
    }
    assert_int_equal(v.len, 0);

    free(v.data);
}

static void test_vec_pop_no_realloc(void **state) {
    (void)state;
    int_vec v;
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int_vec_from_array(&v, values, 8);

    size_t original_capacity = v.capacity;
    int result = 0;

    for (int i = 0; i < 5; i++) {
        assert_int_equal(int_vec_pop(&v, &result), 0);
    }

    assert_int_equal(v.capacity, original_capacity);
    assert_int_equal(v.len, 3);

    free(v.data);
}

static void test_vec_push_after_pop(void **state) {
    (void)state;
    int_vec v;
    int values[] = {10, 20, 30};
    int_vec_from_array(&v, values, 3);

    int result = 0;
    assert_int_equal(int_vec_pop(&v, &result), 0);
    assert_int_equal(v.len, 2);

    int new_val = 100;
    assert_int_equal(int_vec_push(&v, &new_val), 0);
    assert_int_equal(v.len, 3);
    assert_int_equal(v.data[2], 100);

    free(v.data);
}

static void test_vec_from_array_large(void **state) {
    (void)state;
    int_vec v;
    int values[1000];
    for (int i = 0; i < 1000; i++) {
        values[i] = i;
    }

    int ret = int_vec_from_array(&v, values, 1000);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, 1000);
    assert_int_equal(v.capacity, 1000);

    for (int i = 0; i < 1000; i++) {
        assert_int_equal(v.data[i], i);
    }

    free(v.data);
}

/* ========== NEGATIVE TESTS ========== */

static void test_vec_pop_empty_vector(void **state) {
    (void)state;
    int_vec v;
    int_vec_init(&v);

    int result = 0;
    int ret = int_vec_pop(&v, &result);

    /* Should return error since len is 0 */
    assert_int_equal(ret, 1);

    free(v.data);
}

static void test_vec_pop_underflow(void **state) {
    (void)state;
    int_vec v;
    int values[] = {10, 20};
    int_vec_from_array(&v, values, 2);

    int result = 0;

    /* Pop first element - should succeed */
    assert_int_equal(int_vec_pop(&v, &result), 0);
    assert_int_equal(result, 20);

    /* Pop second element - should succeed */
    assert_int_equal(int_vec_pop(&v, &result), 0);
    assert_int_equal(result, 10);

    /* Try to pop from empty vector - should fail */
    assert_int_equal(int_vec_pop(&v, &result), 1);
    assert_int_equal(v.len, 0);

    free(v.data);
}

static void test_vec_from_array_empty(void **state) {
    (void)state;
    int_vec v;
    int values[] = {};

    int ret = int_vec_from_array(&v, values, 0);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, 0);
    assert_int_equal(v.capacity, 0);

    free(v.data);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vec_init),
        cmocka_unit_test(test_vec_init_different_sizes),
        cmocka_unit_test(test_vec_from_array_ints),
        cmocka_unit_test(test_vec_from_array_doubles),
        cmocka_unit_test(test_vec_from_array_struct),
        cmocka_unit_test(test_vec_push_single),
        cmocka_unit_test(test_vec_push_growth),
        cmocka_unit_test(test_vec_pop_from_array),
        cmocka_unit_test(test_vec_pop_lifo),
        cmocka_unit_test(test_vec_pop_no_realloc),
        cmocka_unit_test(test_vec_push_after_pop),
        cmocka_unit_test(test_vec_from_array_large),
        cmocka_unit_test(test_vec_pop_empty_vector),
        cmocka_unit_test(test_vec_pop_underflow),
        cmocka_unit_test(test_vec_from_array_empty),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
