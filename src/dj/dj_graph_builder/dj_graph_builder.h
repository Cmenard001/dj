/**
 * @file dj_graph_builder.h
 * @brief Graph builder is used to build the graph from the obstacles
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_graph_builder.c
 */

#ifndef __DJ_GRAPH_BUILDER_H__
#define __DJ_GRAPH_BUILDER_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_config.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_graph/dj_graph_graph.h"
#include "../dj_logs/dj_logs.h"
#include "../dj_obstacle/dj_obstacle_manager.h"
#include "../dj_viewer/dj_viewer_status.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

typedef struct
{
    dj_graph_graph_t m_graph;
    dj_obstacle_manager_t m_obstacle_manager;
} dj_graph_builder_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize the graph builder
 *
 * @param builder The graph builder to initialize
 */
void dj_graph_builder_init(dj_graph_builder_t *builder);

/**
 * @brief Function to deinitialize the graph builder
 *
 * @param builder The graph builder to deinitialize
 */
void dj_graph_builder_deinit(dj_graph_builder_t *builder);

/**
 * @brief Function to build the graph
 *
 * @param builder The graph builder to build
 * @param viewer_status The status of the viewer (used to build dynamic polygons) (NULL if you want to a graph without dynamic obstacles)
 */
void dj_graph_builder_build_all(dj_graph_builder_t *builder, dj_viewer_status_t *viewer_status, bool is_prebuilt);

/**
 * @brief Function to link a node with all other nodes which do not intersect with an obstacle
 *
 * @param builder Builder to build the links
 * @param node Node to link
 * @param obstacles List of all obstacles
 * @param is_prebuilt If the node is prebuilt
 * @param verbose If true, the function will print the links
 */
void dj_graph_builder_link_node(
    dj_graph_builder_t *builder, dj_graph_node_t *node, static_obstacles_list_t *obstacles, bool is_prebuilt, bool verbose);

/**
 * @brief Function to link a node with all other nodes
 * If the node is on an obstacle, some links will be created to extract the node from the obstacle
 *
 * @param builder Builder to build the links
 * @param node Node to link
 * @param obstacles List of all obstacles
 * @param is_prebuilt If the node is prebuilt
 * @param verbose If true, the function will print some debug messages
 */
void dj_graph_builder_force_link_node(
    dj_graph_builder_t *builder, dj_graph_node_t *node, static_obstacles_list_t *obstacles, bool is_prebuilt, bool verbose);

/**
 * @brief Add a node to the graph with verification
 * The node will be added only if :
 * - it is not already in the graph
 * - it is not on an obstacle
 * - it is on the playground
 *
 * @param builder The graph builder
 * @param node The node to add
 * @param type The type of the node
 * @param viewer_status The status of the viewer (used to get dynamic obstacles from a point of view in space time) (NULL if you want to a graph without dynamic obstacles)
 * @return dj_graph_node_t* The node added
 * @return NULL if the node is not added
 */
dj_graph_node_t *dj_graph_builder_add_node_with_verif(dj_graph_builder_t *builder,
                                                      dj_graph_node_t *node,
                                                      dj_graph_node_type_t type,
                                                      dj_viewer_status_t *viewer_status);

/**
 * @brief Rebuild the graph considering a new obstacle
 *
 * @param builder The graph builder
 * @param obstacle The obstacle to add
 * @param viewer_status The viewer status to build the graph
 * @param is_prebuilt If the nodes and links to add are prebuilt
 * @param add_obstacle_as_node If true, the obstacle will be added as a node in the graph
 */
void dj_graph_builder_build_with_obstacle(dj_graph_builder_t *builder,
                                          dj_obstacle_static_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          bool is_prebuilt,
                                          bool add_obstacle_as_node);

/**
 * @brief Rebuild the graph ingoring an obstacle
 *
 * @param builder The graph builder
 * @param obstacle The obstacle to ignore
 * @param viewer_status The viewer status to build the graph
 * @param is_prebuilt If the nodes and links to add are prebuilt
 */
void dj_graph_builder_build_without_obstacle(dj_graph_builder_t *builder,
                                             dj_obstacle_static_t *obstacle,
                                             dj_viewer_status_t *viewer_status,
                                             bool is_prebuilt);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
