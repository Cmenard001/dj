/**
 * @file dj_obstacle_static.h
 * @brief Static obstacle of dj
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_obstacle_static.c
 */

#ifndef __DJ_OBSTACLE_STATIC_H__
#define __DJ_OBSTACLE_STATIC_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_obstacle_importer/dj_obstacle_id.h"
#include "dj_obstacle.h"
#include "dj_polygon.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a static obstacle
 */
typedef struct
{
    // Mother structure attributes
    DJ_OBSTACLE_MOTHER_ATTRIBUTES
    // Specific attributes
    /**
     * @brief ID of the obstacle
     * @see dj_static_obstacle_id_e
     */
    dj_static_obstacle_id_e m_id;
    /**
     * @brief Shape of the obstacle
     */
    dj_polygon_t m_shape;
} dj_obstacle_static_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a static obstacle
 *
 * @param obstacle Pointer to the obstacle to initialize
 * @param shape Polygon representing the shape of the obstacle
 * @param id ID of the obstacle (must be unique)
 * @param is_enabled true if the obstacle is enabled, false otherwise
 */
void dj_obstacle_static_init(dj_obstacle_static_t *obstacle,
                             dj_polygon_t *shape,
                             dj_static_obstacle_id_e id,
                             bool is_enabled);

/**
 * @brief Function to deinitialize a static obstacle
 *
 * @param obstacle Pointer to the obstacle to deinitialize
 */
void dj_obstacle_static_deinit(dj_obstacle_static_t *obstacle);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
