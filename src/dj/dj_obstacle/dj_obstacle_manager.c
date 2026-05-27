/**
 * @file dj_obstacle_manager.h
 * @brief Manager for all obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_manager.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle_manager.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_id.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_importer.h"
#include <stdlib.h>

LOG_REGISTER("utils/dj/obstacle_manager");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static void compute_dynamic_obstacles(dj_obstacle_manager_t *manager, dj_viewer_status_t *viewer);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Compute the dynamic obstacles to get the static obstacles in space time (so from the point of view of a viewer)
 * @see dj_obstacle_dynamic_get_position
 *
 * @param manager The obstacle manager object
 * @param viewer The viewer status (NULL if you want only static obstacles)
 */
static void compute_dynamic_obstacles(dj_obstacle_manager_t *manager, dj_viewer_status_t *viewer)
{
    SYSTEM_ASSERT(manager != NULL);
    // Check if the same as the last computed viewer or if the viewer is NULL
    // Viewer NULL --> no viewer, so no dynamic obstacles
    if (viewer == NULL)
    {
        return;
    }
    if (viewer->time == manager->last_computed_viewer.time &&
        viewer->position.x == manager->last_computed_viewer.position.x &&
        viewer->position.y == manager->last_computed_viewer.position.y &&
        viewer->speed.x == manager->last_computed_viewer.speed.x &&
        viewer->speed.y == manager->last_computed_viewer.speed.y && manager->last_viewer_is_valid)
    {
        return;
    }
    // Check if the static obstacles are added
    if (!manager->static_obstacles_added)
    {
        LOGD("Static obstacles not added");
        return;
    }
    // Clear the previous dynamic obstacles
    // (Dynamic obstacles are after the static obstacles in the list)
    if (manager->nb_static_obstacles != 0)
    {
        static_obstacles_list_reset(&manager->computed_obstacles, manager->nb_static_obstacles - 1);
    }
    else
    {
        static_obstacles_list_clear(&manager->computed_obstacles);
    }

    for (dj_dynamic_obstacle_id_t obstacle_id = 0;
         obstacle_id <
         dj_obstacle_id_get_dynamic_obstacle_count(&manager->obstacle_importer->id_generator);
         obstacle_id++)
    {
        dj_obstacle_dynamic_t *new_obstacle_to_add =
            dj_obstacle_importer_get_dynamic_obstacle(manager->obstacle_importer, obstacle_id);
        if (new_obstacle_to_add != NULL)
        {
            // Compute the solutions (see dj_obstacle_dynamic_get_position)
            dj_dynamic_polygon_solution_t solutions;
            dj_obstacle_dynamic_get_position(new_obstacle_to_add, viewer, &solutions);
            // Convert the solutions to static obstacles and add them to the manager
            for (uint32_t i = 0; i < solutions.nb_solutions; i++)
            {
                dj_obstacle_static_t *new_obstacle_added =
                    static_obstacles_list_add(&manager->computed_obstacles, NULL);
                SYSTEM_ASSERT(new_obstacle_added != NULL);
                dj_obstacle_static_init(new_obstacle_added,
                                        &solutions.solutions[i],
                                        STATIC_OBSTACLE_UNKNOWN_ID,
                                        0.f, // Dynamic obstacles are not smooth extracted
                                        new_obstacle_to_add->is_enabled);
            }
        }
    }
    // The last viewer is valid
    manager->last_viewer_is_valid = true;
    // The obstacles are computed, so the manager does not need to recompute them if the viewer does not change
    manager->must_recompute = false;
    // Save the last viewer
    manager->last_computed_viewer = *viewer;
}

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_manager_init(dj_obstacle_manager_t *manager,
                              dj_obstacle_importer_t *obstacle_importer)
{
    SYSTEM_ASSERT(manager != NULL);
    SYSTEM_ASSERT(obstacle_importer != NULL);

    static_obstacles_list_init(&manager->computed_obstacles);
    manager->last_viewer_is_valid = false;
    manager->must_recompute = true;
    manager->static_obstacles_added = false;
    manager->nb_static_obstacles = 0;
    manager->all_dynamic_null_kinematics = false;
    manager->obstacle_importer = obstacle_importer;
}

void dj_obstacle_manager_clear_obstacles(dj_obstacle_manager_t *manager)
{
    SYSTEM_ASSERT(manager != NULL);

    static_obstacles_list_clear(&manager->computed_obstacles);
    manager->static_obstacles_added = false;
    manager->must_recompute = true;
}

static_obstacles_list_t *dj_obstacle_manager_get_all_obstacles(dj_obstacle_manager_t *manager,
                                                               dj_viewer_status_t *viewer_status)
{
    SYSTEM_ASSERT(manager != NULL);

    dj_mark_start_time(OBSTACLE_MANAGER_GET_ALL_OBSTACLES);

    // Add all static obstacles if not already added
    if (!manager->static_obstacles_added)
    {
        // Add all static obstacles
        manager->nb_static_obstacles = 0;
        for (dj_static_obstacle_id_t obstacle_id = 0;
             obstacle_id <
             dj_obstacle_id_get_static_obstacle_count(&manager->obstacle_importer->id_generator);
             obstacle_id++)
        {
            dj_obstacle_static_t *new_obstacle_to_add =
                dj_obstacle_importer_get_static_obstacle(manager->obstacle_importer, obstacle_id);
            if (new_obstacle_to_add == NULL)
            {
                continue;
            }
            if (new_obstacle_to_add->is_enabled)
            {
                dj_obstacle_static_t *new_obstacle_added =
                    static_obstacles_list_add(&manager->computed_obstacles, NULL);
                SYSTEM_ASSERT(new_obstacle_added != NULL);
                dj_obstacle_static_init(new_obstacle_added,
                                        &new_obstacle_to_add->shape,
                                        new_obstacle_to_add->id,
                                        new_obstacle_to_add->smooth_extraction_radius,
                                        new_obstacle_to_add->is_enabled);
                manager->nb_static_obstacles++;
            }
            else
            {
                LOGD("Obstacle %d not found, maybe not enabled", obstacle_id);
            }
        }
        manager->static_obstacles_added = true;
    }
    if (viewer_status != NULL)
    {
        // Fast path: if all dynamic obstacles have null kinematics their positions are
        // viewer-independent. Skip recomputation once the first valid result is cached.
        if (manager->all_dynamic_null_kinematics && manager->last_viewer_is_valid)
        {
            dj_mark_end_time(OBSTACLE_MANAGER_GET_ALL_OBSTACLES);
            return &manager->computed_obstacles;
        }

        if (manager->must_recompute || viewer_status->time != manager->last_computed_viewer.time ||
            viewer_status->position.x != manager->last_computed_viewer.position.x ||
            viewer_status->position.y != manager->last_computed_viewer.position.y ||
            viewer_status->speed.x != manager->last_computed_viewer.speed.x ||
            viewer_status->speed.y != manager->last_computed_viewer.speed.y ||
            !manager->last_viewer_is_valid)
        {
            dj_mark_start_time(DYNAMIC_OBSTACLE_COMPUTE);
            compute_dynamic_obstacles(manager, viewer_status);
            dj_mark_end_time(DYNAMIC_OBSTACLE_COMPUTE);
        }
    }

    dj_mark_end_time(OBSTACLE_MANAGER_GET_ALL_OBSTACLES);

    return &manager->computed_obstacles;
}

bool dj_obstacle_manager_is_point_on_obstacle(static_obstacles_list_t *obstacles, point_t point)
{
    SYSTEM_ASSERT(obstacles != NULL);
    for (uint32_t i = 0; i < static_obstacles_list_size(obstacles); i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        if (obstacle == NULL)
        {
            continue;
        }
        const point_t out_point = (point_t){-10000, -10000};
        if (obstacle->is_enabled && obstacle->id != STATIC_OBSTACLE_UNKNOWN_ID &&
            is_in_polygon(obstacle->shape.points,
                          (uint8_t)obstacle->shape.nb_points,
                          &point,
                          &out_point,
                          NULL))
        {
            return true;
        }
    }
    return false;
}

/* ******************************************* Public callback functions declarations ************************************ */
