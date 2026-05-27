/**
 * @file dj_engine.h
 * @brief DJ engine is the handler for the DJ propulsion system.
 * @author Cyprien Ménard
 * @date 29/01/2026
 * @see dj_engine.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "propulsion/prop_types/prop_end_condition.h"
#include "utils/dj/dj_goal_point.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "utils/dj/dj_launcher/dj_launcher.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_importer.h"
#include "utils/dj/dj_solver/dj_solver_common/dj_solver_common.h"
#include "utils/maths/kinematics/angular_acceleration/angular_acceleration.h"
#include "utils/maths/kinematics/angular_speed/angular_speed.h"
#include "utils/maths/kinematics/linear_acceleration/linear_acceleration.h"
#include "utils/maths/kinematics/linear_speed/linear_speed.h"
#include "utils/maths/position/position.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Define all the propulsion properties needed to calculate path duration
 */
typedef struct
{
    linear_speed_t max_linear_speed;             /**< Maximum linear speed [mm/s] */
    linear_acceleration_t linear_acceleration;   /**< Maximum linear acceleration [mm/s²] */
    linear_acceleration_t linear_deceleration;   /**< Maximum linear deceleration [mm/s²] */
    angular_speed_t max_angular_speed;           /**< Maximum angular speed [rad/s] */
    angular_acceleration_t angular_acceleration; /**< Maximum angular acceleration [rad/s²] */
    angular_acceleration_t angular_deceleration; /**< Maximum angular deceleration [rad/s²] */
    linear_speed_t start_speed;                  /**< Current robot linear speed [mm/s] */
    prop_end_condition_t end_condition;          /**< End condition between path points */
    position_t start_position;                   /**< Current robot position */
} dj_engine_propulsion_properties_t;

/**
 * @brief Arguments for the external calculators
 */
typedef struct
{
    void *path_duration_calculator_args; /**< Arguments for the duration calculator callback */
    void
        *dynamic_obstacle_position_calculator_args; /**< Arguments for the dynamic obstacle position
                                                    calculator callback */
} dj_engine_calculator_args_t;

/**
 * @brief Callback function to generate the dj_engine_calculator_args_t before path generation
 * @param args_out Pointer to the generated arguments structure
 * @param args_in Pointer to a dj_engine_propulsion_properties_t structure to help generating the arguments
 */
typedef void (*dj_engine_calculator_args_generator_cb_t)(
    dj_engine_calculator_args_t *args_out, dj_engine_propulsion_properties_t *args_in);

/**
 * @brief Structure representing the DJ engine.
 */
typedef struct
{
    /**
     * @brief Obstacle importer
     */
    dj_obstacle_importer_t obstacle_importer;

    /**
     * @brief Prebuilt graph builder for pathfinding
     * @details Prebuilt graph is a graph that is built once and is used to build
     * the graph faster during a dj computation.
     */
    dj_graph_builder_t prebuilt_graph_builder;

    /**
     * @brief DJ launcher instance to use for path generation
     */
    dj_launcher_t dj_launcher;

    /**
     * @brief Robot radius in millimeters.
     */
    distance_t robot_radius;

    /**
     * @brief Whether the prebuilt graph must be calculated.
     */
    bool prebuilt_graph_must_be_calculated;

    /**
     * @brief Callback function to calculate the duration of the path
     */
    dj_graph_path_get_duration_callback_t duration_calculator;

    /**
     * @brief Callback to get the position of dynamic obstacles
     */
    dj_obstacle_dynamic_get_position_cb_t get_dynamic_obstacle_position;

    /**
     * @brief Callback to call to generate the calculator arguments before path generation
     */
    dj_engine_calculator_args_generator_cb_t calculator_args_generator;

    /**
     * @brief polygon to limit the workspace for graph building.
     */
    const polygon_t *restricted_workspace_polygon;
} dj_engine_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize the DJ engine.
 * @param engine Pointer to the DJ engine to initialize.
 * @param robot_radius Radius of the robot in millimeters.
 * @param restricted_workspace_polygon Polygon to limit the workspace for graph building.
 * @param duration_calculator Callback function to calculate the duration of the path.
 * @param get_dynamic_obstacle_position Callback to get the position of dynamic obstacles.
 * @param get_dynamic_obstacle_position_args Additional arguments for the get position callback
 * @param duration_calculator_args Additional arguments for the duration callback function.
 */
void dj_engine_init(dj_engine_t *engine,
                    distance_t robot_radius,
                    const polygon_t *restricted_workspace_polygon,
                    dj_graph_path_get_duration_callback_t duration_calculator,
                    dj_obstacle_dynamic_get_position_cb_t get_dynamic_obstacle_position,
                    dj_engine_calculator_args_generator_cb_t get_dynamic_obstacle_position_args);

/**
 * @brief Generate paths to one or multiple destinations.
 * @details For multi-destination, this is more efficient than calling this function multiple times
 * because Dijkstra/A* naturally explores outward from the source, so a single run
 * can find shortest paths to all reachable destinations without redundant computation.
 * @param engine Pointer to the DJ engine.
 * @param properties Propulsion properties (start position, speed, etc.).
 * @param goal_points Array of destination points.
 * @param goal_count Number of destinations (max DJ_ENGINE_MAX_DESTINATIONS).
 * @param paths_out Array of computed paths (one per destination). Must be pre-allocated with goal_count entries.
 * @param successes_out Array of booleans indicating if each destination was reached.
 * @param solver_type The type of solver to use.
 * @param retry_without_dynamic_obstacle Whether to retry without dynamic obstacles for failed destinations.
 * @return Number of destinations successfully reached.
 */
uint32_t dj_engine_generate_path(dj_engine_t *engine,
                                 const dj_engine_propulsion_properties_t *properties,
                                 const dj_goal_point_t *goal_points,
                                 uint32_t goal_count,
                                 dj_graph_path_t *paths_out,
                                 bool *successes_out,
                                 dj_solver_type_t solver_type,
                                 bool retry_without_dynamic_obstacle);

/**
 * @brief Force recalculation of the prebuilt graph.
 * @param engine Pointer to the DJ engine.
 * @note A prebuilt graph must be recalculated when obstacles are added or removed.
 * The prebuilt graph will be recalculated automatically when generating a path if needed but it
 * can be forced using this function because recalculation can be time consuming.
 */
void dj_engine_force_recalculate_prebuilt_graph(dj_engine_t *engine);

/**
 * @brief Import a static obstacle into the DJ engine.
 * @warning Memory will be allocated and an unique ID will reserved so do not import too much obstacles
 * @param engine Pointer to the DJ engine.
 * @param shape Shape of the obstacle.
 * @param is_enabled Whether the obstacle is enabled.
 * @param mode Oversize mode.
 * @param margin Margin to apply to the obstacle in the DJ engine, in millimeters.
 * @param smooth_extraction_radius Radius for smooth extraction (mm).
 * @return ID of the imported static obstacle.
 */
dj_static_obstacle_id_t dj_engine_import_static_obstacle(dj_engine_t *engine,
                                                         dj_polygon_t *shape,
                                                         bool is_enabled,
                                                         dj_obstacle_oversize_mode_t mode,
                                                         distance_t margin,
                                                         distance_t smooth_extraction_radius);

/**
 * @brief Import a dynamic obstacle into the DJ engine.
 * @warning Memory will be allocated and an unique ID will reserved so do not import too much obstacles
 * @note The shape must be centralized as best as possible on the origin
 * @note After the import, the obstacle will not have any position, speed or acceleration, so you must refresh it
 * @see dj_obstacle_dynamic_refresh
 * @param engine Pointer to the DJ engine.
 * @param initial_shape Shape of the obstacle.
 * @param mode Oversize mode.
 * @param margin Margin to apply to the obstacle in the DJ engine, in millimeters.
 * @param is_enabled Whether the obstacle is enabled.
 * @return ID of the imported dynamic obstacle.
 */
dj_dynamic_obstacle_id_t dj_engine_import_dynamic_obstacle(dj_engine_t *engine,
                                                           dj_polygon_t *initial_shape,
                                                           dj_obstacle_oversize_mode_t mode,
                                                           distance_t margin,
                                                           bool is_enabled);

/**
 * @brief Refresh the position of a dynamic obstacle in the DJ engine.
 * @param engine Pointer to the DJ engine.
 * @param shape New shape of the dynamic obstacle.
 * @param obstacle_id ID of the dynamic obstacle to refresh.
 * @param acceleration Acceleration of the obstacle (mm/s^2) (on x and y axis).
 * @param initial_speed Initial speed of the obstacle (mm/s) (on x and y axis).
 * @param initial_position Initial position of the obstacle (mm) (on x and y axis).
 * @param mode Oversize mode.
 * @param margin Margin to apply to the obstacle in the DJ engine, in millimeters.
 * @note acceleration and initial_speed should be colinear.
 */
void dj_engine_refresh_dynamic_obstacle(dj_engine_t *engine,
                                        dj_polygon_t *shape,
                                        dj_dynamic_obstacle_id_t obstacle_id,
                                        linear_acceleration_2d_vector_t *acceleration,
                                        linear_speed_2d_vector_t *initial_speed,
                                        point_t *initial_position,
                                        dj_obstacle_oversize_mode_t mode,
                                        distance_t margin);

/**
 * @brief Enable or disable a static obstacle in the DJ engine.
 * @param engine Pointer to the DJ engine.
 * @param obstacle_id ID of the static obstacle to enable or disable.
 * @param enable true to enable the obstacle, false to disable it.
 */
void dj_engine_enable_static_obstacle(dj_engine_t *engine,
                                      dj_static_obstacle_id_t obstacle_id,
                                      bool enable);

/**
 * @brief Enable or disable a dynamic obstacle in the DJ engine.
 * @param engine Pointer to the DJ engine.
 * @param obstacle_id ID of the dynamic obstacle to enable or disable.
 * @param enable true to enable the obstacle, false to disable it.
 */
void dj_engine_enable_dynamic_obstacle(dj_engine_t *engine,
                                       dj_dynamic_obstacle_id_t obstacle_id,
                                       bool enable);

/**
 * @brief Check if a static obstacle is enabled in the DJ engine.
 * @param engine Pointer to the DJ engine.
 * @param obstacle_id ID of the static obstacle to check.
 * @return true if the obstacle is enabled, false otherwise.
 */
bool dj_engine_is_static_obstacle_enabled(dj_engine_t *engine, dj_static_obstacle_id_t obstacle_id);

/**
 * @brief Check if a dynamic obstacle is enabled in the DJ engine.
 * @param engine Pointer to the DJ engine.
 * @param obstacle_id ID of the dynamic obstacle to check.
 * @return true if the obstacle is enabled, false otherwise.
 */
bool dj_engine_is_dynamic_obstacle_enabled(dj_engine_t *engine,
                                           dj_dynamic_obstacle_id_t obstacle_id);

/**
 * @brief Initializes a dj_engine_propulsion_properties_t structure
 * @param properties Pointer to the structure to initialize
 * @param max_linear_speed Maximum linear speed [mm/s]
 * @param linear_acceleration Linear acceleration [mm/s²]
 * @param linear_deceleration Linear deceleration [mm/s²]
 * @param max_angular_speed Maximum angular speed [rad/s]
 * @param angular_acceleration Angular acceleration [rad/s²]
 * @param angular_deceleration Angular deceleration [rad/s²]
 * @param start_speed Current robot linear speed [mm/s]
 * @param end_condition End condition between path points
 * @param smooth_extraction_radius Radius for smooth path extraction (mm)
 * Used to extract smoothly from the obstacles when target or start point is close
 * to the edge of an obstacle.
 * @param start_position Current robot position
 */
void dj_engine_init_propulsion_properties(dj_engine_propulsion_properties_t *properties,
                                          linear_speed_t max_linear_speed,
                                          linear_acceleration_t linear_acceleration,
                                          linear_acceleration_t linear_deceleration,
                                          angular_speed_t max_angular_speed,
                                          angular_acceleration_t angular_acceleration,
                                          angular_acceleration_t angular_deceleration,
                                          linear_speed_t start_speed,
                                          prop_end_condition_t end_condition,
                                          const position_t *start_position);

/* ******************************************* Public callback functions declarations ************************************ */
