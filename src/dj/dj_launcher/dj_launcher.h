/**
 * @file dj_launcher.h
 * @brief Launcher for the dj path generation
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_launcher.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_goal_point.h"
#include "utils/dj/dj_graph/dj_graph_graph.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_importer.h"
#include "utils/dj/dj_solver/dj_solver_common/dj_solver_common.h"
#include "utils/maths/kinematics/linear_speed/linear_speed.h"
#include "utils/maths/polygon/polygon.h"
#include "utils/maths/position/position.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Launcher for the dj path generation
 */
typedef struct
{
    dj_graph_builder_t dj_launcher_builder;
    dj_solver_t dj_launcher_solver;
    dj_graph_path_t dj_launcher_retry_paths[DJ_ENGINE_MAX_DESTINATIONS];
} dj_launcher_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to generate paths from the current position to one or multiple target points
 * @note If any path is possible to generate, this function will not disable the dynamic obstacles
 * @see dj_generate_path
 * @param [in out] launcher The DJ launcher instance to use
 * @param [out] paths_out Array of paths (one per destination)
 * @param [out] successes_out Array of booleans indicating if each destination was reached
 * @param [in] start The start position
 * @param [in] start_speed The start linear speed
 * @param [in] target_points Array of target points
 * @param [in] target_count Number of target points
 * @param [in] enable_dynamic_obstacles Enable the dynamic obstacles
 * @param [in] obstacle_importer The obstacle importer to use
 * @param [in] prebuilt_graph The prebuilt graph to use
 * @param [in] solver_type The type of solver to use
 * @param [in] restricted_workspace_polygon The polygon to use as restricted workspace
 * @param [in] duration_calculator Callback function to calculate the duration of the path
 * @param [in] duration_calculator_args Additional arguments for the duration callback function
 * @return uint32_t Number of destinations successfully reached
 */
uint32_t dj_generate_path_with_param(dj_launcher_t *launcher,
                                     dj_graph_path_t *paths_out,
                                     bool *successes_out,
                                     const position_t *start,
                                     linear_speed_t start_speed,
                                     const dj_goal_point_t *target_points,
                                     uint32_t target_count,
                                     bool enable_dynamic_obstacles,
                                     dj_obstacle_importer_t *obstacle_importer,
                                     dj_graph_graph_t *prebuilt_graph,
                                     dj_solver_type_t solver_type,
                                     const polygon_t *restricted_workspace_polygon,
                                     dj_graph_path_get_duration_callback_t duration_calculator,
                                     void *duration_calculator_args);

/**
 * @brief Function to generate paths from the current position to one or multiple target points
 * @note If no path is found with dynamic obstacles, retries without them
 * @see dj_generate_path_with_param
 * @param [in out] launcher The DJ launcher instance to use
 * @param [out] paths_out Array of paths (one per destination)
 * @param [out] successes_out Array of booleans indicating if each destination was reached
 * @param [in] start The start position
 * @param [in] start_speed The start linear speed
 * @param [in] target_points Array of target points
 * @param [in] target_count Number of target points
 * @param [in] retry_without_dynamic_obstacle If true, retry without dynamic obstacles on failure
 * @param [in] obstacle_importer The obstacle importer to use
 * @param [in] prebuilt_graph The prebuilt graph to use
 * @param [in] solver_type The type of solver to use
 * @param [in] restricted_workspace_polygon The polygon to use as restricted workspace
 * @param [in] duration_calculator Callback function to calculate the duration of the path
 * @param [in] duration_calculator_args Additional arguments for the duration callback function
 * @return uint32_t Number of destinations successfully reached
 */
uint32_t dj_generate_path(dj_launcher_t *launcher,
                          dj_graph_path_t *paths_out,
                          bool *successes_out,
                          const position_t *start,
                          linear_speed_t start_speed,
                          const dj_goal_point_t *target_points,
                          uint32_t target_count,
                          bool retry_without_dynamic_obstacle,
                          dj_obstacle_importer_t *obstacle_importer,
                          dj_graph_graph_t *prebuilt_graph,
                          dj_solver_type_t solver_type,
                          const polygon_t *restricted_workspace_polygon,
                          dj_graph_path_get_duration_callback_t duration_calculator,
                          void *duration_calculator_args);

/* ******************************************* Public callback functions declarations ************************************ */
