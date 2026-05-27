/**
 * @file dj_launcher.c
 * @brief Launcher for the dj path generation
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_launcher.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "utils/dj/dj_launcher/dj_launcher.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "utils/dj/dj_graph_builder/dj_graph_rebuilder.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_importer.h"
#include "utils/dj/dj_solver/dj_solver.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include "utils/maths/kinematics/linear_speed/linear_speed.h"
#include "utils/maths/point/point.h"
#include "utils/time/time.h"
#include <math.h>
#include <stdbool.h>

LOG_REGISTER("utils/dj/launcher");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

uint32_t dj_generate_path_with_param(dj_launcher_t *launcher,
                                     dj_graph_path_t *paths_out,
                                     bool *successes_out,
                                     const position_t *start,
                                     linear_speed_t start_speed,
                                     const dj_goal_point_t *target_points,
                                     uint32_t target_count,
                                     bool enable_dynamic_obstacles,
                                     dj_obstacle_importer_t *obstacle_importer,
                                     dj_graph_graph_t *prebuilt_graph,
                                     dj_solver_type_t solver_type,
                                     const polygon_t *restricted_workspace_polygon,
                                     dj_graph_path_get_duration_callback_t duration_calculator,
                                     void *duration_calculator_args)
{
    SYSTEM_ASSERT(paths_out != NULL);
    SYSTEM_ASSERT(successes_out != NULL);
    SYSTEM_ASSERT(start != NULL);
    SYSTEM_ASSERT(target_points != NULL);
    SYSTEM_ASSERT(target_count > 0 && target_count <= DJ_ENGINE_MAX_DESTINATIONS);
    SYSTEM_ASSERT(obstacle_importer != NULL);
    SYSTEM_ASSERT(prebuilt_graph != NULL);

    // Extract plain point_t array for solver functions
    point_t target_point_positions[DJ_ENGINE_MAX_DESTINATIONS];
    for (uint32_t i = 0; i < target_count; i++)
    {
        target_point_positions[i] = target_points[i].point;
    }

    dj_mark_reset_all();
    vector_2d_polar_t initial_speed_polar = {
        .magnitude = fabsf(start_speed),
        .angle = start->angle,
    };
    vector_2d_distance_t initial_speed;
    polar_to_cartesian(&initial_speed_polar, &initial_speed);
    time_ms_t time = time_get_absolute_time_ms();

    dj_viewer_status_t viewer_status;
    viewer_status_init(&viewer_status, &start->point, &initial_speed, time);

    // Initialize all output paths
    for (uint32_t i = 0; i < target_count; i++)
    {
        dj_graph_path_init(&paths_out[i],
                           &viewer_status,
                           target_points[i].arrival_angle,
                           target_points[i].use_angle,
                           start->angle,
                           duration_calculator,
                           duration_calculator_args);
        successes_out[i] = false;
    }

    LOGD("Start of dj with %u destinations:", (unsigned)target_count);
    LOGD("Start point: (%f, %f)", start->point.x, start->point.y);
    for (uint32_t i = 0; i < target_count; i++)
    {
        LOGD("Target %u: (%f, %f)",
             (unsigned)i,
             target_point_positions[i].x,
             target_point_positions[i].y);
    }
    LOGD("Initial speed: (%f, %f)", initial_speed.x, initial_speed.y);
    LOGD("Time: %f", time);

    dj_mark_start_time(ALL);

    // Disable the dynamic obstacles if needed
    dj_obstacle_importer_enable_dynamic_obstacles(obstacle_importer, enable_dynamic_obstacles);

    // Create the graph builder
    dj_graph_builder_t *builder = &launcher->dj_launcher_builder;
#ifdef DJ_GRAPH_DISPLAY_ENABLED
    const bool display = true;
#else
    const bool display = false;
#endif
    dj_graph_builder_init(builder, obstacle_importer, restricted_workspace_polygon, display);

    // First rebuild the graph with all end nodes
    dj_graph_builder_first_rebuild(
        builder, prebuilt_graph, &start->point, target_point_positions, target_count);

    // Create the solver
    dj_solver_t *solver = &launcher->dj_launcher_solver;
    dj_solver_init(solver,
                   builder,
                   prebuilt_graph,
                   solver_type,
                   duration_calculator,
                   duration_calculator_args);

    // Solve the graph for all destinations
    dj_solver_solve(solver,
                    &viewer_status,
                    start->angle,
                    target_points,
                    target_count,
                    paths_out,
                    successes_out);

    // Validate and log results
    uint32_t success_count = 0;
    for (uint32_t i = 0; i < target_count; i++)
    {
        if (successes_out[i])
        {
            // Verify the path actually ends at the target
            point_t end_point;
            dj_graph_path_get_end(&paths_out[i], &end_point);
            if (end_point.x == target_point_positions[i].x &&
                end_point.y == target_point_positions[i].y)
            {
                paths_out[i].must_be_recomputed = true;
                LOGD("Path to destination %u found with %d points, duration: %d ms",
                     (unsigned)i,
                     (int)dj_graph_path_get_point_count(&paths_out[i]),
                     (int)dj_graph_path_get_duration(&paths_out[i]));
                success_count++;
            }
            else
            {
                successes_out[i] = false;
                LOGD("DJ : Path to destination %u invalid (end mismatch)", (unsigned)i);
            }
        }
        else
        {
            LOGD("DJ : No path found to destination %u", (unsigned)i);
        }
    }

    dj_mark_end_time(ALL);
    LOGD("DJ: %u/%u destinations reached", (unsigned)success_count, (unsigned)target_count);

    return success_count;
}

uint32_t dj_generate_path(dj_launcher_t *launcher,
                          dj_graph_path_t *paths_out,
                          bool *successes_out,
                          const position_t *start,
                          linear_speed_t start_speed,
                          const dj_goal_point_t *target_points,
                          uint32_t target_count,
                          bool retry_without_dynamic_obstacle,
                          dj_obstacle_importer_t *obstacle_importer,
                          dj_graph_graph_t *prebuilt_graph,
                          dj_solver_type_t solver_type,
                          const polygon_t *restricted_workspace_polygon,
                          dj_graph_path_get_duration_callback_t duration_calculator,
                          void *duration_calculator_args)
{
    SYSTEM_ASSERT(paths_out != NULL);
    SYSTEM_ASSERT(successes_out != NULL);
    SYSTEM_ASSERT(start != NULL);
    SYSTEM_ASSERT(target_points != NULL);
    SYSTEM_ASSERT(obstacle_importer != NULL);
    SYSTEM_ASSERT(prebuilt_graph != NULL);

    uint32_t success_count = dj_generate_path_with_param(launcher,
                                                         paths_out,
                                                         successes_out,
                                                         start,
                                                         start_speed,
                                                         target_points,
                                                         target_count,
                                                         true,
                                                         obstacle_importer,
                                                         prebuilt_graph,
                                                         solver_type,
                                                         restricted_workspace_polygon,
                                                         duration_calculator,
                                                         duration_calculator_args);

    if (success_count == 0 && retry_without_dynamic_obstacle)
    {
        LOGW("DJ: %u/%u paths not found, retrying without dynamic obstacles",
             (unsigned)(target_count - success_count),
             (unsigned)target_count);

        // Build arrays for the failed destinations only
        dj_goal_point_t remaining_targets[DJ_ENGINE_MAX_DESTINATIONS];
        uint32_t remaining_indices[DJ_ENGINE_MAX_DESTINATIONS];
        uint32_t remaining_count = 0;
        for (uint32_t i = 0; i < target_count; i++)
        {
            if (!successes_out[i])
            {
                remaining_targets[remaining_count] = target_points[i];
                remaining_indices[remaining_count] = i;
                remaining_count++;
            }
        }

        // Retry without dynamic obstacles for the remaining destinations
        dj_graph_path_t *retry_paths = launcher->dj_launcher_retry_paths;
        bool retry_successes[DJ_ENGINE_MAX_DESTINATIONS];

        uint32_t retry_success_count = dj_generate_path_with_param(launcher,
                                                                   retry_paths,
                                                                   retry_successes,
                                                                   start,
                                                                   start_speed,
                                                                   remaining_targets,
                                                                   remaining_count,
                                                                   false,
                                                                   obstacle_importer,
                                                                   prebuilt_graph,
                                                                   solver_type,
                                                                   restricted_workspace_polygon,
                                                                   duration_calculator,
                                                                   duration_calculator_args);

        // Merge retry results back into the output arrays
        for (uint32_t i = 0; i < remaining_count; i++)
        {
            if (retry_successes[i])
            {
                uint32_t original_index = remaining_indices[i];
                dj_graph_path_copy(&paths_out[original_index], &retry_paths[i]);
                successes_out[original_index] = true;
            }
        }
        success_count += retry_success_count;
    }

    if (success_count < target_count)
    {
        LOGW("DJ: %u/%u destinations unreachable",
             (unsigned)(target_count - success_count),
             (unsigned)target_count);
    }

    return success_count;
}

/* ***************************************** Public callback functions definitions *************************************** */
