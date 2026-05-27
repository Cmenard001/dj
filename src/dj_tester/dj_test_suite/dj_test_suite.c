/**
 * @file dj_test_suite.c
 * @brief A test suite groups a set of DJ pathfinding tests with a shared obstacle configuration
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see dj_test_suite.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_test_suite.h"
#include "dj_tester/dj_test/dj_test.h"
#include "dj_tester/dj_test_params/dj_test_params.h"
#include "point_generator/point_generator.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_test_suite_run(const dj_test_suite_config_t *suite, dj_test_set_t *test_set)
{
    /* Configure the point generator bounds for this suite */
    point_generator_set_bounds(suite->playground_min_x,
                               suite->playground_max_x,
                               suite->playground_min_y,
                               suite->playground_max_y);

    /* Set up the engine with this suite's obstacles */
    dj_test_params_engine_setup_for_suite(suite);

    /* Initialise the test set */
    dj_test_set_init(test_set);

    /* Generate and add tests */
    for (uint32_t i = 0; i < suite->nb_tests; i++)
    {
        point_t start_point;
        point_t destination_point;
        generate_point(&start_point);
        generate_point(&destination_point);

        dj_test_params_t params;
        dj_test_params_init(&params, start_point, destination_point);

        dj_test_t test;
        dj_test_init(&test, &params);

        if (!dj_test_set_add_test(test_set, test))
        {
            break; /* test set full */
        }
    }

    /* Run all tests */
    dj_test_set_run_tests(test_set);
}

/* ***************************************** Public callback functions definitions *************************************** */
