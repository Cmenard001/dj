/**
 * @file dj_solver.c
 * @brief Interface of dj solver
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_solver.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_solver.h"
#include "../dj_dependencies/dj_dependencies.h"

#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_solver_init(dj_solver_t *solver, dj_graph_builder_t *graph_builder)
{
#if DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_DIJKSTRA
    dj_solver_dijkstra_init((dj_solver_dijkstra_t *)solver, graph_builder);
#elif DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_ASTAR
    dj_solver_astar_init((dj_solver_astar_t *)solver, graph_builder);
#endif
}

void dj_solver_deinit(dj_solver_t *solver)
{
#if DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_DIJKSTRA
    dj_solver_dijkstra_deinit((dj_solver_dijkstra_t *)solver);
#elif DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_ASTAR
    dj_solver_astar_deinit((dj_solver_astar_t *)solver);
#endif
}

void dj_solver_solve(dj_solver_t *solver, dj_viewer_status_t *start_status)
{
#if DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_DIJKSTRA
    dj_solver_dijkstra_solve((dj_solver_dijkstra_t *)solver, start_status);
#elif DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_ASTAR
    dj_solver_astar_solve((dj_solver_astar_t *)solver, start_status);
#endif
}

void dj_solver_get_solution(dj_solver_t *solver, dj_graph_path_t *solution)
{
#if DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_DIJKSTRA
    dj_solver_dijkstra_get_solution((dj_solver_dijkstra_t *)solver, solution);
#elif DJ_SOLVER_TYPE == DJ_SOLVER_TYPE_ASTAR
    dj_solver_astar_get_solution((dj_solver_astar_t *)solver, solution);
#endif
}

/* ******************************************* Public callback functions declarations ************************************ */
