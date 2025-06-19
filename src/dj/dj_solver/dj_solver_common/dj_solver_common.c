/**
 * @file dj_solver_common.c
 * @brief Common file for the dj solvers
 * @author Cyprien MÃ©nard
 * @date 23/03/2025
 * @see dj_solver_common.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_solver_common.h"
#include "../../dj_logs/dj_logs.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

MAGIC_ARRAY_SRC(path_list, dj_graph_path_t);

int32_t dj_graph_solver_index_in_list(path_list_t *list, GEOMETRY_point_t *end_point)
{
    dj_control_non_null(list, -1);
    dj_control_non_null(end_point, -1);

    for (int32_t i = 0; i < path_list_size(list); i++)
    {
        GEOMETRY_point_t current_end_point;
        dj_graph_path_get_end(path_list_get(list, i), &current_end_point);
        if (end_point->x == current_end_point.x && end_point->y == current_end_point.y)
        {
            return i;
        }
    }
    return -1;
}

void dj_solver_common_init_paths(path_list_t *path_list, dj_graph_path_t **current_path, dj_viewer_status_t *start_status)
{
    dj_control_non_null(path_list, );
    dj_control_non_null(current_path, );
    dj_control_non_null(start_status, );

    path_list_init(path_list);
    dj_graph_path_t *new_path = path_list_add(path_list, NULL);
    dj_graph_path_init(new_path, start_status);
    *current_path = new_path;
}

void dj_solver_common_deinit_paths(path_list_t *path_list)
{
    dj_control_non_null(path_list, );
    for (uint32_t i = 0; i < path_list_size(path_list); i++)
    {
        dj_graph_path_deinit(path_list_get(path_list, i));
    }
}

dj_graph_path_t *dj_solver_common_find_next_path(path_list_t *path_list, bool use_heuristic, GEOMETRY_point_t *goal_position)
{
    dj_control_non_null(path_list, NULL);

    dj_graph_path_t *next_path = NULL;
    uint32_t path_list_count = path_list_size(path_list);

    for (int i = 0; i < path_list_count; i++)
    {
        dj_graph_path_t *path = path_list_get(path_list, i);
        if (!dj_graph_path_is_finished(path))
        {
            int32_t cost = dj_graph_path_get_duration(path);
            if (use_heuristic && goal_position != NULL)
            {
                GEOMETRY_point_t current_end_point;
                dj_graph_path_get_end(path, &current_end_point);
                cost += GEOMETRY_manhattan_distance(current_end_point, *goal_position);
            }

            if (next_path == NULL || cost < dj_graph_path_get_duration(next_path))
            {
                next_path = path;
            }
        }
    }

    return next_path;
}

/* ***************************************** Public callback functions definitions *************************************** */
