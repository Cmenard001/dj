/**
 * @file dj_obstacle_static.h
 * @brief Static obstacle of dj
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_static.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_obstacle.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_id.h"
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
     * @see dj_static_obstacle_id_t
     */
    dj_static_obstacle_id_t id;
    /**
     * @brief Shape of the obstacle
     */
    dj_polygon_t shape;
    /**
     * @brief Radius for smooth path extraction around the obstacle (mm)
     */
    distance_t smooth_extraction_radius;
} dj_obstacle_static_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a static obstacle
 * @param obstacle Pointer to the obstacle to initialize
 * @param shape Polygon representing the shape of the obstacle
 * @param id ID of the obstacle (must be unique)
 * @param smooth_extraction_radius Radius for smooth path extraction around the obstacle (mm)
 * Used to extract smoothly from the obstacles when target or start point is close
 * to the edge of the obstacle.
 * @param is_enabled true if the obstacle is enabled, false otherwise
 */
void dj_obstacle_static_init(dj_obstacle_static_t *obstacle,
                             dj_polygon_t *shape,
                             dj_static_obstacle_id_t id,
                             distance_t smooth_extraction_radius,
                             bool is_enabled);

/* ******************************************* Public callback functions declarations ************************************ */
