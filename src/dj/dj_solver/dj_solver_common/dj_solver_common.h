/**
 * @file dj_solver_common.h
 * @brief Common files for the dj solvers
 * @author Cyprien MÃ©nard
 * @date 23/03/2025
 * @see dj_solver_common.c
 */

#ifndef __DJ_SOLVER_COMMON_H__
#define __DJ_SOLVER_COMMON_H__

/* ******************************************************* Includes ****************************************************** */

#include "../../dj_dependencies/dj_dependencies.h"
#include "../../dj_graph/dj_graph_graph.h"
#include "../../dj_graph/dj_graph_path.h"
#include "../../dj_viewer/dj_viewer_status.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

MAGIC_ARRAY_TYPEDEF(path_list, dj_graph_path_t, DJ_GRAPH_GRAPH_MAX_NODES);

/* *********************************************** Public functions declarations ***************************************** */

MAGIC_ARRAY_HEADER(path_list, dj_graph_path_t)

/**
 * @brief Function to check if a path already goes to the same point in the list
 *
 * @param list List of path to check
 * @param path Path to check
 * @return int32_t Index of the path in the list, -1 if not found
 */
int32_t dj_graph_solver_index_in_list(path_list_t *list, GEOMETRY_point_t *end_point);

/**
 * @brief Function to initialize the path list
 *
 * @param path_list List of path to initialize
 * @param current_path Pointer to the current path
 * @param start_status Start status of the path
 */
void dj_solver_common_init_paths(path_list_t *path_list, dj_graph_path_t **current_path, dj_viewer_status_t *start_status);

/**
 * @brief Function to deinitialize the paths
 *
 * @param path_list List of path to deinitialize
 */
void dj_solver_common_deinit_paths(path_list_t *path_list);

/**
 * @brief Function to find the next path to explore
 *
 * @param path_list List of path to explore
 * @param use_heuristic Use the heuristic to find the next path
 * @param goal_position Position of the goal
 * @return dj_graph_path_t* Next path to explore
 */
dj_graph_path_t *dj_solver_common_find_next_path(path_list_t *path_list,
                                                 bool use_heuristic,
                                                 GEOMETRY_point_t *goal_position);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
