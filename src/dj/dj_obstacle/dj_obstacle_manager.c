/**
 * @file dj_obstacle_manager.h
 * @brief Manager for all obstacles
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_obstacle_manager.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle_manager.h"
#include "../dj_logs/dj_logs.h"
#include "../dj_logs/dj_time_marker.h"
#include "../dj_obstacle_importer/dj_obstacle_id.h"
#include "../dj_obstacle_importer/dj_obstacle_importer.h"

#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

MAGIC_ARRAY_SRC(static_obstacles_list, dj_obstacle_static_t);

static void compute_dynamic_obstacles(dj_obstacle_manager_t *manager, dj_viewer_status_t *viewer);

/* ************************************************** Private variables ************************************************** */

/**
 * @brief Compute the dynamic obstacles to get the static obstacles in space time (so from the point of view of a viewer)
 * @see dj_obstacle_dynamic_get_position
 *
 * @param manager The obstacle manager object
 * @param viewer The viewer status (NULL if you want only static obstacles)
 */
static void compute_dynamic_obstacles(dj_obstacle_manager_t *manager, dj_viewer_status_t *viewer)
{
    dj_control_non_null(manager, )
        // Check if the same as the last computed viewer or if the viewer is NULL
        // Viewer NULL --> no viewer, so no dynamic obstacles
        if (viewer == NULL)
    {
        return;
    }
    if (viewer->m_time == manager->m_last_computed_viewer.m_time
        && viewer->m_position.x == manager->m_last_computed_viewer.m_position.x
        && viewer->m_position.y == manager->m_last_computed_viewer.m_position.y
        && viewer->m_speed.x == manager->m_last_computed_viewer.m_speed.x
        && viewer->m_speed.y == manager->m_last_computed_viewer.m_speed.y && manager->m_last_viewer_is_valid)
    {
        return;
    }
    // Check if the static obstacles are added
    if (!manager->m_static_obstacles_added)
    {
        dj_debug_printf("Static obstacles not added\n");
        return;
    }
    // Clear the previous dynamic obstacles
    // (Dynamic obstacles are after the static obstacles in the list)
    if (manager->m_nb_static_obstacles != 0)
    {
        static_obstacles_list_reset(&manager->m_computed_obstacles, manager->m_nb_static_obstacles - 1);
    }
    else
    {
        static_obstacles_list_clear(&manager->m_computed_obstacles);
    }

    for (dj_dynamic_obstacle_id_e obstacle_id = 0; obstacle_id < DYNAMIC_OBSTACLE_COUNT; obstacle_id++)
    {
        dj_obstacle_dynamic_t *new_obstacle_to_add = dj_obstacle_importer_get_dynamic_obstacle(obstacle_id);
        if (new_obstacle_to_add != NULL)
        {
            // Compute the solutions (see dj_obstacle_dynamic_get_position)
            dj_dynamic_polygon_solution_t solutions;
            dj_obstacle_dynamic_get_position(new_obstacle_to_add, viewer, &solutions);
            // Convert the solutions to static obstacles and add them to the manager
            for (uint32_t i = 0; i < solutions.m_nb_solutions; i++)
            {
                dj_obstacle_static_t *new_obstacle_added = static_obstacles_list_add(&manager->m_computed_obstacles, NULL);
                dj_control_non_null(new_obstacle_added, );
                dj_obstacle_static_init(new_obstacle_added,
                                        &solutions.m_solutions[i],
                                        STATIC_OBSTACLE_UNKNOWN_ID,
                                        new_obstacle_to_add->m_is_enabled);
            }
        }
    }
    // The last viewer is valid
    manager->m_last_viewer_is_valid = true;
    // The obstacles are computed, so the manager does not need to recompute them if the viewer does not change
    manager->m_must_recompute = false;
    // Save the last viewer
    manager->m_last_computed_viewer = *viewer;
}

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_manager_init(dj_obstacle_manager_t *manager)
{
    dj_control_non_null(manager, );

    static_obstacles_list_init(&manager->m_computed_obstacles);
    manager->m_last_viewer_is_valid = false;
    manager->m_must_recompute = true;
    manager->m_static_obstacles_added = false;
    manager->m_nb_static_obstacles = 0;
}

void dj_obstacle_manager_deinit(dj_obstacle_manager_t *manager)
{
    dj_control_non_null(manager, );
    // Deinitialize all the obstacles
    for (uint32_t i = 0; i < static_obstacles_list_size(&manager->m_computed_obstacles); i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(&manager->m_computed_obstacles, i);
        dj_control_non_null(obstacle, );
        dj_obstacle_static_deinit(obstacle);
    }
}

void dj_obstacle_manager_clear_obstacles(dj_obstacle_manager_t *manager)
{
    dj_control_non_null(manager, );

    static_obstacles_list_clear(&manager->m_computed_obstacles);
    manager->m_static_obstacles_added = false;
    manager->m_must_recompute = true;
}

static_obstacles_list_t *dj_obstacle_manager_get_all_obstacles(dj_obstacle_manager_t *manager,
                                                               dj_viewer_status_t *viewer_status)
{
    dj_control_non_null(manager, NULL);

    dj_mark_start_time(DJ_MARK_OBSTACLE_MANAGER_GET_ALL_OBSTACLES);

    // Add all static obstacles if not already added
    if (!manager->m_static_obstacles_added)
    {
        // Add all static obstacles
        manager->m_nb_static_obstacles = 0;
        for (dj_static_obstacle_id_e obstacle_id = 0; obstacle_id < dj_obstacle_id_get_static_obstacle_count();
             obstacle_id++)
        {
            dj_obstacle_static_t *new_obstacle_to_add = dj_obstacle_importer_get_static_obstacle(obstacle_id);
            dj_control_non_null(new_obstacle_to_add, NULL);
            if (new_obstacle_to_add->m_is_enabled)
            {
                dj_obstacle_static_t *new_obstacle_added = static_obstacles_list_add(&manager->m_computed_obstacles, NULL);
                dj_control_non_null(new_obstacle_added, NULL);
                dj_obstacle_static_init(new_obstacle_added,
                                        &new_obstacle_to_add->m_shape,
                                        new_obstacle_to_add->m_id,
                                        new_obstacle_to_add->m_is_enabled);
                manager->m_nb_static_obstacles++;
            }
            else
            {
                dj_debug_printf("Obstacle %d not found, maybe not enabled\n", obstacle_id);
            }
        }
        manager->m_static_obstacles_added = true;
    }
    if (viewer_status != NULL)
    {
        if (manager->m_must_recompute || viewer_status->m_time != manager->m_last_computed_viewer.m_time
            || viewer_status->m_position.x != manager->m_last_computed_viewer.m_position.x
            || viewer_status->m_position.y != manager->m_last_computed_viewer.m_position.y
            || viewer_status->m_speed.x != manager->m_last_computed_viewer.m_speed.x
            || viewer_status->m_speed.y != manager->m_last_computed_viewer.m_speed.y || !manager->m_last_viewer_is_valid)
        {
            compute_dynamic_obstacles(manager, viewer_status);
        }
    }

    dj_mark_end_time(DJ_MARK_OBSTACLE_MANAGER_GET_ALL_OBSTACLES);

    return &manager->m_computed_obstacles;
}

bool dj_obstacle_manager_is_point_on_obstacle(static_obstacles_list_t *obstacles, GEOMETRY_point_t point)
{
    for (uint32_t i = 0; i < static_obstacles_list_size(obstacles); i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        dj_control_non_null(obstacle, false);
        if (obstacle->m_is_enabled && obstacle->m_id != STATIC_OBSTACLE_UNKNOWN_ID
            && GEOMETRY_is_in_polygon(
                obstacle->m_shape.points, obstacle->m_shape.nb_points, point, (GEOMETRY_point_t){-10000, -10000}, NULL))
        {
            return true;
        }
    }
    return false;
}

/* ******************************************* Public callback functions declarations ************************************ */
