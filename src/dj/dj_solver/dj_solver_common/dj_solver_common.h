/**
 * @file dj_solver_common.h
 * @brief Common files for the dj solvers
 * @author Cyprien Ménard
 * @date 23/03/2025
 * @see dj_solver_common.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */
#include "utils/dj/dj_config.h"
#include "utils/dj/dj_goal_point.h"
#include "utils/dj/dj_graph/dj_graph_graph.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include "utils/lists/magic_array.h"

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Mother attribute for all solvers
 */
#define DJ_SOLVER_ATTRIBUTE_MOTHER                                                                 \
    dj_graph_builder_t *graph_builder;                                                             \
    dj_graph_graph_t *prebuilt_graph;                                                              \
    dj_solver_type_t type;                                                                         \
    dj_graph_path_get_duration_callback_t duration_calculator;                                     \
    void *duration_calculator_args;                                                                \
    path_list_t path_list;                                                                         \
    dj_graph_path_t temp_path;                                                                     \
    int32_t node_to_path_index[DJ_GRAPH_GRAPH_MAX_NODES];                                          \
    uint16_t min_heap[DJ_GRAPH_GRAPH_MAX_NODES];                                                   \
    uint16_t heap_size;

/* ************************************************** Public types definition ******************************************** */

MAGIC_ARRAY_TYPEDEF(path_list, dj_graph_path_t, DJ_GRAPH_GRAPH_MAX_NODES);

/**
 * @brief Enum to define the type of solver
 */
typedef enum
{
    DJ_SOLVER_ASTAR,
    DJ_SOLVER_DIJKSTRA,
} dj_solver_type_t;

/**
 * @brief Structure to store the solver
 */
typedef struct
{
    DJ_SOLVER_ATTRIBUTE_MOTHER
} dj_solver_t;

/* *********************************************** Public functions declarations ***************************************** */

MAGIC_ARRAY_DECLARATION(path_list, dj_graph_path_t)

/**
 * @brief Function to check if a path already goes to the same point in the list
 * @param list List of path to check
 * @param path Path to check
 * @return int32_t Index of the path in the list, -1 if not found
 */
int32_t dj_graph_solver_index_in_list(path_list_t *list, point_t *end_point);

/**
 * @brief Function to initialize the path list
 * @param path_list List of path to initialize
 * @param current_path Pointer to the current path
 * @param start_status Start status of the path
 * @param arrival_angle Desired arrival angle [rad]
 * @param use_arrival_angle If true, a final rotation to arrival_angle is required
 * @param init_heading Initial heading of the robot [rad]
 * @param duration_calculator Callback function to calculate the duration of the path
 * @param duration_calculator_args Additional arguments for the duration callback function
 */
void dj_solver_common_init_paths(path_list_t *path_list,
                                 dj_graph_path_t **current_path,
                                 dj_viewer_status_t *start_status,
                                 angle_t arrival_angle,
                                 bool use_arrival_angle,
                                 angle_t init_heading,
                                 dj_graph_path_get_duration_callback_t duration_calculator,
                                 void *duration_calculator_args);

/**
 * @brief Function to find the next path to explore
 * @param path_list List of path to explore
 * @param use_heuristic Use the heuristic to find the next path
 * @param goal_positions Array of goal positions (used for heuristic)
 * @param goal_count Number of goals
 * @param goals_found Array indicating which goals have been found
 * @return dj_graph_path_t* Next path to explore
 * @note For A*, the heuristic uses the minimum distance to any unreached goal
 */
dj_graph_path_t *dj_solver_common_find_next_path(path_list_t *path_list,
                                                 bool use_heuristic,
                                                 const dj_goal_point_t *goal_positions,
                                                 uint32_t goal_count,
                                                 const bool *goals_found);

/**
 * @brief Initialize the node-to-path index mapping
 * @param solver The solver to initialize the mapping for
 */
void dj_solver_common_init_node_to_path(dj_solver_t *solver);

/**
 * @brief Initialize the min-heap
 * @param solver The solver containing the heap
 */
void dj_solver_common_heap_init(dj_solver_t *solver);

/**
 * @brief Push an element onto the min-heap
 * @param solver The solver containing the heap
 * @param path_index Index of the path in path_list
 * @param use_heuristic Whether to use A* heuristic
 * @param goals Goal positions (for heuristic)
 * @param goal_count Number of goals
 * @param goals_found Which goals are found
 */
void dj_solver_common_heap_push(dj_solver_t *solver,
                                uint16_t path_index,
                                bool use_heuristic,
                                const dj_goal_point_t *goals,
                                uint32_t goal_count,
                                const bool *goals_found);

/**
 * @brief Pop the minimum element from the heap
 * @param solver The solver containing the heap
 * @param use_heuristic Whether to use A* heuristic
 * @param goals Goal positions (for heuristic)
 * @param goal_count Number of goals
 * @param goals_found Which goals are found
 * @return dj_graph_path_t* The path with minimum cost, or NULL if heap is empty
 */
dj_graph_path_t *dj_solver_common_heap_pop(dj_solver_t *solver,
                                           bool use_heuristic,
                                           const dj_goal_point_t *goals,
                                           uint32_t goal_count,
                                           const bool *goals_found);

/* ******************************************* Public callback functions declarations ************************************ */
