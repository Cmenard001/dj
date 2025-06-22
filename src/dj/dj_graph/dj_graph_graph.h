/**
 * @file dj_graph_graph.h
 * @brief dj graph used in the pathfinding algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_graph.c
 */

#ifndef __DJ_GRAPH_GRAPH_H__
#define __DJ_GRAPH_GRAPH_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_config.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "dj_graph_link.h"
#include "dj_graph_node.h"

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Enumeration to represent the type of a node
 */
typedef enum
{
    DJ_GRAPH_NODE_TYPE_NORMAL,
    DJ_GRAPH_NODE_TYPE_START,
    DJ_GRAPH_NODE_TYPE_END,
    DJ_GRAPH_NODE_TYPE_PREBUILT
} dj_graph_node_type_t;

MAGIC_ARRAY_TYPEDEF(node_list, dj_graph_node_t, DJ_GRAPH_GRAPH_MAX_NODES);
MAGIC_ARRAY_TYPEDEF(link_list, dj_graph_link_t, DJ_GRAPH_GRAPH_MAX_NODES *DJ_GRAPH_GRAPH_AVERAGE_LINKS / 2);

/**
 * @brief Structure to represent a graph
 */
typedef struct
{
    node_list_t nodes;
    link_list_t links;
    dj_graph_node_t *start;
    dj_graph_node_t *end;
    int32_t last_prebuilt_node;
    int32_t last_prebuilt_link;
} dj_graph_graph_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize a graph object
 *
 * @param graph The graph object to initialize
 */
void dj_graph_graph_init(dj_graph_graph_t *graph);

/**
 * @brief Deinitialize a graph object
 *
 * @param graph The graph object to deinitialize
 */
void dj_graph_graph_deinit(dj_graph_graph_t *graph);

/**
 * @brief Add a node to the graph
 *
 * @param graph The graph object
 * @param node The node to add
 * @return dj_graph_node_t* The node added
 */
dj_graph_node_t *dj_graph_graph_add_node(dj_graph_graph_t *graph, dj_graph_node_t *node, dj_graph_node_type_t type);

/**
 * @brief Add a link to the graph
 *
 * @param graph The graph object
 * @param node1 The id of the first node
 * @param node2 The id of the second node
 * @param is_prebuilt True if the link is prebuilt, false otherwise
 * @return dj_graph_link_t* The link added
 */
dj_graph_link_t *dj_graph_graph_add_link(dj_graph_graph_t *graph,
                                         dj_graph_node_t *node1,
                                         dj_graph_node_t *node2,
                                         bool is_prebuilt);

/**
 * @brief Get a node from the graph
 *
 * @param graph The graph object
 * @param id The ID of the node to get
 * @return dj_graph_node_t* The node object
 */
dj_graph_node_t *dj_graph_graph_get_node(dj_graph_graph_t *graph, uint32_t id);

/**
 * @brief Get a node from the graph using the position
 *
 * @param graph The graph object
 * @param pos The position of the node to get
 * @return dj_graph_node_t* The node object
 */
dj_graph_node_t *dj_graph_graph_get_node_from_pos(dj_graph_graph_t *graph, GEOMETRY_point_t *pos);

/**
 * @brief Get a link from the graph
 *
 * @param graph The graph object
 * @param id The ID of the link to get
 * @return dj_graph_link_t* The link object
 */
dj_graph_link_t *dj_graph_graph_get_link(dj_graph_graph_t *graph, uint32_t id);

/**
 * @brief Get a link from the graph using the nodes
 *
 * @param graph The graph object
 * @param node1 The first node
 * @param node2 The second node
 * @return dj_graph_link_t* The link object
 */
dj_graph_link_t *dj_graph_graph_get_link_from_nodes(dj_graph_graph_t *graph, dj_graph_node_t *node1, dj_graph_node_t *node2);

/**
 * @brief Get the number of nodes in the graph
 *
 * @param graph The graph object
 * @return uint32_t The number of nodes
 */
uint32_t dj_graph_graph_get_num_nodes(dj_graph_graph_t *graph);

/**
 * @brief Get the number of links in the graph
 *
 * @param graph The graph object
 * @return uint32_t The number of links
 */
uint32_t dj_graph_graph_get_num_links(dj_graph_graph_t *graph);

/**
 * @brief Functions to clear the nodes and links of the graph
 *
 * @param graph The graph object
 */
void dj_graph_graph_clear_nodes(dj_graph_graph_t *graph);
void dj_graph_graph_clear_links(dj_graph_graph_t *graph);
void dj_graph_graph_clear(dj_graph_graph_t *graph);
void dj_graph_graph_clear_without_prebuilt(dj_graph_graph_t *graph);

/**
 * @brief Function to get the start node of the graph
 *
 * @param graph The graph object
 * @return dj_graph_node_t* The start node
 */
dj_graph_node_t *dj_graph_graph_get_start(dj_graph_graph_t *graph);

/**
 * @brief Function to get the end node of the graph
 *
 * @param graph The graph object
 * @return dj_graph_node_t* The end node
 */
dj_graph_node_t *dj_graph_graph_get_end(dj_graph_graph_t *graph);

/**
 * @brief Function to get the start node of the graph
 *
 * @param graph The graph object
 * @return uint32_t The index of the start node
 */
uint32_t dj_graph_graph_get_index_start(dj_graph_graph_t *graph);

/**
 * @brief Function to get the end node of the graph
 *
 * @param graph The graph object
 * @return uint32_t The index of the end node
 */
uint32_t dj_graph_graph_get_index_end(dj_graph_graph_t *graph);

/**
 * @brief Function to check if a node is reachable from a point
 *
 * @param graph The graph object
 * @param start The point to start from
 * @param node The node to reach
 */
bool dj_graph_node_is_reachable(dj_graph_graph_t *graph, GEOMETRY_point_t *start, dj_graph_node_t *node);

/**
 * @brief Function to check if a node is on the playground
 *
 * @param point The point to check
 */
bool dj_graph_graph_is_node_on_playground(GEOMETRY_point_t *point);

/**
 * @brief Function to print the graph
 *
 * @param graph The graph object
 */
void dj_graph_graph_print(dj_graph_graph_t *graph);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
