/**
 * @file dj_graph_builder_precompute.h
 * @brief Precomputed polygon data structures for graph building optimization
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_obstacle_manager.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure to store precomputed polygon data for optimization
 * @note Includes geometry conversion and bounding box for fast rejection tests
 */
typedef struct
{
    polygon_t geometry;      ///< Converted geometry polygon
    distance_t min_x, max_x; ///< Bounding box X coordinates
    distance_t min_y, max_y; ///< Bounding box Y coordinates
    bool is_enabled;         ///< Cache of obstacle enabled state
    uint32_t nb_points;      ///< Cache of number of points
} precomputed_polygon_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Precompute polygon data for all obstacles
 * @param obstacles List of obstacles to precompute
 * @param precomputed Output array (must be pre-allocated with size >= DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES)
 * @return uint32_t Number of obstacles precomputed
 *
 * @note The precomputed array must have space for DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES elements
 * @note Disabled obstacles are marked with is_enabled = false
 */
uint32_t dj_graph_builder_precompute_obstacles(static_obstacles_list_t *obstacles,
                                               precomputed_polygon_t *precomputed);

/**
 * @brief Check if a link's bounding box intersects with an obstacle's bounding box
 * @param link_segment The link segment to check
 * @param precomp Precomputed polygon data
 * @return true if bounding boxes overlap
 * @return false if they don't overlap (early rejection)
 */
bool dj_graph_builder_bbox_overlap(const segment_t *link_segment,
                                   const precomputed_polygon_t *precomp);

/* ******************************************* Public callback functions declarations ************************************ */
