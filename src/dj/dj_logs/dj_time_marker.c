/**
 * @file dj_time_marker.c
 * @brief Markers for dj can be used to measure the time taken by different parts of the code
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_time_marker.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_time_marker.h"

#include "../dj_dependencies/dj_dependencies.h"
#include "dj_logs.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

typedef struct
{
    time32_t start_time;
    time32_t duration;
} dj_time_marker_t;

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/**
 * @brief Names of the time markers
 */
const char *const dj_time_marker_names[] = {FOREACH(WORD_CONVERT_TO_STRING, DJ_MARKERS)};

/**
 * @brief Array of time markers
 */
static dj_time_marker_t dj_time_markers[DJ_TIME_MARKER_TYPE_COUNT];

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_mark_reset(dj_time_marker_e type)
{
    dj_time_markers[type].start_time = 0;
    dj_time_markers[type].duration = 0;
}

void dj_mark_reset_all()
{
    for (dj_time_marker_e i = 0; i < DJ_TIME_MARKER_TYPE_COUNT; i++)
    {
        dj_mark_reset(i);
    }
}

void dj_mark_start_time(dj_time_marker_e type)
{
    dj_dep_get_time(&dj_time_markers[type].start_time);
}

void dj_mark_end_time(dj_time_marker_e type)
{
    time32_t end_time;
    dj_dep_get_time(&end_time);
    dj_time_markers[type].duration += end_time - dj_time_markers[type].start_time;
}

void dj_print_duration(dj_time_marker_e type)
{
#if DJ_ENABLE_DEBUG_LOGS
    int duration = dj_time_markers[type].duration;

    dj_debug_printf("Duration of %s: %d ms\n", dj_time_marker_names[type], duration);
#endif
}

void dj_print_all_durations()
{
    for (dj_time_marker_e i = 0; i < DJ_TIME_MARKER_TYPE_COUNT; i++)
    {
        dj_print_duration(i);
    }
}

/* ******************************************* Public callback functions declarations ************************************ */
