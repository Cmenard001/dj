#include "dj_launcher.h"

#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_graph/dj_graph_path.h"
#include "../dj_graph_builder/dj_graph_builder.h"
#include "../dj_graph_builder/dj_prebuilt_graph.h"
#include "../dj_logs/dj_logs.h"
#include "../dj_logs/dj_time_marker.h"
#include "../dj_obstacle_importer/dj_obstacle_importer.h"
#include "../dj_solver/dj_solver.h"
#include "../dj_viewer/dj_viewer_status.h"

#include <stdbool.h>

bool dj_genarate_path_with_param(dj_graph_path_t *path, GEOMETRY_point_t target_point, bool enable_dynamic_obstacles)
{
    dj_control_non_null(path, false);

    dj_mark_reset_all();
    dj_mark_start_time(DJ_MARK_ALL);

    GEOMETRY_point_t start_point;
    dj_dep_get_robot_position(&start_point);
    GEOMETRY_vector_t initial_speed;
    dj_dep_get_robot_speed(&initial_speed);
    time32_t time;
    dj_dep_get_time(&time);

    dj_viewer_status_t viewer_status;
    viewer_status_init(&viewer_status, &start_point, &initial_speed, time);
    dj_graph_path_init(path, &viewer_status);

    dj_debug_printf("Start of dj with these parameters:\n");
    dj_debug_printf("Start point: (%d, %d)\n", start_point.x, start_point.y);
    dj_debug_printf("Target point: (%d, %d)\n", target_point.x, target_point.y);
    dj_debug_printf("Initial speed: (%d, %d)\n", initial_speed.x, initial_speed.y);
    dj_debug_printf("Time: %d\n", (int)time);

    // Disable the dynamic obstacles if needed
    dj_obstacle_importer_enable_dynamic_obstacles(enable_dynamic_obstacles);

    // Create the graph builder
    dj_graph_builder_t builder;
    dj_graph_builder_init(&builder);

    // Fisrt rebuild the graph with the prebuilt graph
    first_rebuild_graph_with_prebuilt_graph(&builder, start_point, target_point);

    // Create the solver
    dj_solver_t solver;
    dj_solver_init(&solver, &builder);

    // Solve the graph
    dj_solver_solve(&solver, &viewer_status);

    // Get the solution
    dj_solver_get_solution(&solver, path);

    dj_graph_builder_deinit(&builder);
    dj_solver_deinit(&solver);
    viewer_status_deinit(&viewer_status);

    // Check if the path is valid (if the last point is the target point)
    GEOMETRY_point_t end_point;
    dj_graph_path_get_end(path, &end_point);
    if (end_point.x != target_point.x || end_point.y != target_point.y)
    {
        dj_mark_end_time(DJ_MARK_ALL);
        dj_print_all_durations();
        dj_debug_printf("DJ : Impossible to find a path to the target point\n");
        return false; // We arn't arrived to the target point
    }

    // Print all the nodes
#if DJ_ENABLE_DEBUG_LOGS
    dj_debug_printf("Path found with %d points\n", (int)dj_graph_path_get_point_count(path));
    path->must_be_recomputed = true;
    dj_debug_printf("Path duration: %d\n", (int)dj_graph_path_get_duration(path));
    dj_debug_printf("Points:\n");
    for (uint32_t i = 0; i < dj_graph_path_get_point_count(path); i++)
    {
        GEOMETRY_point_t point;
        dj_graph_path_get_point(path, &point, i);
        dj_debug_printf("(%d, %d)\n", point.x, point.y);
    }
#endif

    dj_mark_end_time(DJ_MARK_ALL);

    dj_print_all_durations();
    return true;
}

bool dj_genarate_path(dj_graph_path_t *path, GEOMETRY_point_t target_point, bool retry_without_dynamic_obstacle)
{
    bool result = dj_genarate_path_with_param(path, target_point, true);
    if (!result && retry_without_dynamic_obstacle)
    {
        dj_info_printf("DJ : Path not found, trying without dynamic obstacles\n");
        result = dj_genarate_path_with_param(path, target_point, false);
    }
    if (!result)
    {
        dj_info_printf("DJ : Path not found without dynamic obstacles, error unknown\n");
    }
    return result;
}
