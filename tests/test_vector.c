#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

/* ========== POSITIVE TESTS ========== */

static void test_vec_init(void **state) {
    (void)state;
    struct vec v;
    int ret = vec_init(&v, sizeof(int));

    assert_int_equal(ret, 0);
    assert_int_equal(v.element_size, sizeof(int));
    assert_int_equal(v.capacity, 1);
    assert_int_equal(v.len, 0);
    assert_non_null(v.data);

    free(v.data);
}

static void test_vec_init_different_sizes(void **state) {
    (void)state;
    struct vec v;
    int ret;

    ret = vec_init(&v, sizeof(char));
    assert_int_equal(ret, 0);
    assert_int_equal(v.element_size, sizeof(char));
    free(v.data);

    ret = vec_init(&v, sizeof(double));
    assert_int_equal(ret, 0);
    assert_int_equal(v.element_size, sizeof(double));
    free(v.data);

    ret = vec_init(&v, sizeof(long long));
    assert_int_equal(ret, 0);
    assert_int_equal(v.element_size, sizeof(long long));
    free(v.data);
}

static void test_vec_from_array_ints(void **state) {
    (void)state;
    struct vec v;
    int values[] = {10, 20, 30, 40, 50};
    int count = 5;

    int ret = vec_from_array(&v, values, sizeof(int), count);

    assert_int_equal(ret, 0);
    assert_int_equal(v.element_size, sizeof(int));
    assert_int_equal(v.len, count);
    assert_int_equal(v.capacity, count);

    for (int i = 0; i < count; i++) {
        int actual = *(int *)(v.data + sizeof(int) * i);
        assert_int_equal(actual, values[i]);
    }

    free(v.data);
}

static void test_vec_from_array_doubles(void **state) {
    (void)state;
    struct vec v;
    double values[] = {1.5, 2.7, 3.14159, 4.0};
    int count = 4;

    int ret = vec_from_array(&v, values, sizeof(double), count);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, count);
    assert_int_equal(v.capacity, count);

    for (int i = 0; i < count; i++) {
        double actual = *(double *)(v.data + sizeof(double) * i);
        double expected = values[i];
        assert_double_equal(actual, expected, 0.000001);
    }

    free(v.data);
}

static void test_vec_from_array_struct(void **state) {
    (void)state;
    typedef struct {
        int x;
        int y;
    } Point;

    struct vec v;
    Point points[] = {{1, 2}, {3, 4}, {5, 6}};
    int count = 3;

    int ret = vec_from_array(&v, points, sizeof(Point), count);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, count);

    for (int i = 0; i < count; i++) {
        Point *p = (Point *)(v.data + sizeof(Point) * i);
        assert_int_equal(p->x, points[i].x);
        assert_int_equal(p->y, points[i].y);
    }

    free(v.data);
}

static void test_vec_push_single(void **state) {
    (void)state;
    struct vec v;
    vec_init(&v, sizeof(int));
    int value = 42;

    int ret = vec_push(&v, &value);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, 1);

    int actual = *(int *)v.data;
    assert_int_equal(actual, 42);

    free(v.data);
}

static void test_vec_push_growth(void **state) {
    (void)state;
    struct vec v;
    vec_init(&v, sizeof(int));
    int value = 7;

    assert_int_equal(v.capacity, 1);

    assert_int_equal(vec_push(&v, &value), 0);
    assert_int_equal(v.len, 1);
    assert_int_equal(v.capacity, 1);

    assert_int_equal(vec_push(&v, &value), 0);
    assert_int_equal(v.len, 2);
    assert_int_equal(v.capacity, 2);

    assert_int_equal(vec_push(&v, &value), 0);
    assert_int_equal(v.len, 3);
    assert_int_equal(v.capacity, 4);

    assert_int_equal(vec_push(&v, &value), 0);
    assert_int_equal(vec_push(&v, &value), 0);
    assert_int_equal(v.len, 5);
    assert_int_equal(v.capacity, 8);

    free(v.data);
}

static void test_vec_pop_from_array(void **state) {
    (void)state;
    struct vec v;
    int values[] = {10, 20, 30, 40, 50};
    vec_from_array(&v, values, sizeof(int), 5);

    int result = 0;
    int ret = vec_pop(&v, &result);

    assert_int_equal(ret, 0);
    assert_int_equal(result, 50);
    assert_int_equal(v.len, 4);

    ret = vec_pop(&v, &result);
    assert_int_equal(ret, 0);
    assert_int_equal(result, 40);
    assert_int_equal(v.len, 3);

    free(v.data);
}

static void test_vec_pop_lifo(void **state) {
    (void)state;
    struct vec v;
    int values[] = {10, 20, 30, 40, 50};
    vec_from_array(&v, values, sizeof(int), 5);

    int result = 0;
    for (int i = 4; i >= 0; i--) {
        int ret = vec_pop(&v, &result);
        assert_int_equal(ret, 0);
        assert_int_equal(result, values[i]);
    }
    assert_int_equal(v.len, 0);

    free(v.data);
}

static void test_vec_pop_no_realloc(void **state) {
    (void)state;
    struct vec v;
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8};
    vec_from_array(&v, values, sizeof(int), 8);

    size_t original_capacity = v.capacity;
    int result = 0;

    for (int i = 0; i < 5; i++) {
        assert_int_equal(vec_pop(&v, &result), 0);
    }

    assert_int_equal(v.capacity, original_capacity);
    assert_int_equal(v.len, 3);

    free(v.data);
}

static void test_vec_push_after_pop(void **state) {
    (void)state;
    struct vec v;
    int values[] = {10, 20, 30};
    vec_from_array(&v, values, sizeof(int), 3);

    int result = 0;
    assert_int_equal(vec_pop(&v, &result), 0);
    assert_int_equal(v.len, 2);

    int new_val = 100;
    assert_int_equal(vec_push(&v, &new_val), 0);
    assert_int_equal(v.len, 3);

    int actual = *(int *)(v.data + sizeof(int) * 2);
    assert_int_equal(actual, 100);

    free(v.data);
}

static void test_vec_from_array_large(void **state) {
    (void)state;
    struct vec v;
    int values[1000];
    for (int i = 0; i < 1000; i++) {
        values[i] = i;
    }

    int ret = vec_from_array(&v, values, sizeof(int), 1000);

    assert_int_equal(ret, 0);
    assert_int_equal(v.len, 1000);
    assert_int_equal(v.capacity, 1000);

    for (int i = 0; i < 1000; i++) {
        int actual = *(int *)(v.data + sizeof(int) * i);
        assert_int_equal(actual, i);
    }

    free(v.data);
}

/* ========== NEGATIVE TESTS ========== */

static void test_vec_pop_empty_vector(void **state) {
    (void)state;
    struct vec v;
    vec_init(&v, sizeof(int));

    int result = 0;
    int ret = vec_pop(&v, &result);

    /* Should return error since len is 0 */
    assert_int_equal(ret, 1);

    free(v.data);
}

static void test_vec_pop_underflow(void **state) {
    (void)state;
    struct vec v;
    int values[] = {10, 20};
    vec_from_array(&v, values, sizeof(int), 2);

    int result = 0;

    /* Pop first element - should succeed */
    assert_int_equal(vec_pop(&v, &result), 0);
    assert_int_equal(result, 20);

    /* Pop second element - should succeed */
    assert_int_equal(vec_pop(&v, &result), 0);
    assert_int_equal(result, 10);

    /* Try to pop from empty vector - should fail */
    assert_int_equal(vec_pop(&v, &result), 1);
    assert_int_equal(v.len, 0);

    free(v.data);
}

static void test_vec_from_array_empty(void **state) {
    (void)state;
    struct vec v;
    int values[] = {};

    int ret = vec_from_array(&v, values, sizeof(int), 0);

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
