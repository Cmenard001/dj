/**
 * @file dj_solver_astar.c
 * @brief Dj solver using the A* algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_solver_astar.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_solver_astar.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "utils/dj/dj_graph/dj_graph_link.h"
#include "utils/dj/dj_graph/dj_graph_node.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "utils/dj/dj_graph_builder/dj_graph_rebuilder.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/dj/dj_solver/dj_solver_common/dj_solver_common.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include <stdint.h>

LOG_REGISTER("utils/dj/solver/astar");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_solver_astar_init(dj_solver_t *solver,
                          dj_graph_builder_t *graph_builder,
                          dj_graph_graph_t *prebuilt_graph,
                          dj_solver_type_t type,
                          dj_graph_path_get_duration_callback_t duration_calculator,
                          void *duration_calculator_args)
{
    SYSTEM_ASSERT(solver != NULL);
    SYSTEM_ASSERT(graph_builder != NULL);
    SYSTEM_ASSERT(prebuilt_graph != NULL);
    SYSTEM_ASSERT(duration_calculator != NULL);
    solver->graph_builder = graph_builder;
    solver->prebuilt_graph = prebuilt_graph;
    solver->type = type;
    solver->duration_calculator = duration_calculator;
    solver->duration_calculator_args = duration_calculator_args;
    solver->type = type;
}

void dj_solver_astar_solve(dj_solver_t *solver,
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
    SYSTEM_ASSERT(goal_count > 0 && goal_count <= DJ_ENGINE_MAX_DESTINATIONS);
    SYSTEM_ASSERT(solutions_out != NULL);
    SYSTEM_ASSERT(found_out != NULL);

    LOGD("Start of the A* solve algorithm (%u destinations)", (unsigned)goal_count);
    dj_mark_start_time(SOLVE);

    // Initialize found flags
    uint32_t goals_remaining = goal_count;
    for (uint32_t g = 0; g < goal_count; g++)
    {
        found_out[g] = false;
    }

    dj_graph_path_t *current_path = NULL;
    bool path_end_reached = false;

    // List of path generated during the A* algorithm

    dj_solver_common_init_paths(&solver->path_list,
                                &current_path,
                                start_status,
                                goals->arrival_angle,
                                goals->use_angle,
                                initial_heading,
                                solver->duration_calculator,
                                solver->duration_calculator_args);
    SYSTEM_ASSERT(current_path != NULL);

    // Initialize node-to-path index and min-heap
    dj_solver_common_init_node_to_path(solver);
    dj_solver_common_heap_init(solver);

    // Register the initial path (index 0) in the heap
    dj_solver_common_heap_push(solver, 0, true, goals, goal_count, found_out);

    do
    {
        // Check if current path reaches any unsettled goal
        point_t current_end;
        dj_graph_path_get_end(current_path, &current_end);
        for (uint32_t g = 0; g < goal_count; g++)
        {
            if (!found_out[g] && current_end.x == goals[g].point.x &&
                current_end.y == goals[g].point.y)
            {
                found_out[g] = true;
                dj_graph_path_copy(&solutions_out[g], current_path);
                solutions_out[g].arrival_angle = goals[g].arrival_angle;
                solutions_out[g].use_arrival_angle = goals[g].use_angle;
                goals_remaining--;
                LOGD("Goal %u reached (remaining: %u)", (unsigned)g, (unsigned)goals_remaining);
                break;
            }
        }

        // All goals found, we can stop
        if (goals_remaining == 0)
        {
            break;
        }

        dj_viewer_status_t current_status;
        dj_graph_path_get_end_status(current_path, &current_status);
        // Rebuild the graph from the point of view of the current status
        dj_graph_builder_rebuild(solver->graph_builder, solver->prebuilt_graph, &current_status);

        path_end_reached = true; // Reset path_end_reached for each iteration

        // Get current node via position lookup
        dj_graph_node_t *current_node =
            dj_graph_graph_get_node_from_pos(&solver->graph_builder->graph,
                                             &current_status.position);

        // Get all nodes that are reachable from the end of the current path
        dj_mark_start_time(SOLVE_EXPLORE_LINKS);

        if (current_node != NULL)
        {
            for (uint8_t adj_idx = 0; adj_idx < current_node->adj_count; adj_idx++)
            {
                uint16_t link_index = current_node->adj_link_indices[adj_idx];
                dj_graph_link_t *link =
                    dj_graph_graph_get_link(&solver->graph_builder->graph, link_index);
                if (link == NULL || !link->enabled)
                {
                    continue;
                }

                // Get the other node
                dj_graph_node_t *node;
                if (link->node1 == current_node)
                {
                    node = link->node2;
                }
                else
                {
                    node = link->node1;
                }

                // Check if the node is already in the current path
                if (!dj_graph_path_is_in_path(current_path, &node->pos) && link->enabled)
                {
                    // Get node index for O(1) lookup
                    uint32_t node_idx =
                        dj_graph_graph_get_node_index(&solver->graph_builder->graph, node);
                    int32_t index_in_list = solver->node_to_path_index[node_idx];
                    if (index_in_list == -1)
                    {
                        // Add the new path to the list
                        dj_graph_path_t *new_path_added_to_list =
                            path_list_add(&solver->path_list, NULL);
                        if (new_path_added_to_list == NULL)
                        {
                            continue;
                        }
                        dj_graph_path_copy(new_path_added_to_list, current_path);
                        dj_graph_path_add_point(new_path_added_to_list, &node->pos);
                        uint16_t new_idx = (uint16_t)(path_list_size(&solver->path_list) - 1u);
                        solver->node_to_path_index[node_idx] = (int32_t)new_idx;
                        dj_solver_common_heap_push(
                            solver, new_idx, true, goals, goal_count, found_out);
                        path_end_reached = false;
                    }
                    else
                    {
                        // Check if the new path is better than the one already in the list
                        dj_graph_path_t *existing_path =
                            path_list_get(&solver->path_list, (uint32_t)index_in_list);
                        if (existing_path == NULL)
                        {
                            continue;
                        }
                        dj_graph_path_copy(&solver->temp_path, current_path);
                        dj_graph_path_add_point(&solver->temp_path, &node->pos);
                        if (dj_graph_path_get_duration(&solver->temp_path) <
                            dj_graph_path_get_duration(existing_path))
                        {
                            // Replace the existing path with the new one
                            dj_graph_path_copy(existing_path, &solver->temp_path);
                            // Re-insert into heap (lazy update)
                            dj_solver_common_heap_push(solver,
                                                       (uint16_t)index_in_list,
                                                       true,
                                                       goals,
                                                       goal_count,
                                                       found_out);
                            path_end_reached = false;
                        }
                    }
                }
            }
        }

        // The current path is finished if we have reached any node
        if (path_end_reached)
        {
            dj_graph_path_set_as_finished(current_path, true);
        }
        dj_mark_end_time(SOLVE_EXPLORE_LINKS);

        // Use heuristic: min distance to any unreached goal
        dj_mark_start_time(SOLVE_FIND_NEXT);
        // Pop next unfinished path from heap (skip already-finished entries)
        current_path = NULL;
        while (solver->heap_size > 0)
        {
            dj_graph_path_t *candidate =
                dj_solver_common_heap_pop(solver, true, goals, goal_count, found_out);
            if (candidate != NULL && !candidate->is_finished)
            {
                current_path = candidate;
                break;
            }
        }
        dj_mark_end_time(SOLVE_FIND_NEXT);
    } while (current_path != NULL);

    dj_mark_end_time(SOLVE);
    LOGD("End of the A* solve algorithm (%u/%u goals found)",
         (unsigned)(goal_count - goals_remaining),
         (unsigned)goal_count);
}

/* ******************************************* Public callback functions declarations ************************************ */
