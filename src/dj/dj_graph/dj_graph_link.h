/**
 * @file dj_graph_link.h
 * @brief dj link of graph
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_graph_link.c
 */

#ifndef __DJ_GRAPH_LINK_H__
#define __DJ_GRAPH_LINK_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"
#include "dj_graph_node.h"

#include <stdbool.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a link of graph
 */
typedef struct
{
    dj_graph_node_t *m_node1;
    dj_graph_node_t *m_node2;
    bool m_enabled;
} dj_graph_link_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a link of graph
 *
 * @param link pointer to the link to be initialized
 * @param node1 pointer to the first node
 * @param node2 pointer to the second node
 */
void dj_graph_link_init(dj_graph_link_t *link, dj_graph_node_t *node1, dj_graph_node_t *node2);

/**
 * @brief Function to deinitialize a link of graph
 *
 * @param link pointer to the link to be deinitialized
 */
void dj_graph_link_deinit(dj_graph_link_t *link);

/**
 * @brief Function to get the first node of a link
 *
 * @param link pointer to the link
 * @return dj_graph_node_t* pointer to the first node
 */
dj_graph_node_t *dj_graph_link_get_node1(dj_graph_link_t *link);

/**
 * @brief Function to get the second node of a link
 *
 * @param link pointer to the link
 * @return dj_graph_node_t* pointer to the second node
 */
dj_graph_node_t *dj_graph_link_get_node2(dj_graph_link_t *link);

/**
 * @brief Function to compare two links of graph
 *
 * @param link1 link to compare
 * @param link2 link to compare
 * @return bool true if the links are equal, false otherwise
 */
bool dj_graph_link_compare(dj_graph_link_t *link1, dj_graph_link_t *link2);

/**
 * @brief Function to check if a link is connected to a node
 *
 * @param link pointer to the link
 * @param node pointer to the node
 * @return bool true if the link is connected to the node, false otherwise
 */
bool dj_graph_link_is_connected_to_node(dj_graph_link_t *link, dj_graph_node_t *node);

/**
 * @brief Function to check if a link is connected to a pos
 *
 * @param link pointer to the link
 * @param pos pointer to the pos
 * @return NULL if the pos isn't connected to the node
 * @return dj_graph_node_t* A pointer on the other node connected to the link
 */
dj_graph_node_t *dj_graph_link_is_connected_to_pos(dj_graph_link_t *link, GEOMETRY_point_t *pos);

/**
 * @brief Function to enable or disable a link
 *
 * @param link pointer to the link
 * @param enable true to enable the link, false to disable it
 */
void dj_graph_link_enable(dj_graph_link_t *link, bool enable);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
