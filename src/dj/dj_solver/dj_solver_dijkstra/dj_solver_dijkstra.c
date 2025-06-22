/**
 * @file dj_solver_dijkstra.c
 * @brief Dj solver using the dijkstra algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_solver_dijkstra.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_solver_dijkstra.h"

#include "../../dj_dependencies/dj_dependencies.h"
#include "../../dj_graph/dj_graph_node.h"
#include "../../dj_graph/dj_graph_path.h"
#include "../../dj_graph_builder/dj_graph_builder.h"
#include "../../dj_graph_builder/dj_prebuilt_graph.h"
#include "../../dj_logs/dj_logs.h"
#include "../../dj_logs/dj_time_marker.h"
#include "../../dj_viewer/dj_viewer_status.h"
#include "../dj_solver_common/dj_solver_common.h"

#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/**
 * @brief list of path generated during the dijkstra algorithm
 * @note This array is global to avoid stack overflow
 */
static path_list_t path_list;

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_solver_dijkstra_init(dj_solver_dijkstra_t *solver, dj_graph_builder_t *graph_builder)
{
    dj_control_non_null(solver, ) dj_control_non_null(graph_builder, ) solver->graph_builder = graph_builder;
    // Initialize the solution with invalid values
    dj_viewer_status_t start_status;
    viewer_status_init(&start_status, &(GEOMETRY_point_t){0, 0}, &(GEOMETRY_vector_t){0, 0}, 0);
    dj_graph_path_init(&solver->solution, &start_status);
}

void dj_solver_dijkstra_deinit(dj_solver_dijkstra_t *solver)
{
    dj_control_non_null(solver, ) dj_graph_path_deinit(&solver->solution);
}

void dj_solver_dijkstra_solve(dj_solver_dijkstra_t *solver, dj_viewer_status_t *start_status)
{
    dj_control_non_null(solver, ) dj_control_non_null(start_status, )

        dj_debug_printf("Start of the solve algorithm\n");
    dj_mark_start_time(DJ_MARK_SOLVE);

    dj_graph_path_t *current_path = NULL;
    bool path_end_reached = false;

    dj_solver_common_init_paths(&path_list, &current_path, start_status);
    dj_control_non_null(current_path, );

    do
    {
        dj_viewer_status_t current_status;
        dj_graph_path_get_end_status(current_path, &current_status);
        rebuild_graph_with_prebuilt_graph(solver->graph_builder, &current_status);

        path_end_reached = true; // Reset path_end_reached for each iteration

        // Get all nodes that are reachable from the end of the current path
        for (uint32_t link_index = 0; link_index < dj_graph_graph_get_num_links(&solver->graph_builder->m_graph);
             link_index++)
        {
            // Get the link
            dj_graph_link_t *link = dj_graph_graph_get_link(&solver->graph_builder->m_graph, link_index);

            // Check if the link is reachable
            dj_graph_node_t *node = dj_graph_link_is_connected_to_pos(link, &current_status.m_position);
            if (node == NULL)
            {
                continue;
            }

            if (node != NULL && !dj_graph_path_is_in_path(current_path, &node->m_pos) && link->m_enabled)
            {
                // Check if the end point is already in the path_list
                int32_t index_in_list = dj_graph_solver_index_in_list(&path_list, &node->m_pos);
                if (index_in_list == -1)
                {
                    // Add the new path to the list
                    dj_graph_path_t *new_path_added_to_list = path_list_add(&path_list, NULL);
                    dj_control_non_null(new_path_added_to_list, ) dj_graph_path_copy(new_path_added_to_list, current_path);
                    dj_graph_path_add_point(new_path_added_to_list, &node->m_pos);
                    path_end_reached = false;
                }
                else
                {
                    // Check if the new path is better than the one already in the list
                    dj_graph_path_t *existing_path = path_list_get(&path_list, index_in_list);
                    dj_control_non_null(existing_path, ) dj_graph_path_t new_path_to_check;
                    dj_graph_path_copy(&new_path_to_check, current_path);
                    dj_graph_path_add_point(&new_path_to_check, &node->m_pos);
                    if (dj_graph_path_get_duration(&new_path_to_check) < dj_graph_path_get_duration(existing_path))
                    {
                        // Replace the existing path with the new one
                        dj_graph_path_deinit(existing_path);
                        dj_graph_path_copy(existing_path, &new_path_to_check);
                        path_end_reached = false;
                    }
                    dj_graph_path_deinit(&new_path_to_check);
                }
            }
        }

        // The current path is finished if we have reached any node
        if (path_end_reached)
        {
            dj_graph_path_set_as_finished(current_path, true);
        }

        current_path = dj_solver_common_find_next_path(&path_list, false, NULL);
    } while (current_path != NULL
             && (current_path->m_end_status.m_position.x != solver->graph_builder->m_graph.end->m_pos.x
                 || current_path->m_end_status.m_position.y != solver->graph_builder->m_graph.end->m_pos.y));

    dj_mark_end_time(DJ_MARK_SOLVE);
    dj_debug_printf("End of the solve algorithm\n");

    // Print all the paths generated
    // Print all the nodes
#if DJ_ENABLE_DEBUG_LOGS
    dj_debug_printf("Genrated paths:\n");
    for (int i = 0; i < path_list_size(&path_list); i++)
    {
        dj_graph_path_t *path = path_list_get(&path_list, i);
        dj_debug_printf("Path %d: (%d ms)", i, (int)dj_graph_path_get_duration(path));
        for (int j = 0; j < dj_graph_path_get_point_count(path); j++)
        {
            GEOMETRY_point_t point;
            dj_graph_path_get_point(path, &point, j);
            dj_debug_printf("(%d, %d) ", point.x, point.y);
        }
        dj_debug_printf("\n");
    }
#endif

    // Shearch the solution in the path list
    for (uint32_t i = 0; i < path_list_size(&path_list); i++)
    {
        dj_graph_path_t *path = path_list_get(&path_list, i);
        dj_control_non_null(path, ) if (path->m_end_status.m_position.x == solver->graph_builder->m_graph.end->m_pos.x
                                        && path->m_end_status.m_position.y == solver->graph_builder->m_graph.end->m_pos.y)
        {
            dj_graph_path_copy(&solver->solution, path);
            dj_debug_printf("Solution found\n");
            break;
        }
    }

    dj_solver_common_deinit_paths(&path_list);
}

void dj_solver_dijkstra_get_solution(dj_solver_dijkstra_t *solver, dj_graph_path_t *solution)
{
    dj_graph_path_copy(solution, &solver->solution);
}

/* ******************************************* Public callback functions declarations ************************************ */
