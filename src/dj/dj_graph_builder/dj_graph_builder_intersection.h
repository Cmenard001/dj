/**
 * @file dj_graph_builder_intersection.h
 * @brief Intersection and collision detection for graph building
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder_precompute.h"
#include "utils/dj/dj_obstacle/dj_obstacle_manager.h"
#include "utils/maths/segment/segment.h"
#include <stdbool.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Check if a segment intersects with an obstacle
 * @details Handles special cases:
 *   - Links that are edges of the polygon (allowed)
 *   - Links that are diagonals passing through polygon interior (blocked)
 *
 * @param segment The segment to check
 * @param node_pos Position of the first node
 * @param other_node_pos Position of the second node
 * @param precomp Precomputed polygon data
 * @return true if the segment intersects and should block the link
 * @return false if the segment doesn't intersect or is a valid edge
 */
bool dj_graph_builder_segment_intersects_obstacle(const segment_t *segment,
                                                  const point_t *node_pos,
                                                  const point_t *other_node_pos,
                                                  const precomputed_polygon_t *precomp);

/**
 * @brief Check if a position is strictly inside any obstacle
 * @param pos Position to check
 * @param obstacles List of obstacles to check against
 * @return true if the position is strictly inside an obstacle
 * @return false otherwise
 */
bool dj_graph_builder_pos_on_obstacle(point_t pos, static_obstacles_list_t *obstacles);

/**
 * @brief Check if a link passes through the interior of a polygon
 * @details Samples points along the segment to detect interior passage
 *
 * @param segment The segment to check
 * @param precomp Precomputed polygon data
 * @return true if the segment passes through the polygon interior
 * @return false otherwise
 */
bool dj_graph_builder_link_through_interior(const segment_t *segment,
                                            const precomputed_polygon_t *precomp);

/**
 * @brief Check if a link is an edge of a polygon
 * @details Two nodes are considered to form an edge if they are consecutive vertices
 *
 * @param node_pos Position of first node
 * @param other_node_pos Position of second node
 * @param precomp Precomputed polygon data
 * @return true if the link is an edge of the polygon
 * @return false otherwise
 */
bool dj_graph_builder_is_polygon_edge(const point_t *node_pos,
                                      const point_t *other_node_pos,
                                      const precomputed_polygon_t *precomp);

/* ******************************************* Public callback functions declarations ************************************ */
