/**
 * @file dj_time_marker.h
 * @brief Markers for dj can be used to measure the time taken by different parts of the code
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_time_marker.c
 */

#ifndef __DJ_TIME_MARKER_H__
#define __DJ_TIME_MARKER_H__

/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

#define DJ_MARKERS                                                                                                          \
    DJ_MARK_ALL, DJ_MARK_FORCE_LINK_START_END, DJ_MARK_BUILD_GRAPH_NODES, DJ_MARK_BUILD_GRAPH_LINKS, DJ_MARK_FIRST_REBUILD, \
        DJ_MARK_SOLVE, DJ_MARK_REBUILD, DJ_MARK_REBUILD_ENABLE_ALL, DJ_MARK_OBSTACLE_MANAGER_GET_ALL_OBSTACLES,             \
        DJ_MARK_REBUILD_ADD_OBSTACLE_AS_NODE, DJ_MARK_REBUILD_ADD_VIEWER_AS_NODE, DJ_MARK_REBUILD_DISABLE_LINKS,            \
        DJ_MARK_PATH_DURATION

/* ************************************************** Public types definition ******************************************** */

typedef enum
{
    DJ_MARKERS,
    DJ_TIME_MARKER_TYPE_COUNT
} dj_time_marker_e;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to reset a time marker
 *
 * @param type
 */
void dj_mark_reset(dj_time_marker_e type);

/**
 * @brief Function to reset all time markers
 */
void dj_mark_reset_all();

/**
 * @brief Function to mark the start of a period of time
 *
 * @param type
 */
void dj_mark_start_time(dj_time_marker_e type);

/**
 * @brief Function to mark the end of a period of time
 *
 * @param type
 */
void dj_mark_end_time(dj_time_marker_e type);

/**
 * @brief Function to print the duration of a period of time
 *
 * @param type
 */
void dj_print_duration(dj_time_marker_e type);

/**
 * @brief Function to print the duration of all periods of time
 */
void dj_print_all_durations();

/* ******************************************* Public callback functions declarations ************************************ */

#endif
