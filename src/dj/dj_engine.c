/**
 * @file dj_engine.c
 * @brief DJ engine is the handler for the DJ propulsion system.
 * @author Cyprien Ménard
 * @date 29/01/2026
 * @see dj_engine.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_engine.h"
#include "system/log/log.h"
#include "utils/dj/dj_graph/dj_graph_graph.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_link_ops.h"
#include "utils/dj/dj_launcher/dj_launcher.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/maths/distance/distance.h"

LOG_REGISTER("utils/dj/engine");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/**
 * @brief Function to update the prebuilt graph when a static obstacle is enabled or disabled
 * @param obstacle The obstacle to update
 * @param enable If true, the obstacle is enabled, else it is disabled
 * @note This function is used to update the prebuilt graph when a static obstacle is enabled or disabled.
 * This is more efficient than forcing a full recalculation of the prebuilt graph.
 */
static void update_prebuilt_graph_on_obstacle_enabling(dj_engine_t *engine,
                                                       dj_obstacle_static_t *obstacle,
                                                       bool enable);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

static void update_prebuilt_graph_on_obstacle_enabling(dj_engine_t *engine,
                                                       dj_obstacle_static_t *obstacle,
                                                       bool enable)
{
    SYSTEM_ASSERT(obstacle != NULL);

    dj_mark_start_time(PREBUILT_GRAPH_REBUILD);

    // Force all the obstacles to be imported again in the obstacle manager
    dj_obstacle_manager_clear_obstacles(&engine->prebuilt_graph_builder.obstacle_manager);

    // Rebuild the graph with the obstacle
    if (enable)
    {
        // Rebuild the graph with the obstacle
        dj_graph_builder_build_with_obstacle(
            &engine->prebuilt_graph_builder, obstacle, NULL, true, true);
    }
    else
    {
        // Completely rebuild the graph without the obstacle to re-enable all links
        // Recalculate the prebuilt graph
        engine->prebuilt_graph_must_be_calculated = true;
    }

    dj_mark_end_time(PREBUILT_GRAPH_REBUILD);
}

/* ********************************************** Public functions definitions ******************************************* */

void dj_engine_init(dj_engine_t *engine,
                    distance_t robot_radius,
                    const polygon_t *restricted_workspace_polygon,
                    dj_graph_path_get_duration_callback_t duration_calculator,
                    dj_obstacle_dynamic_get_position_cb_t get_dynamic_obstacle_position,
                    dj_engine_calculator_args_generator_cb_t get_dynamic_obstacle_position_args)
{
    SYSTEM_ASSERT(engine != NULL);
    SYSTEM_ASSERT(duration_calculator != NULL);
    SYSTEM_ASSERT(get_dynamic_obstacle_position != NULL);
    SYSTEM_ASSERT(get_dynamic_obstacle_position_args != NULL);

    engine->robot_radius = robot_radius;
    engine->prebuilt_graph_must_be_calculated = true;
    engine->restricted_workspace_polygon = restricted_workspace_polygon;
    engine->duration_calculator = duration_calculator;
    engine->get_dynamic_obstacle_position = get_dynamic_obstacle_position;
    engine->calculator_args_generator = get_dynamic_obstacle_position_args;

    // Initialize the obstacle importer
    dj_obstacle_importer_init(&engine->obstacle_importer);
}

uint32_t dj_engine_generate_path(dj_engine_t *engine,
                                 const dj_engine_propulsion_properties_t *properties,
                                 const dj_goal_point_t *goal_points,
                                 uint32_t goal_count,
                                 dj_graph_path_t *paths_out,
                                 bool *successes_out,
                                 dj_solver_type_t solver_type,
                                 bool retry_without_dynamic_obstacle)
{
    SYSTEM_ASSERT(engine != NULL);
    SYSTEM_ASSERT(properties != NULL);
    SYSTEM_ASSERT(goal_points != NULL);
    SYSTEM_ASSERT(goal_count > 0 && goal_count <= DJ_ENGINE_MAX_DESTINATIONS);
    SYSTEM_ASSERT(paths_out != NULL);
    SYSTEM_ASSERT(successes_out != NULL);

    dj_mark_session_reset();
    dj_mark_start_time(GENERATE_PATH);

    // Check if the prebuilt graph must be recalculated
    if (engine->prebuilt_graph_must_be_calculated)
    {
#ifdef DJ_PREBUILT_GRAPH_DISPLAY_ENABLED
        const bool display = true;
#else
        const bool display = false;
#endif
        dj_graph_builder_init(&engine->prebuilt_graph_builder,
                              &engine->obstacle_importer,
                              engine->restricted_workspace_polygon,
                              display);
        dj_graph_builder_build_all(&engine->prebuilt_graph_builder, NULL, true);
        engine->prebuilt_graph_must_be_calculated = false;
    }

    // Call the calculator args generator to prepare the arguments
    // Use a temporary copy with goal_point set for the callback (backward compat)
    dj_engine_calculator_args_t calculator_args;
    engine->calculator_args_generator(&calculator_args,
                                      (dj_engine_propulsion_properties_t *)properties);

    // Set the dynamic obstacle position calculator args in the obstacle importer
    dj_obstacle_importer_set_dynamic_obstacle_position_args(
        &engine->obstacle_importer, calculator_args.dynamic_obstacle_position_calculator_args);

    // Generate paths to all destinations
    uint32_t success_count = dj_generate_path(&engine->dj_launcher,
                                              paths_out,
                                              successes_out,
                                              &properties->start_position,
                                              properties->start_speed,
                                              goal_points,
                                              goal_count,
                                              retry_without_dynamic_obstacle,
                                              &engine->obstacle_importer,
                                              &engine->prebuilt_graph_builder.graph,
                                              solver_type,
                                              engine->restricted_workspace_polygon,
                                              engine->duration_calculator,
                                              calculator_args.path_duration_calculator_args);

    dj_mark_end_time(GENERATE_PATH);
    dj_print_session_durations("generate_path");

    return success_count;
}

void dj_engine_force_recalculate_prebuilt_graph(dj_engine_t *engine)
{
    SYSTEM_ASSERT(engine != NULL);

    // Recalculate the prebuilt graph
#ifdef DJ_PREBUILT_GRAPH_DISPLAY_ENABLED
    const bool display = true;
#else
    const bool display = false;
#endif

    dj_mark_session_reset();
    dj_mark_start_time(PREBUILT_GRAPH_REBUILD);

    dj_graph_builder_init(&engine->prebuilt_graph_builder,
                          &engine->obstacle_importer,
                          engine->restricted_workspace_polygon,
                          display);
    dj_graph_builder_build_all(&engine->prebuilt_graph_builder, NULL, true);

    dj_mark_end_time(PREBUILT_GRAPH_REBUILD);
    dj_print_session_durations("force_recalculate_prebuilt_graph");

    // The prebuilt graph is now recalculated
    engine->prebuilt_graph_must_be_calculated = false;
}

dj_static_obstacle_id_t dj_engine_import_static_obstacle(dj_engine_t *engine,
                                                         dj_polygon_t *shape,
                                                         bool is_enabled,
                                                         dj_obstacle_oversize_mode_t mode,
                                                         distance_t margin,
                                                         distance_t smooth_extraction_radius)
{
    SYSTEM_ASSERT(engine != NULL);
    SYSTEM_ASSERT(shape != NULL);

    distance_t oversize_distance = engine->robot_radius + margin;

    dj_static_obstacle_id_t obstacle_id =
        dj_obstacle_importer_import_static_obstacle(&engine->obstacle_importer,
                                                    shape,
                                                    is_enabled,
                                                    mode,
                                                    oversize_distance,
                                                    smooth_extraction_radius);

    // The prebuilt graph must be recalculated
    engine->prebuilt_graph_must_be_calculated = true;

    return obstacle_id;
}

dj_dynamic_obstacle_id_t dj_engine_import_dynamic_obstacle(dj_engine_t *engine,
                                                           dj_polygon_t *initial_shape,
                                                           dj_obstacle_oversize_mode_t mode,
                                                           distance_t margin,
                                                           bool is_enabled)
{
    SYSTEM_ASSERT(engine != NULL);
    SYSTEM_ASSERT(initial_shape != NULL);

    distance_t oversize_distance = engine->robot_radius + margin;

    dj_dynamic_obstacle_id_t obstacle_id =
        dj_obstacle_importer_import_dynamic_obstacle(&engine->obstacle_importer,
                                                     initial_shape,
                                                     mode,
                                                     oversize_distance,
                                                     is_enabled,
                                                     engine->get_dynamic_obstacle_position);

    return obstacle_id;
}

void dj_engine_refresh_dynamic_obstacle(dj_engine_t *engine,
                                        dj_polygon_t *shape,
                                        dj_dynamic_obstacle_id_t obstacle_id,
                                        linear_acceleration_2d_vector_t *acceleration,
                                        linear_speed_2d_vector_t *initial_speed,
                                        point_t *initial_position,
                                        dj_obstacle_oversize_mode_t mode,
                                        distance_t margin)
{
    SYSTEM_ASSERT(engine != NULL);
    SYSTEM_ASSERT(acceleration != NULL);
    SYSTEM_ASSERT(initial_speed != NULL);
    SYSTEM_ASSERT(initial_position != NULL);

    distance_t oversize_distance = engine->robot_radius + margin;

    dj_obstacle_importer_refresh_dynamic_obstacle(&engine->obstacle_importer,
                                                  obstacle_id,
                                                  shape,
                                                  acceleration,
                                                  initial_speed,
                                                  initial_position,
                                                  oversize_distance,
                                                  mode);
}

void dj_engine_enable_static_obstacle(dj_engine_t *engine,
                                      dj_static_obstacle_id_t obstacle_id,
                                      bool enable)
{
    SYSTEM_ASSERT(engine != NULL);

    // Check if the obstacle is already in the desired state to avoid unnecessary
    // recalculations
    if (dj_obstacle_importer_static_is_enabled(&engine->obstacle_importer, obstacle_id) == enable)
    {
        LOGD("Static obstacle %d is already %s, skipping",
             obstacle_id,
             enable ? "enabled" : "disabled");
        return;
    }

    LOGD("Setting static obstacle %d to %s", obstacle_id, enable ? "enabled" : "disabled");

    dj_mark_session_reset();

    dj_obstacle_importer_enable_static_obstacle(&engine->obstacle_importer, obstacle_id, enable);

    // The prebuilt graph must be recalculated
    update_prebuilt_graph_on_obstacle_enabling(
        engine,
        dj_obstacle_importer_get_static_obstacle(&engine->obstacle_importer, obstacle_id),
        enable);

    dj_print_session_durations("enable_static_obstacle");
}

void dj_engine_enable_dynamic_obstacle(dj_engine_t *engine,
                                       dj_dynamic_obstacle_id_t obstacle_id,
                                       bool enable)
{
    SYSTEM_ASSERT(engine != NULL);

    LOGD("Setting dynamic obstacle %d to %s", obstacle_id, enable ? "enabled" : "disabled");

    dj_obstacle_importer_enable_dynamic_obstacle(&engine->obstacle_importer, obstacle_id, enable);
}

bool dj_engine_is_static_obstacle_enabled(dj_engine_t *engine, dj_static_obstacle_id_t obstacle_id)
{
    SYSTEM_ASSERT(engine != NULL);

    return dj_obstacle_importer_static_is_enabled(&engine->obstacle_importer, obstacle_id);
}

bool dj_engine_is_dynamic_obstacle_enabled(dj_engine_t *engine,
                                           dj_dynamic_obstacle_id_t obstacle_id)
{
    SYSTEM_ASSERT(engine != NULL);

    return dj_obstacle_importer_dynamic_is_enabled(&engine->obstacle_importer, obstacle_id);
}

void dj_engine_init_propulsion_properties(dj_engine_propulsion_properties_t *properties,
                                          linear_speed_t max_linear_speed,
                                          linear_acceleration_t linear_acceleration,
                                          linear_acceleration_t linear_deceleration,
                                          angular_speed_t max_angular_speed,
                                          angular_acceleration_t angular_acceleration,
                                          angular_acceleration_t angular_deceleration,
                                          linear_speed_t start_speed,
                                          prop_end_condition_t end_condition,
                                          const position_t *start_position)
{
    SYSTEM_ASSERT(properties != NULL);
    SYSTEM_ASSERT(start_position != NULL);
    properties->max_linear_speed = max_linear_speed;
    properties->linear_acceleration = linear_acceleration;
    properties->linear_deceleration = linear_deceleration;
    properties->max_angular_speed = max_angular_speed;
    properties->angular_acceleration = angular_acceleration;
    properties->angular_deceleration = angular_deceleration;
    properties->start_speed = start_speed;
    properties->end_condition = end_condition;
    properties->start_position = *start_position;
}

/* ***************************************** Public callback functions definitions *************************************** */
