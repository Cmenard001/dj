/**
 * @file dj_graph_builder_link_ops.h
 * @brief Link operations for graph building (build all links, handle obstacles)
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "utils/dj/dj_obstacle/dj_obstacle_manager.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include <stdbool.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Build links between all nodes in the graph
 * @details Links all nodes that don't intersect with obstacles
 *
 * @param builder Builder containing the graph
 * @param viewer_status Viewer status for dynamic obstacles (NULL for static only)
 * @param is_prebuilt Whether the links are prebuilt
 */
void dj_graph_builder_build_all_links(dj_graph_builder_t *builder,
                                      dj_viewer_status_t *viewer_status,
                                      bool is_prebuilt);

/**
 * @brief Rebuild graph considering a new obstacle
 * @details Disables intersecting links and optionally adds obstacle nodes
 *
 * @param builder Builder to update
 * @param obstacle New obstacle to consider
 * @param viewer_status Viewer status for dynamic obstacles
 * @param is_prebuilt Whether new links are prebuilt
 * @param add_obstacle_as_node Whether to add obstacle vertices as nodes
 */
void dj_graph_builder_build_with_obstacle(dj_graph_builder_t *builder,
                                          dj_obstacle_static_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          bool is_prebuilt,
                                          bool add_obstacle_as_node);

/**
 * @brief Disable links that intersect with an obstacle
 * @details Uses bounding box optimization for fast rejection
 *
 * @param builder Builder containing the graph
 * @param obstacle Obstacle to check against
 */
void dj_graph_builder_disable_intersecting_links(dj_graph_builder_t *builder,
                                                 dj_obstacle_static_t *obstacle);

/**
 * @brief Add obstacle vertices as graph nodes and link them
 * @param builder Builder to add nodes to
 * @param obstacle Obstacle whose vertices to add
 * @param viewer_status Viewer status for verification
 * @param obstacles All obstacles for linking
 * @param is_prebuilt Whether links are prebuilt
 */
void dj_graph_builder_add_obstacle_nodes(dj_graph_builder_t *builder,
                                         dj_obstacle_static_t *obstacle,
                                         dj_viewer_status_t *viewer_status,
                                         static_obstacles_list_t *obstacles,
                                         bool is_prebuilt);

/* ******************************************* Public callback functions declarations ************************************ */
