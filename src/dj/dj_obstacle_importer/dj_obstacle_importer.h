/**
 * @file dj_obstacle_importer.h
 * @brief Importer of dj obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obsacle_importer.c
 */

#ifndef __DJ_OBSTACLE_IMPORTER_H__
#define __DJ_OBSTACLE_IMPORTER_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_obstacle/dj_obstacle_dynamic.h"
#include "../dj_obstacle/dj_obstacle_static.h"
#include "../dj_obstacle/dj_oversize_obstacle.h"
#include "../dj_obstacle/dj_polygon.h"
#include "dj_obstacle_id.h"

#include <stdbool.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize the obstacle importer
 * @warning This function should be called at match start (when the color is known)
 * @warning Do not call this function two times !!!
 */
void dj_obstacle_importer_init();

/**
 * @brief Function to deinitialize the obstacle importer
 * @warning This function must be called only if an initialization has been done
 */
void dj_obstacle_importer_deinit();

/**
 * @brief Function to import default obstacles
 * @warning This function should be called before the beginning of the match
 */
void dj_obstacle_import_default_obstacles();

/**
 * @brief Function to import a static obstacle
 * @warning Memory will be allocated and an unique ID will reserved so do not import too much obstacles
 *
 * @param shape Shape of the obstacle
 * @param is_enabled true if the obstacle is enabled, false otherwise
 * @param mode Oversize mode
 * @param margin Margin to take into account in millimeters
 * @return dj_static_obstacle_id_e ID of the obstacle
 */
dj_static_obstacle_id_e dj_obstacle_importer_import_static_obstacle(dj_polygon_t *shape,
                                                                    bool is_enabled,
                                                                    dj_obsrtacle_oversize_mode_e mode,
                                                                    uint8_t margin);

/**
 * @brief Function to import a dynamic obstacle
 * @warning Memory will be allocated and an unique ID will reserved so do not import too much obstacles
 * @note The shape must be centralized as best as possible on the origin
 * @note After the import, the obstacle will not have any position, speed or acceleration, so you must refresh it
 * @see dj_obstacle_dynamic_refresh
 *
 * @param initial_shape Shape of the obstacle
 * @param is_enabled true if the obstacle is enabled, false otherwise
 * @return dj_dynamic_obstacle_id_e ID of the obstacle
 */
dj_dynamic_obstacle_id_e dj_obstacle_importer_import_dynamic_obstacle(dj_polygon_t *initial_shape, bool is_enabled);

/**
 * @brief Function to refresh the position of a dynamic obstacle
 *
 * @param dj_dynamic_obstacle_id_e obstacle_id ID of the obstacle to refresh
 * @param GEOMETRY_vector_t* acceleration Acceleration of the obstacle (mm/s^2) (on x and y axis)
 * @param GEOMETRY_vector_t* initial_speed Initial speed of the obstacle (mm/s) (on x and y axis)
 * @param GEOMETRY_point_t* initial_position Initial position of the obstacle (mm) (on x and y axis)
 *
 * @note acceleration and initial_speed should be colinear
 */
void dj_obstacle_importer_refresh_dynamic_obstacle(dj_dynamic_obstacle_id_e obstacle_id,
                                                   GEOMETRY_vector_t *acceleration,
                                                   GEOMETRY_vector_t *initial_speed,
                                                   GEOMETRY_point_t *initial_position);

/**
 * @brief Function to enable or disable a static obstacle
 *
 * @param obstacle_id ID of the obstacle to enable or disable
 * @param enable true to enable the obstacle, false to disable it
 */
void dj_obstacle_importer_enable_static_obstacle(dj_static_obstacle_id_e obstacle_id, bool enable);

/**
 * @brief Function to enable or disable a dynamic obstacle
 *
 * @param obstacle_id ID of the obstacle to enable or disable
 * @param enable true to enable the obstacle, false to disable it
 */
void dj_obstacle_importer_enable_dynamic_obstacle(dj_dynamic_obstacle_id_e obstacle_id, bool enable);

/**
 * @brief Function to check if a specific static obstacle is enabled
 *
 * @param obstacle_id ID of the obstacle to check
 *
 * @return true if the obstacle is enabled, false otherwise
 */
bool dj_obstacle_importer_static_is_enabled(dj_static_obstacle_id_e obstacle_id);

/**
 * @brief Function to check if a specific dynamic obstacle is enabled
 *
 * @param obstacle_id ID of the obstacle to check
 *
 * @return true if the obstacle is enabled, false otherwise
 */
bool dj_obstacle_importer_dynamic_is_enabled(dj_dynamic_obstacle_id_e obstacle_id);

/**
 * @brief Function to get a static obstacle
 *
 * @param obstacle_id ID of the obstacle to get
 * @return dj_obstacle_static_t* The static obstacle
 */
dj_obstacle_static_t *dj_obstacle_importer_get_static_obstacle(dj_static_obstacle_id_e obstacle_id);

/**
 * @brief Function to get a static obstacle
 *
 * @param obstacle_id ID of the obstacle to get
 * @return dj_obstacle_static_t* The static obstacle
 */
dj_obstacle_static_t *dj_obstacle_importer_get_static_obstacle(dj_static_obstacle_id_e obstacle_id);

/**
 * @brief Function to get a dynamic obstacle
 *
 * @param obstacle_id ID of the obstacle to get
 * @return dj_obstacle_dynamic_t* The dynamic obstacle
 */
dj_obstacle_dynamic_t *dj_obstacle_importer_get_dynamic_obstacle(dj_dynamic_obstacle_id_e obstacle_id);

/**
 * @brief Function to enable or disable all dynamic obstacles
 * @note By default, all dynamic obstacles are enabled
 * @note This function overrides only the enable dynamic obstacles, so if you disable an obstacle
 * with dj_obstacle_importer_enable_dynamic_obstacle, it can't be enabled with this function
 *
 * @param enable true to enable all dynamic obstacles, false to disable them
 */
void dj_obstacle_importer_enable_dynamic_obstacles(bool enable);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
