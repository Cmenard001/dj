/**
 * @file dj_solver.h
 * @brief Interface of dj solver
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_solver.c
 */

/* ******************************************************* Includes ****************************************************** */

#ifndef __DJ_SOLVER_H__
#define __DJ_SOLVER_H__

#include "../dj_graph/dj_graph_path.h"
#include "../dj_graph_builder/dj_graph_builder.h"

#define DJ_SOLVER_TYPE_DIJKSTRA 0
#define DJ_SOLVER_TYPE_ASTAR 1

#define DJ_SOLVER_TYPE DJ_SOLVER_TYPE_ASTAR

#if (DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_DIJKSTRA)
#include "dj_solver_dijkstra/dj_solver_dijkstra.h"
#elif (DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_ASTAR)
#include "dj_solver_astar/dj_solver_astar.h"
#endif

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

#if DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_DIJKSTRA
typedef dj_solver_dijkstra_t dj_solver_t;
#elif DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_ASTAR
typedef dj_solver_astar_t dj_solver_t;
#endif

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a solver
 *
 * @param solver Pointer to the solver to initialize
 * @param graph_builder Pointer to the graph builder to use
 */
void dj_solver_init(dj_solver_t *solver, dj_graph_builder_t *graph_builder);

/**
 * @brief Function to deinitialize a solver
 *
 * @param solver Pointer to the solver to deinitialize
 */
void dj_solver_deinit(dj_solver_t *solver);

/**
 * @brief Function to solve the graph
 *
 * @param solver Pointer to the solver to solve
 */
void dj_solver_solve(dj_solver_t *solver, dj_viewer_status_t *start_status);

/**
 * @brief Function to get the solution
 *
 * @param solver Pointer to the solver to get the solution from
 * @param solution Pointer to the path to store the solution
 */
void dj_solver_get_solution(dj_solver_t *solver, dj_graph_path_t *solution);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
