/**
 * @file dj_test_report.h
 * @brief Report of a DJ test
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test_report.c
 */

#ifndef __DJ_TEST_REPORT_H__
#define __DJ_TEST_REPORT_H__

/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>
#include <stdbool.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum length of a test report
 *
 * This macro defines the maximum length of a test report string.
 * It is used to allocate memory for the report and ensure it fits within the buffer.
 */
#define DJ_TEST_REPORT_MAX_LENGTH (1024)

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a report of a DJ test
 *
 * This structure holds the report string and its length.
 */
typedef struct
{
    char m_report[DJ_TEST_REPORT_MAX_LENGTH]; /**< Report string. This string should be null-terminated */
} dj_test_report_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize a DJ test report
 *
 * This function initializes a DJ test report by setting the report string to an empty string.
 *
 * @param report Pointer to the test report to initialize
 */
void dj_test_report_init(dj_test_report_t *report);

/**
 * @brief Add a message to the DJ test report
 *
 * This function appends a message to the test report. It ensures that the report does not exceed its maximum length.
 *
 * @param report Pointer to the test report
 * @param message The message to add to the report
 * @return true if the message was added successfully, false if it would exceed the maximum length
 */
bool dj_test_report_add_message(dj_test_report_t *report, const char *message);

/**
 * @brief Get the current report string
 *
 * This function retrieves the current report string from the test report.
 *
 * @param report Pointer to the test report
 * @return Pointer to the report string
 */
const char *dj_test_report_get_string(const dj_test_report_t *report);

/**
 * @brief Get the length of the current report string
 *
 * This function retrieves the length of the current report string in the test report.
 *
 * @param report Pointer to the test report
 * @return Length of the report string
 */
uint16_t dj_test_report_get_length(const dj_test_report_t *report);

/**
 * @brief Print the DJ test report to the console
 *
 * This function prints the contents of the test report to the console.
 *
 * @param report Pointer to the test report to print
 */
void dj_test_report_print(const dj_test_report_t *report);

/**
 * @brief Print the DJ test report to a file
 *
 * This function writes the contents of the test report to a specified file.
 *
 * @param report Pointer to the test report to print
 * @param filename Name of the file where the report will be written
 */
void dj_test_report_print_to_file(const dj_test_report_t *report, const char *filename);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TEST_REPORT_H__ */