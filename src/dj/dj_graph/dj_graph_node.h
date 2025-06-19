/**
 * @file dj_graph_node.h
 * @brief dj node of graph
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_graph_node.c
 */

#ifndef __DJ_GRAPH_NODE_H__
#define __DJ_GRAPH_NODE_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"

#include <stdbool.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure to represent a node of graph
 */
typedef struct
{
    GEOMETRY_point_t m_pos;
    bool m_enabled;
} dj_graph_node_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a node of graph
 *
 * @param node pointer to the node to be initialized
 * @param pos location of the node
 */
void dj_graph_node_init(dj_graph_node_t *node, GEOMETRY_point_t *pos);

/**
 * @brief Function to deinitialize a node of graph
 *
 * @param node pointer to the node to be deinitialized
 */
void dj_graph_node_deinit(dj_graph_node_t *node);

/**
 * @brief Function to get the location of a node
 *
 * @param node pointer to the node
 * @return GEOMETRY_point_t location of the node
 */
GEOMETRY_point_t dj_graph_node_get_pos(dj_graph_node_t *node);

/**
 * @brief Function to compare two nodes of graph
 *
 * @param node1 first node to compare
 * @param node2 second node to compare
 * @return bool true if the nodes are equal, false otherwise
 */
bool dj_graph_node_compare(dj_graph_node_t *node1, dj_graph_node_t *node2);

/**
 * @brief Function to enable or disable a node of graph
 *
 * @param node pointer to the node to be enabled
 * @param enable true to enable the node, false to disable it
 */
void dj_graph_node_enable(dj_graph_node_t *node, bool enable);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
