/**
 * @file dj_solver.c
 * @brief Interface of dj solver
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_solver.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_solver/dj_solver.h"
#include "system/assert/system_assert.h"
#include <stdlib.h>
/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_solver_init(dj_solver_t *solver,
                    dj_graph_builder_t *graph_builder,
                    dj_graph_graph_t *prebuilt_graph,
                    dj_solver_type_t type,
                    dj_graph_path_get_duration_callback_t duration_calculator,
                    void *duration_calculator_args)
{
    SYSTEM_ASSERT(solver != NULL);
    SYSTEM_ASSERT(graph_builder != NULL);
    switch (type)
    {
        case DJ_SOLVER_DIJKSTRA:
            dj_solver_dijkstra_init(solver,
                                    graph_builder,
                                    prebuilt_graph,
                                    type,
                                    duration_calculator,
                                    duration_calculator_args);
            break;
        case DJ_SOLVER_ASTAR:
            dj_solver_astar_init(solver,
                                 graph_builder,
                                 prebuilt_graph,
                                 type,
                                 duration_calculator,
                                 duration_calculator_args);
            break;
        default:
            SYSTEM_ASSERT(false);
            break;
    }
}

void dj_solver_solve(dj_solver_t *solver,
                     dj_viewer_status_t *start_status,
                     angle_t initial_heading,
                     const dj_goal_point_t *goals,
                     uint32_t goal_count,
                     dj_graph_path_t *solutions_out,
                     bool *found_out)
{
    SYSTEM_ASSERT(solver != NULL);
    SYSTEM_ASSERT(start_status != NULL);
    SYSTEM_ASSERT(goals != NULL);
    SYSTEM_ASSERT(solutions_out != NULL);
    SYSTEM_ASSERT(found_out != NULL);

    switch (solver->type)
    {
        case DJ_SOLVER_DIJKSTRA:
            dj_solver_dijkstra_solve(
                solver, start_status, initial_heading, goals, goal_count, solutions_out, found_out);
            break;
        case DJ_SOLVER_ASTAR:
            dj_solver_astar_solve(
                solver, start_status, initial_heading, goals, goal_count, solutions_out, found_out);
            break;
        default:
            SYSTEM_ASSERT(false);
            break;
    }
}

/* ******************************************* Public callback functions declarations ************************************ */
