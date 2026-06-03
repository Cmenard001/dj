/**
 * @file dj_obstacle_importer.h
 * @brief Importer of dj obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obsacle_importer.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_obstacle_dynamic.h"
#include "utils/dj/dj_obstacle/dj_obstacle_static.h"
#include "utils/dj/dj_obstacle/dj_oversize_obstacle.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_id.h"
#include "utils/lists/magic_array.h"
#include "utils/maths/kinematics/linear_acceleration/linear_acceleration.h"
#include "utils/maths/kinematics/linear_speed/linear_speed.h"
#include "utils/shape/shape.h"
#include <stdbool.h>
/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum number of computed obstacles
 */
#define DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES  (50)

/**
 * @brief Maximum number of dynamic obstacles
 */
#define DJ_OBSTACLE_MANAGER_MAX_DYNAMIC_OBSTACLES (50)

/* ************************************************** Public types definition ******************************************** */
MAGIC_ARRAY_TYPEDEF(static_obstacles_list,
                    dj_obstacle_static_t,
                    DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES);

MAGIC_ARRAY_TYPEDEF(dynamic_obstacles_list,
                    dj_obstacle_dynamic_t,
                    DJ_OBSTACLE_MANAGER_MAX_DYNAMIC_OBSTACLES);

/**
 * @brief DJ obstacle importer structure
 */
typedef struct
{
    /**
     * @brief List of static obstacles
     */
    static_obstacles_list_t static_obstacles;

    /**
     * @brief List of dynamic obstacles
     */
    dynamic_obstacles_list_t dynamic_obstacles;

    /**
     * @brief Enable or disable the dynamic obstacles
     * @see dj_obstacle_importer_enable_dynamic_obstacles
     */
    bool dynamic_obstacles_enabled;

    /**
     * @brief Obstacle ID generator
     */
    dj_obstacle_id_generator_t id_generator;

    /**
     * @brief Shape handles for static obstacles
     */
    shape_handle_t static_obstacle_shapes[DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES];

    /**
     * @brief Whether the obstacles are currently displayed on the HMI or not
     */
    bool is_displaying_obstacles;
} dj_obstacle_importer_t;

/* *********************************************** Public functions declarations ***************************************** */

MAGIC_ARRAY_DECLARATION(static_obstacles_list, dj_obstacle_static_t);

/**
 * @brief Function to initialize the obstacle importer
 * @param importer Pointer to the obstacle importer structure
 */
void dj_obstacle_importer_init(dj_obstacle_importer_t *importer);

/**
 * @brief Function to import a static obstacle
 * @warning Memory will be allocated and an unique ID will reserved so do not import too much obstacles
 * @param importer Pointer to the obstacle importer structure
 * @param shape Shape of the obstacle
 * @param is_enabled true if the obstacle is enabled, false otherwise
 * @param mode Oversize mode
 * @param oversize_distance Distance to oversize the obstacle
 * @param smooth_extraction_radius Radius for smooth extraction (mm)
 * @return dj_static_obstacle_id_t ID of the obstacle
 */
dj_static_obstacle_id_t dj_obstacle_importer_import_static_obstacle(
    dj_obstacle_importer_t *importer,
    dj_polygon_t *shape,
    bool is_enabled,
    dj_obstacle_oversize_mode_t mode,
    distance_t oversize_distance,
    distance_t smooth_extraction_radius);

/**
 * @brief Function to import a dynamic obstacle
 * @warning Memory will be allocated and an unique ID will reserved so do not import too much obstacles
 * @note The shape must be centralized as best as possible on the origin
 * @note After the import, the obstacle will not have any position, speed or acceleration, so you must refresh it
 * @see dj_obstacle_dynamic_refresh
 * @param importer Pointer to the obstacle importer structure
 * @param initial_shape Shape of the obstacle
 * @param mode Oversize mode
 * @param is_enabled true if the obstacle is enabled, false otherwise
 * @param oversize_distance Distance to oversize the obstacle
 * @param get_position Callback to get the position of the dynamic obstacle
 * @return dj_dynamic_obstacle_id_t ID of the obstacle
 */
dj_dynamic_obstacle_id_t dj_obstacle_importer_import_dynamic_obstacle(
    dj_obstacle_importer_t *importer,
    dj_polygon_t *initial_shape,
    dj_obstacle_oversize_mode_t mode,
    distance_t oversize_distance,
    bool is_enabled,
    dj_obstacle_dynamic_get_position_cb_t get_position);

/**
 * @brief Set the additional arguments for dynamic obstacle position calculation
 * @param importer Pointer to the obstacle importer structure
 * @param args Pointer to the additional arguments
 */
void dj_obstacle_importer_set_dynamic_obstacle_position_args(dj_obstacle_importer_t *importer,
                                                             void *args);

/**
 * @brief Function to refresh the position of a dynamic obstacle
 * @param importer Pointer to the obstacle importer structure
 * @param obstacle_id ID of the obstacle to refresh
 * @param shape New shape of the dynamic obstacle
 * @param dj_dynamic_obstacle_id_t obstacle_id ID of the obstacle to refresh
 * @param linear_acceleration_2d_vector_t* acceleration Acceleration of the obstacle (mm/s^2) (on x and y axis)
 * @param linear_speed_2d_vector_t* initial_speed Initial speed of the obstacle (mm/s) (on x and y axis)
 * @param point_t* initial_position Initial position of the obstacle (mm) (on x and y axis)
 * @param distance_t oversize_distance Distance to oversize the obstacle
 * @param dj_obstacle_oversize_mode_t mode Oversize mode
 *
 * @note acceleration and initial_speed should be colinear
 */
void dj_obstacle_importer_refresh_dynamic_obstacle(dj_obstacle_importer_t *importer,
                                                   dj_dynamic_obstacle_id_t obstacle_id,
                                                   dj_polygon_t *shape,
                                                   linear_acceleration_2d_vector_t *acceleration,
                                                   linear_speed_2d_vector_t *initial_speed,
                                                   point_t *initial_position,
                                                   distance_t oversize_distance,
                                                   dj_obstacle_oversize_mode_t mode);

/**
 * @brief Function to enable or disable a static obstacle
 * @param importer Pointer to the obstacle importer structure
 * @param obstacle_id ID of the obstacle to enable or disable
 * @param enable true to enable the obstacle, false to disable it
 */
void dj_obstacle_importer_enable_static_obstacle(dj_obstacle_importer_t *importer,
                                                 dj_static_obstacle_id_t obstacle_id,
                                                 bool enable);

/**
 * @brief Function to enable or disable a dynamic obstacle
 * @param importer Pointer to the obstacle importer structure
 * @param obstacle_id ID of the obstacle to enable or disable
 * @param enable true to enable the obstacle, false to disable it
 */
void dj_obstacle_importer_enable_dynamic_obstacle(dj_obstacle_importer_t *importer,
                                                  dj_dynamic_obstacle_id_t obstacle_id,
                                                  bool enable);

/**
 * @brief Function to check if a specific static obstacle is enabled
 * @param importer Pointer to the obstacle importer structure
 * @param obstacle_id ID of the obstacle to check
 *
 * @return true if the obstacle is enabled, false otherwise
 */
bool dj_obstacle_importer_static_is_enabled(const dj_obstacle_importer_t *importer,
                                            dj_static_obstacle_id_t obstacle_id);

/**
 * @brief Function to check if a specific dynamic obstacle is enabled
 * @param importer Pointer to the obstacle importer structure
 * @param obstacle_id ID of the obstacle to check
 *
 * @return true if the obstacle is enabled, false otherwise
 */
bool dj_obstacle_importer_dynamic_is_enabled(const dj_obstacle_importer_t *importer,
                                             dj_dynamic_obstacle_id_t obstacle_id);
/**
 * @brief Function to get a static obstacle
 * @param importer Pointer to the obstacle importer structure
 * @param obstacle_id ID of the obstacle to get
 * @return dj_obstacle_static_t* The static obstacle
 */
dj_obstacle_static_t *dj_obstacle_importer_get_static_obstacle(
    const dj_obstacle_importer_t *importer, const dj_static_obstacle_id_t obstacle_id);

/**
 * @brief Function to get a dynamic obstacle
 * @param importer Pointer to the obstacle importer structure
 * @param obstacle_id ID of the obstacle to get
 * @return dj_obstacle_dynamic_t* The dynamic obstacle
 */
dj_obstacle_dynamic_t *dj_obstacle_importer_get_dynamic_obstacle(
    const dj_obstacle_importer_t *importer, const dj_dynamic_obstacle_id_t obstacle_id);

/**
 * @brief Function to enable or disable all dynamic obstacles
 * @note By default, all dynamic obstacles are enabled
 * @note This function overrides only the enable dynamic obstacles, so if you disable an obstacle
 * with dj_obstacle_importer_enable_dynamic_obstacle, it can't be enabled with this function
 * @param importer Pointer to the obstacle importer structure
 * @param enable true to enable all dynamic obstacles, false to disable them
 */
void dj_obstacle_importer_enable_dynamic_obstacles(dj_obstacle_importer_t *importer, bool enable);

/**
 * @brief Check if all registered dynamic obstacles have null speed and acceleration
 * @param importer Pointer to the obstacle importer structure
 * @return true if every dynamic obstacle has initial_speed == {0,0} and acceleration == {0,0},
 *         or if there are no dynamic obstacles. false otherwise.
 * @note The comparison is done against exact 0.0f — valid because the values are assigned
 *       explicitly via dj_obstacle_importer_refresh_dynamic_obstacle, not computed.
 */
bool dj_obstacle_importer_all_dynamic_null_kinematics(const dj_obstacle_importer_t *importer);

/* ******************************************* Public callback functions declarations ************************************ */
