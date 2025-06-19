/**
 * @file dj.h
 * @brief Dj is an algorithm that allows the robot to move from point A to point B while
 * avoiding static and dynamic obstacles
 * @note This file must be overriden by the user with the correct interface
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj.h"

#if DJ_ROBOT_ESEO_FRAMEWORK
#include "QS_adversary_tracker.h"
#include "dj_launcher/dj_try_going.h"
#endif
#include "dj_graph_builder/dj_prebuilt_graph.h"
#include "dj_obstacle_importer/dj_obstacle_importer.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/**
 * @brief Flag to know if dj is initialized
 */
static bool dj_initialized = false;

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

inline void dj_init()
{
    // Call all the initializations of dj
    dj_obstacle_importer_init();
    dj_obstacle_import_default_obstacles();
    dj_prebuilt_graph_init();
    dj_initialized = true;
}

inline void dj_init_at_match_launch()
{
    // Nothing to do
}

inline void dj_init_after_color_change()
{
    if (dj_initialized)
    {
        dj_obstacle_importer_deinit();
        dj_prebuilt_graph_deinit();
    }
    dj_init();
}

#if DJ_ROBOT_ESEO_FRAMEWORK

void dj_process_main()
{
    // Update the dynamic obstacles
    uint8_t closest_adversary_id = QS_adversary_tracker_get_closest_adversary_id();
    adversary_tracked_t *adversary = QS_adversary_tracker_get_adversary(closest_adversary_id);
    if (adversary != NULL)
    {
        // Enable the dynamic obstacle
        dj_obstacle_importer_enable_dynamic_obstacle(DYNAMIC_OBSTACLE_ADV_ROBOT, true);
        // Refresh the dynamic obstacle
        dj_obstacle_importer_refresh_dynamic_obstacle(DYNAMIC_OBSTACLE_ADV_ROBOT,
                                                      &(GEOMETRY_vector_t){0, 0}, //adversary->m_acceleration,
                                                      &(GEOMETRY_vector_t){0, 0}, //adversary->m_speed,
                                                      &adversary->m_pos);
    }
    else
    {
        // Disable the dynamic obstacle
        dj_obstacle_importer_enable_dynamic_obstacle(DYNAMIC_OBSTACLE_ADV_ROBOT, false);
    }
}

uint8_t dj_try_going_with_params(uint16_t x,
                                 uint16_t y,
                                 uint8_t in_progress,
                                 uint8_t success_state,
                                 uint8_t fail_state,
                                 PROP_speed_e speed,
                                 way_e way,
                                 avoidance_e avoidance,
                                 STRAT_endCondition_e end_condition,
                                 bool retry_without_dynamic_obstacle)
{
    return dj_priv_try_going(
        x, y, in_progress, success_state, fail_state, speed, way, avoidance, end_condition, retry_without_dynamic_obstacle);
}

uint8_t dj_try_going(uint16_t x,
                     uint16_t y,
                     uint8_t in_progress,
                     uint8_t success_state,
                     uint8_t fail_state,
                     PROP_speed_e speed,
                     way_e way,
                     avoidance_e avoidance,
                     STRAT_endCondition_e end_condition)
{
    return dj_try_going_with_params(
        x, y, in_progress, success_state, fail_state, speed, way, avoidance, end_condition, true);
}

#endif // DJ_ROBOT_ESEO_FRAMEWORK

void dj_enable_static_polygon(dj_static_obstacle_id_e polygon_id, bool enable)
{
    // Check if the polygon is already in this state
    if (dj_obstacle_importer_static_is_enabled(polygon_id) == enable)
    {
        return;
    }
    // Enable or disable the polygon
    dj_obstacle_importer_enable_static_obstacle(polygon_id, enable);
    // Update the prebuilt graph with the obstacle (or without it)
    dj_obstacle_static_t *obstacle = dj_obstacle_importer_get_static_obstacle(polygon_id);
    update_prebuilt_graph_on_obstacle_enabling(obstacle, enable);
}

void dj_enable_dynamic_polygon(dj_dynamic_obstacle_id_e polygon_id, bool enable)
{
    // Check if the polygon is already in this state
    if (dj_obstacle_importer_dynamic_is_enabled(polygon_id) == enable)
    {
        return;
    }
    dj_obstacle_importer_enable_dynamic_obstacle(polygon_id, enable);
}

bool dj_is_static_polygon_enabled(dj_static_obstacle_id_e polygon_id)
{
    return dj_obstacle_importer_static_is_enabled(polygon_id);
}

bool dj_is_dynamic_polygon_enabled(dj_dynamic_obstacle_id_e polygon_id)
{
    return dj_obstacle_importer_dynamic_is_enabled(polygon_id);
}

/* ******************************************* Public callback functions declarations ************************************ */
