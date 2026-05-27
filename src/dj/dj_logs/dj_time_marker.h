/**
 * @file dj_time_marker.h
 * @brief Markers for dj can be used to measure the time taken by different parts of the code
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_time_marker.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

#define DJ_MARKERS                                                                                 \
    GENERATE_PATH, PREBUILT_GRAPH_REBUILD, ALL, FORCE_LINK_START_END, FORCE_LINK_NODE,             \
        BUILD_GRAPH_NODES, BUILD_GRAPH_LINKS, FIRST_REBUILD, COPY_PREBUILT_GRAPH, SOLVE, REBUILD,  \
        REBUILD_ENABLE_ALL, OBSTACLE_MANAGER_GET_ALL_OBSTACLES, DYNAMIC_OBSTACLE_COMPUTE,          \
        REBUILD_ADD_VIEWER_AS_NODE, REBUILD_DISABLE_LINKS, BUILD_WITH_OBSTACLE, PATH_DURATION,     \
        SOLVE_EXPLORE_LINKS, SOLVE_FIND_NEXT

/* ************************************************** Public types definition ******************************************** */

typedef enum
{
    DJ_MARKERS,
    DJ_TIME_MARKER_COUNT
} dj_time_marker_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Reset a single time marker, clearing all its statistics
 * @param type The marker to reset
 */
void dj_mark_reset(dj_time_marker_t type);

/**
 * @brief Reset all time markers, clearing all their statistics
 */
void dj_mark_reset_all(void);

/**
 * @brief Mark all markers as unused for the current session, without clearing accumulated statistics
 * @note Call this at the start of a timed operation to track which markers were used
 */
void dj_mark_session_reset(void);

/**
 * @brief Mark the start of a timed period
 * @param type The marker to start
 * @note Also marks the marker as used in the current session
 */
void dj_mark_start_time(dj_time_marker_t type);

/**
 * @brief Mark the end of a timed period and accumulate statistics
 * @param type The marker to stop
 */
void dj_mark_end_time(dj_time_marker_t type);

/**
 * @brief Print the statistics of a single time marker
 * @param type The marker to print
 */
void dj_print_duration(dj_time_marker_t type);

/**
 * @brief Print the statistics of all time markers
 */
void dj_print_all_durations(void);

/**
 * @brief Print the statistics of all markers that were used in the current session
 * @param operation_name Name of the operation to display in the header
 */
void dj_print_session_durations(const char *operation_name);

/* ******************************************* Public callback functions declarations ************************************ */
