/**
 * @file dj_graph_builder_node_ops.h
 * @brief Node operations for graph building (add, link, force link)
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph/dj_graph_graph.h"
#include "utils/dj/dj_graph/dj_graph_node.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "utils/dj/dj_obstacle/dj_obstacle_manager.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include <stdbool.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Result of finding an extraction point for a node on an obstacle
 */
typedef struct
{
    bool found;         ///< Whether a valid extraction point was found
    point_t extraction; ///< The extraction point coordinates
    float distance;     ///< Distance to the extraction point
} extraction_result_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Build all nodes from obstacles
 * @details Adds every vertex of enabled obstacles as a node in the graph
 *
 * @param builder Builder to add nodes to
 * @param viewer_status Viewer status for dynamic obstacles (NULL for static only)
 */
void dj_graph_builder_build_nodes(dj_graph_builder_t *builder, dj_viewer_status_t *viewer_status);

/**
 * @brief Add a node to the graph with verification
 * @details The node will be added only if:
 *   - It is not already in the graph
 *   - It is not on an obstacle
 *   - It is on the playground
 *
 * @param builder The graph builder
 * @param node The node to add
 * @param type The type of the node
 * @param viewer_status Viewer status for dynamic obstacles (NULL for static only)
 * @return Pointer to the added node, or NULL if not added
 */
dj_graph_node_t *dj_graph_builder_add_node_with_verif(dj_graph_builder_t *builder,
                                                      dj_graph_node_t *node,
                                                      dj_graph_node_type_t type,
                                                      dj_viewer_status_t *viewer_status);

/**
 * @brief Link a node with all other valid nodes
 * @details Creates links to all nodes that don't intersect with obstacles
 *
 * @param builder Builder containing the graph
 * @param node Node to link
 * @param obstacles List of all obstacles
 * @param is_prebuilt Whether the links are prebuilt
 * @param verbose Enable debug logging
 */
void dj_graph_builder_link_node(dj_graph_builder_t *builder,
                                dj_graph_node_t *node,
                                static_obstacles_list_t *obstacles,
                                bool is_prebuilt,
                                bool verbose);

/**
 * @brief Force link a node, extracting it from obstacles if necessary
 * @details If the node is on an obstacle, finds the nearest extraction point
 *
 * @param builder Builder containing the graph
 * @param node Node to link
 * @param obstacles List of all obstacles
 * @param smooth_extraction_radius Radius for smooth path extraction (mm)
 * @param is_prebuilt Whether the links are prebuilt
 * @param verbose Enable debug logging
 * @details smooth_extraction_radius is used to determine if the extraction points
 * should be the closest valid point or if a smooth extraction should be performed
 * by linking multiple points from the node. If an edge of an obstacle is within
 * the smooth_extraction_radius, multiple extraction points will be created along
 * the edge.
 */
void dj_graph_builder_force_link_node(dj_graph_builder_t *builder,
                                      dj_graph_node_t *node,
                                      static_obstacles_list_t *obstacles,
                                      bool is_prebuilt,
                                      bool verbose);

/**
 * @brief Find the best extraction point for a node on an obstacle
 * @details Checks vertices, edge projections, and edge intersections
 *
 * @param builder The graph builder (provides working buffer)
 * @param node_pos Position of the node to extract
 * @param obstacles List of obstacles
 * @return extraction_result_t Result containing extraction point if found
 */
extraction_result_t dj_graph_builder_find_extraction_point(dj_graph_builder_t *builder,
                                                           point_t node_pos,
                                                           static_obstacles_list_t *obstacles);

/* ******************************************* Public callback functions declarations ************************************ */
