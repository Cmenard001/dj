/**
 * @file dependencies.h
 * @brief Useful file to centralize dependency includes and simplify the codebase
 * @author Cyprien MÃ©nard
 * @date 25/03/2025
 * @see dj_dependencies.c
 */

#ifndef __DJ_DEPENDENCIES_H__
#define __DJ_DEPENDENCIES_H__

/* ******************************************************* Includes ****************************************************** */

/**
 * @brief File dependencies
 * @note These lines must be overriden by the user
 */
#include "QS_macro.h"
#include "QS_magic_array.h"
#include "QS_maths/QS_maths.h"
#include "QS_types.h"
#include <stdio.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Print a log
 * @param ... The log to print
 */
#define dj_dep_debug_printf(...) printf(__VA_ARGS__)
#define dj_dep_error_printf(...) printf(__VA_ARGS__)
#define dj_dep_info_printf(...) printf(__VA_ARGS__)

#define DJ_DEP_HEAP_VARS_SUFFIX

/**
 * @brief No sense in the context of a dj exemple
 */
#define COLOR_Y(y) (y)

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Get the robot position
 * @param robot_position The robot position
 */
void dj_dep_get_robot_position(GEOMETRY_point_t *robot_position);

/**
 * @brief Get the robot speed
 * @param robot_speed The robot speed
 */
void dj_dep_get_robot_speed(GEOMETRY_vector_t *robot_speed);

/**
 * @brief Get the robot acceleration
 * @param robot_acceleration The robot acceleration
 */
void dj_dep_get_robot_acceleration(GEOMETRY_vector_t *robot_acceleration);

/**
 * @brief Get the time
 * @param time The time
 */
void dj_dep_get_time(time32_t *time);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
