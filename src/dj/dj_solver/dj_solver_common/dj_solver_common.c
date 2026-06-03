/**
 * @file dj_solver_common.c
 * @brief Common file for the dj solvers
 * @author Cyprien Ménard
 * @date 23/03/2025
 * @see dj_solver_common.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_solver_common.h"
#include "system/assert/system_assert.h"
#include "utils/lists/magic_array.h"
#include <string.h>

LOG_REGISTER("utils/dj/solver/common");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

MAGIC_ARRAY_DECLARATION(path_list, dj_graph_path_t);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

MAGIC_ARRAY_DEFINITION(path_list, dj_graph_path_t);

int32_t dj_graph_solver_index_in_list(path_list_t *list, point_t *end_point)
{
    SYSTEM_ASSERT(list != NULL);
    SYSTEM_ASSERT(end_point != NULL);

    for (uint32_t i = 0; i < path_list_size(list); i++)
    {
        point_t current_end_point;
        dj_graph_path_get_end(path_list_get(list, i), &current_end_point);
        if (end_point->x == current_end_point.x && end_point->y == current_end_point.y)
        {
            return (int32_t)i;
        }
    }
    return -1;
}

void dj_solver_common_init_paths(path_list_t *path_list,
                                 dj_graph_path_t **current_path,
                                 dj_viewer_status_t *start_status,
                                 angle_t arrival_angle,
                                 bool use_arrival_angle,
                                 angle_t init_heading,
                                 dj_graph_path_get_duration_callback_t duration_calculator,
                                 void *duration_calculator_args)
{
    SYSTEM_ASSERT(path_list != NULL);
    SYSTEM_ASSERT(current_path != NULL);
    SYSTEM_ASSERT(start_status != NULL);

    path_list_init(path_list);
    dj_graph_path_t *new_path = path_list_add(path_list, NULL);
    dj_graph_path_init(new_path,
                       start_status,
                       arrival_angle,
                       use_arrival_angle,
                       init_heading,
                       duration_calculator,
                       duration_calculator_args);
    *current_path = new_path;
}

dj_graph_path_t *dj_solver_common_find_next_path(path_list_t *path_list,
                                                 bool use_heuristic,
                                                 const dj_goal_point_t *goal_positions,
                                                 uint32_t goal_count,
                                                 const bool *goals_found)
{
    SYSTEM_ASSERT(path_list != NULL);

    dj_graph_path_t *next_path = NULL;
    time_ms_t best_cost = 0;
    uint32_t path_list_count = path_list_size(path_list);

    for (uint32_t i = 0; i < path_list_count; i++)
    {
        dj_graph_path_t *path = path_list_get(path_list, i);
        if (!dj_graph_path_is_finished(path))
        {
            time_ms_t cost = dj_graph_path_get_duration(path);
            if (use_heuristic && goal_positions != NULL && goals_found != NULL)
            {
                // Use minimum manhattan distance to any unreached goal as heuristic
                point_t current_end_point;
                dj_graph_path_get_end(path, &current_end_point);
                time_ms_t min_heuristic = 0;
                bool first = true;
                for (uint32_t g = 0; g < goal_count; g++)
                {
                    if (!goals_found[g])
                    {
                        time_ms_t h = (time_ms_t)point_manhattan_distance(
                            &current_end_point, (point_t *)&goal_positions[g].point);
                        if (first || h < min_heuristic)
                        {
                            min_heuristic = h;
                            first = false;
                        }
                    }
                }
                cost += min_heuristic;
            }

            if (next_path == NULL || cost < best_cost)
            {
                next_path = path;
                best_cost = cost;
            }
        }
    }

    return next_path;
}

void dj_solver_common_init_node_to_path(dj_solver_t *solver)
{
    SYSTEM_ASSERT(solver != NULL);
    memset(solver->node_to_path_index, -1, sizeof(solver->node_to_path_index));
}

/* ---- Min-heap helpers ---- */

static time_ms_t heap_cost(dj_solver_t *solver,
                           uint16_t path_index,
                           bool use_heuristic,
                           const dj_goal_point_t *goals,
                           uint32_t goal_count,
                           const bool *goals_found)
{
    dj_graph_path_t *path = path_list_get(&solver->path_list, path_index);
    time_ms_t cost = dj_graph_path_get_duration(path);
    if (use_heuristic && goals != NULL)
    {
        point_t end_point;
        dj_graph_path_get_end(path, &end_point);
        time_ms_t min_h = 0;
        bool first = true;
        for (uint32_t g = 0; g < goal_count; g++)
        {
            if (!goals_found[g])
            {
                time_ms_t h =
                    (time_ms_t)point_manhattan_distance(&end_point, (point_t *)&goals[g].point);
                if (first || h < min_h)
                {
                    min_h = h;
                    first = false;
                }
            }
        }
        cost += min_h;
    }
    return cost;
}

void dj_solver_common_heap_init(dj_solver_t *solver)
{
    SYSTEM_ASSERT(solver != NULL);
    solver->heap_size = 0;
}

void dj_solver_common_heap_push(dj_solver_t *solver,
                                uint16_t path_index,
                                bool use_heuristic,
                                const dj_goal_point_t *goals,
                                uint32_t goal_count,
                                const bool *goals_found)
{
    SYSTEM_ASSERT(solver != NULL);
    if (solver->heap_size >= DJ_GRAPH_GRAPH_MAX_NODES)
    {
        return;
    }
    uint16_t i = solver->heap_size;
    solver->min_heap[i] = path_index;
    solver->heap_size++;

    time_ms_t new_cost =
        heap_cost(solver, path_index, use_heuristic, goals, goal_count, goals_found);

    // Sift up
    while (i > 0)
    {
        uint16_t parent = (uint16_t)((i - 1u) / 2u);
        time_ms_t parent_cost = heap_cost(
            solver, solver->min_heap[parent], use_heuristic, goals, goal_count, goals_found);
        if (new_cost < parent_cost)
        {
            // Swap
            uint16_t tmp = solver->min_heap[parent];
            solver->min_heap[parent] = solver->min_heap[i];
            solver->min_heap[i] = tmp;
            i = parent;
        }
        else
        {
            break;
        }
    }
}

dj_graph_path_t *dj_solver_common_heap_pop(dj_solver_t *solver,
                                           bool use_heuristic,
                                           const dj_goal_point_t *goals,
                                           uint32_t goal_count,
                                           const bool *goals_found)
{
    SYSTEM_ASSERT(solver != NULL);
    if (solver->heap_size == 0)
    {
        return NULL;
    }

    uint16_t top_index = solver->min_heap[0];
    solver->heap_size--;
    if (solver->heap_size > 0)
    {
        solver->min_heap[0] = solver->min_heap[solver->heap_size];
        // Sift down
        uint16_t i = 0;
        while (true)
        {
            uint16_t left = (uint16_t)(2u * i + 1u);
            uint16_t right = (uint16_t)(2u * i + 2u);
            uint16_t smallest = i;
            time_ms_t smallest_cost = heap_cost(
                solver, solver->min_heap[smallest], use_heuristic, goals, goal_count, goals_found);

            if (left < solver->heap_size)
            {
                time_ms_t left_cost = heap_cost(
                    solver, solver->min_heap[left], use_heuristic, goals, goal_count, goals_found);
                if (left_cost < smallest_cost)
                {
                    smallest = left;
                    smallest_cost = left_cost;
                }
            }
            if (right < solver->heap_size)
            {
                time_ms_t right_cost = heap_cost(
                    solver, solver->min_heap[right], use_heuristic, goals, goal_count, goals_found);
                if (right_cost < smallest_cost)
                {
                    smallest = right;
                }
            }
            if (smallest != i)
            {
                uint16_t tmp = solver->min_heap[i];
                solver->min_heap[i] = solver->min_heap[smallest];
                solver->min_heap[smallest] = tmp;
                i = smallest;
            }
            else
            {
                break;
            }
        }
    }

    dj_graph_path_t *path = path_list_get(&solver->path_list, top_index);
    return path;
}

/* ***************************************** Public callback functions definitions *************************************** */
