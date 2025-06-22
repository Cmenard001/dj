/**
 * @file dj_launcher.h
 * @brief Launcher for the dj path generation
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_launcher.c
 */

#ifndef __DJ_LAUNCHER_H__
#define __DJ_LAUNCHER_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_graph/dj_graph_path.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to generate a path from the current position to the target point
 * @note If any path is possible to generate, this function will not disable the dynamic obstacles
 * @see dj_genarate_path
 *
 * @param [out] path The path to generate
 * @param [in] target_point The target point
 * @param [in] enable_dynamic_obstacles Enable the dynamic obstacles
 *
 * @return true if on success, false otherwise
 */
bool dj_genarate_path_with_param(dj_graph_path_t *path, GEOMETRY_point_t target_point, bool enable_dynamic_obstacles);

/**
 * @brief Function to generate a path from the current position to the target point
 * @note If any path is possible to generate, the dynamic obstacles will be disabled and the path will be generated
 * @see dj_genarate_path_with_param
 *
 * @param [out] path The path to generate
 * @param [in] target_point The target point
 * @param [in] retry_without_dynamic_obstacle If true, the robot will try to generate a path without dynamic obstacles
 *
 * @return true on success, false otherwise
 */
bool dj_genarate_path(dj_graph_path_t *path, GEOMETRY_point_t target_point, bool retry_without_dynamic_obstacle);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
