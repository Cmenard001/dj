/**
 * @file dj_graph_graph.h
 * @brief dj graph used in the pathfinding algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_graph.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_config.h"
#include "utils/dj/dj_graph/dj_graph_link.h"
#include "utils/dj/dj_graph/dj_graph_node.h"
#include "utils/lists/magic_array.h"
#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum number of links per node in the graph
 */
#define DJ_GRAPH_GRAPH_MAX_LINKS (DJ_GRAPH_GRAPH_MAX_NODES * DJ_GRAPH_GRAPH_AVERAGE_LINKS / 2)

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
MAGIC_ARRAY_TYPEDEF(link_list, dj_graph_link_t, DJ_GRAPH_GRAPH_MAX_LINKS);

/**
 * @brief Structure to represent a graph
 */
typedef struct
{
    node_list_t nodes;
    link_list_t links;
    dj_graph_node_t *start;
    dj_graph_node_t *end;
    dj_graph_node_t *ends[DJ_ENGINE_MAX_DESTINATIONS];
    uint32_t end_count;
    int32_t last_prebuilt_node;
    int32_t last_prebuilt_link;
} dj_graph_graph_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize a graph object
 * @param graph The graph object to initialize
 * @param display True to display the graph, false otherwise
 * @warning Only one graph can be displayed at a time
 */
void dj_graph_graph_init(dj_graph_graph_t *graph, bool display);

/**
 * @brief Add a node to the graph
 * @param graph The graph object
 * @param node The node to add
 * @return dj_graph_node_t* The node added
 */
dj_graph_node_t *dj_graph_graph_add_node(dj_graph_graph_t *graph,
                                         dj_graph_node_t *node,
                                         dj_graph_node_type_t type);

/**
 * @brief Add a link to the graph
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
 * @param graph The graph object
 * @param id The ID of the node to get
 * @return dj_graph_node_t* The node object
 */
dj_graph_node_t *dj_graph_graph_get_node(dj_graph_graph_t *graph, uint32_t id);

/**
 * @brief Get a node from the graph using the position
 * @param graph The graph object
 * @param pos The position of the node to get
 * @return dj_graph_node_t* The node object
 */
dj_graph_node_t *dj_graph_graph_get_node_from_pos(dj_graph_graph_t *graph, point_t *pos);

/**
 * @brief Get a link from the graph
 * @param graph The graph object
 * @param id The ID of the link to get
 * @return dj_graph_link_t* The link object
 */
dj_graph_link_t *dj_graph_graph_get_link(dj_graph_graph_t *graph, uint32_t id);

/**
 * @brief Get a link from the graph using the nodes
 * @param graph The graph object
 * @param node1 The first node
 * @param node2 The second node
 * @return dj_graph_link_t* The link object
 */
dj_graph_link_t *dj_graph_graph_get_link_from_nodes(dj_graph_graph_t *graph,
                                                    dj_graph_node_t *node1,
                                                    dj_graph_node_t *node2);

/**
 * @brief Get the number of nodes in the graph
 * @param graph The graph object
 * @return uint32_t The number of nodes
 */
uint32_t dj_graph_graph_get_num_nodes(dj_graph_graph_t *graph);

/**
 * @brief Get the number of links in the graph
 * @param graph The graph object
 * @return uint32_t The number of links
 */
uint32_t dj_graph_graph_get_num_links(dj_graph_graph_t *graph);

/**
 * @brief Functions to clear the nodes and links of the graph
 * @param graph The graph object
 */
void dj_graph_graph_clear_nodes(dj_graph_graph_t *graph);
void dj_graph_graph_clear_links(dj_graph_graph_t *graph);
void dj_graph_graph_clear(dj_graph_graph_t *graph);
void dj_graph_graph_clear_without_prebuilt(dj_graph_graph_t *graph);

/**
 * @brief Function to get the start node of the graph
 * @param graph The graph object
 * @return dj_graph_node_t* The start node
 */
dj_graph_node_t *dj_graph_graph_get_start(dj_graph_graph_t *graph);

/**
 * @brief Function to get the end node of the graph
 * @param graph The graph object
 * @return dj_graph_node_t* The end node
 */
dj_graph_node_t *dj_graph_graph_get_end(dj_graph_graph_t *graph);

/**
 * @brief Get the number of end nodes in the graph
 * @param graph The graph object
 * @return uint32_t The number of end nodes
 */
uint32_t dj_graph_graph_get_end_count(dj_graph_graph_t *graph);

/**
 * @brief Get one of the end nodes by index
 * @param graph The graph object
 * @param index Index of the end node (0-based)
 * @return dj_graph_node_t* The end node, or NULL if index is invalid
 */
dj_graph_node_t *dj_graph_graph_get_end_by_index(dj_graph_graph_t *graph, uint32_t index);

/**
 * @brief Function to get the start node of the graph
 * @param graph The graph object
 * @return uint32_t The index of the start node
 */
uint32_t dj_graph_graph_get_index_start(dj_graph_graph_t *graph);

/**
 * @brief Function to get the end node of the graph
 * @param graph The graph object
 * @return uint32_t The index of the end node
 */
uint32_t dj_graph_graph_get_index_end(dj_graph_graph_t *graph);

/**
 * @brief Get the index of a node in the graph from its pointer (O(1) pointer arithmetic)
 * @param graph The graph object
 * @param node Pointer to the node (must belong to this graph)
 * @return uint32_t Index of the node in the node list
 */
uint32_t dj_graph_graph_get_node_index(dj_graph_graph_t *graph, const dj_graph_node_t *node);

/**
 * @brief Function to check if a node is reachable from a point
 * @param graph The graph object
 * @param start The point to start from
 * @param node The node to reach
 */
bool dj_graph_node_is_reachable(dj_graph_graph_t *graph, point_t *start, dj_graph_node_t *node);

/**
 * @brief Function to check if a node is on the playground
 * @param point The point to check
 */
bool dj_graph_graph_is_node_on_playground(point_t *point);

/**
 * @brief Function to print the graph
 * @param graph The graph object
 */
void dj_graph_graph_print(dj_graph_graph_t *graph);

/* ******************************************* Public callback functions declarations ************************************ */
