/**
 * @file dj_tester.c
 * @brief Tester for the DJ module
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_tester.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_tester.h"
#include "point_generator/point_generator.h"

/* **************************************************** Private macros *************************************************** */

#define DJ_REPORT_FILE_NAME "dj_test_report.txt"

/**
 * @brief Number of tests to be performed by the DJ tester.
 */
#define DJ_TESTER_NB_TESTS (10)

#if DJ_TESTER_NB_TESTS > DJ_TEST_SET_MAX_TESTS
#error "DJ_TESTER_NB_TESTS exceeds the maximum number of tests allowed in the test set."
#endif

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_tester_launch_tests()
{
    // Initialize the test set
    dj_test_set_t test_set;
    dj_test_set_init(&test_set);

    // Add tests to the test set
    for (uint8_t i = 0; i < DJ_TESTER_NB_TESTS; i++)
    {
        // Create a test for the DJ module
        GEOMETRY_point_t start_point;
        GEOMETRY_point_t destination_point;
        generate_point(&start_point);
        generate_point(&destination_point);
        dj_test_params_t params;
        dj_test_params_init(&params, start_point, destination_point);
        dj_test_t test;
        dj_test_init(&test, &params);
        // Add the test to the test set
        if (!dj_test_set_add_test(&test_set, test))
        {
            // If the test set is full, break the loop
            printf("Test set is full, cannot add more tests.\n");
            break;
        }
    }

    // Run all tests defined in the DJ module
    dj_test_set_run_tests(&test_set);

    // Generate a report of the test results
    dj_test_report_t l_report;
    dj_test_get_report(&test_set, &l_report);
    // Print the report
    dj_test_report_print(&l_report);
    // Save the report to a file
    dj_test_report_print_to_file(&l_report, DJ_REPORT_FILE_NAME);
}

/* ***************************************** Public callback functions definitions *************************************** */