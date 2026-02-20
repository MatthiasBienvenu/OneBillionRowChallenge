#include <assert.h>
#include <cmocka.h>
#include <stddef.h>
#include <stdint.h>

#include "naive.h"

void test_oneb_init(void **state) {
    /* Test whether oneb_challenge_init works as intended  */
    struct oneb_data oneb_data;
    oneb_challenge_init(&oneb_data);

    /* We may have to change these assertions if we decide to use another data
     * structure */
    assert_int_equal(oneb_data.nb_cities, 0);
    assert_null(oneb_data.cities);
}

void test_add_city(void **state) {

    struct oneb_data oneb_data;
    oneb_challenge_init(&oneb_data);

    char *city_name = "Evry-Courcouronnes";
    float temperature = 17.3;

    struct city *city =
        oneb_challenge_add_city(&oneb_data, city_name, temperature);
    assert_int_equal(oneb_data.nb_cities, 1);
    assert_non_null(city);
    assert_string_equal(city->name, city_name);
    assert_float_equal(city->min_temp, temperature, 0.01);
    assert_float_equal(city->max_temp, temperature, 0.01);
    assert_float_equal(city->total_temp, temperature, 0.01);
    assert_int_equal(city->count, 1);
}

int main(int argc, char **argv) {
    const struct CMUnitTest tests[] = {
        /* a few tests for goblin army.
         *
         * If you implemented correctly the functions, all these tests should be
         * successfull Of course this test suite may not cover all the tricky
         * cases, and you should add your own tests.
         */
        cmocka_unit_test(test_oneb_init), cmocka_unit_test(test_add_city)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
