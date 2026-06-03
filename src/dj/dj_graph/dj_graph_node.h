/**
 * @file dj_graph_node.h
 * @brief dj node of graph
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_node.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_config.h"
#include "utils/maths/point/point.h"
#include <stdbool.h>
#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure to represent a node of graph
 */
typedef struct
{
    point_t pos;
    bool enabled;
    uint16_t adj_link_indices[DJ_GRAPH_NODE_MAX_ADJACENCY]; /**< Indices of adjacent links */
    uint16_t adj_count;                                     /**< Number of adjacent links */
} dj_graph_node_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a node of graph
 * @param node pointer to the node to be initialized
 * @param pos location of the node
 */
void dj_graph_node_init(dj_graph_node_t *node, const point_t *pos);

/**
 * @brief Function to get the location of a node
 * @param node pointer to the node
 * @return point_t location of the node
 */
point_t dj_graph_node_get_pos(const dj_graph_node_t *node);

/**
 * @brief Function to compare two nodes of graph
 * @param node1 first node to compare
 * @param node2 second node to compare
 * @return bool true if the nodes are equal, false otherwise
 */
bool dj_graph_node_compare(const dj_graph_node_t *node1, const dj_graph_node_t *node2);

/**
 * @brief Function to enable or disable a node of graph
 * @param node pointer to the node to be enabled
 * @param enable true to enable the node, false to disable it
 */
void dj_graph_node_enable(dj_graph_node_t *node, bool enable);

/* ******************************************* Public callback functions declarations ************************************ */
