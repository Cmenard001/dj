/**
 * @file dj_test_report.c
 * @brief Report of a DJ test
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test_report.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_test_report.h"

#include <stdio.h>
#include <string.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_test_report_init(dj_test_report_t *report)
{
    // Initialize the report string to an empty string
    report->m_report[0] = '\0';
}

bool dj_test_report_add_message(dj_test_report_t *report, const char *message)
{
    // Use a temporary buffer to avoid aliasing issues
    char temp[DJ_TEST_REPORT_MAX_LENGTH];
    int len = snprintf(temp, DJ_TEST_REPORT_MAX_LENGTH, "%s%s", report->m_report, message);
    if (len < DJ_TEST_REPORT_MAX_LENGTH)
    {
        strcpy(report->m_report, temp);
        return true; // Message added successfully
    }
    return false; // Message would exceed maximum length
}

const char *dj_test_report_get_report(const dj_test_report_t *report)
{
    // Return the current report string
    return report->m_report;
}

uint16_t dj_test_report_get_length(const dj_test_report_t *report)
{
    // Return the length of the current report string
    return strlen(report->m_report);
}

void dj_test_report_print(const dj_test_report_t *report)
{
    // Print the current report string
    printf("%s\n", report->m_report);
}

void dj_test_report_print_to_file(const dj_test_report_t *report, const char *filename)
{
    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file)
    {
        // Write the report to the file
        fprintf(file, "%s\n", report->m_report);
        // Close the file
        fclose(file);
    }
}

/* ***************************************** Public callback functions definitions *************************************** */