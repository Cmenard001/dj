/**
 * @file dj_solver_dijkstra.h
 * @brief Dj solver using the dijkstra algorithm
 * @author Cyprien Ménard
 * @date 29/01/2026
 * @see dj_solver_dijkstra.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "utils/dj/dj_goal_point.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "utils/dj/dj_solver/dj_solver_common/dj_solver_common.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize the solver
 * @param solver Pointer to the solver to initialize
 * @param graph_builder Pointer to the graph builder to use
 * @param prebuilt_graph Pointer to the prebuilt graph to use
 * @param type Type of the solver to initialize (should be DJ_SOLVER_DIJKSTRA)
 */
void dj_solver_dijkstra_init(dj_solver_t *solver,
                             dj_graph_builder_t *graph_builder,
                             dj_graph_graph_t *prebuilt_graph,
                             dj_solver_type_t type,
                             dj_graph_path_get_duration_callback_t duration_calculator,
                             void *duration_calculator_args);

/**
 * @brief Function to solve the graph for one or multiple destinations
 * @param solver Pointer to the solver
 * @param start_status Initial status of the viewer
 * @param initial_heading Initial heading of the viewer
 * @param goals Array of goal positions
 * @param goal_count Number of goals
 * @param solutions_out Array of paths (one per goal), filled on success
 * @param found_out Array of booleans indicating if each goal was reached
 */
void dj_solver_dijkstra_solve(dj_solver_t *solver,
                              dj_viewer_status_t *start_status,
                              angle_t initial_heading,
                              const dj_goal_point_t *goals,
                              uint32_t goal_count,
                              dj_graph_path_t *solutions_out,
                              bool *found_out);

/* ******************************************* Public callback functions declarations ************************************ */
