/**
 * @file dj_solver_astar.h
 * @brief Dj solver using the A* algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_solver_astar.c
 */

#ifndef __DJ_SOLVER_DIJKSTRA_H__
#define __DJ_SOLVER_DIJKSTRA_H__

/* ******************************************************* Includes ****************************************************** */

#include "../../dj_graph/dj_graph_graph.h"
#include "../../dj_graph/dj_graph_path.h"
#include "../../dj_graph_builder/dj_graph_builder.h"
#include "../../dj_viewer/dj_viewer_status.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure to store the A* solver
 */
typedef struct
{
    /**
     * @brief Graph builder to use
     */
    dj_graph_builder_t *graph_builder;
    /**
     * @brief Solution of the solver
     */
    dj_graph_path_t solution;
} dj_solver_astar_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize the solver
 *
 * @param solver Pointer to the solver to initialize
 * @param graph_builder Pointer to the graph builder to use
 */
void dj_solver_astar_init(dj_solver_astar_t *solver, dj_graph_builder_t *graph_builder);

/**
 * @brief Function to deinitialize the solver
 *
 * @param solver Pointer to the solver to deinitialize
 */
void dj_solver_astar_deinit(dj_solver_astar_t *solver);

/**
 * @brief Function to solve the graph
 *
 * @param solver Pointer to the solver to solve
 */
void dj_solver_astar_solve(dj_solver_astar_t *solver, dj_viewer_status_t *start_status);

/**
 * @brief Function to get the solution
 *
 * @param solver Pointer to the solver to get the solution from
 * @param solution Pointer to the path to store the solution
 */
void dj_solver_astar_get_solution(dj_solver_astar_t *solver, dj_graph_path_t *solution);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
