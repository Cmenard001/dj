/**
 * @file dj_graph_node.c
 * @brief dj node of graph
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_node.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_graph_node.h"
#include "../dj_logs/dj_logs.h"

#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_node_init(dj_graph_node_t *node, GEOMETRY_point_t *pos)
{
    dj_control_non_null(node, );
    dj_control_non_null(pos, );
    node->m_pos = *pos;
    node->m_enabled = true;
}

void dj_graph_node_deinit(dj_graph_node_t *node)
{
    // Nothing to do
}

GEOMETRY_point_t dj_graph_node_get_pos(dj_graph_node_t *node)
{
    GEOMETRY_point_t null_point = {0, 0};
    dj_control_non_null(node, null_point);
    return node->m_pos;
}

inline bool dj_graph_node_compare(dj_graph_node_t *node1, dj_graph_node_t *node2)
{
    dj_control_non_null(node1, false);
    dj_control_non_null(node2, false);
    return GEOMETRY_point_compare(node1->m_pos, node2->m_pos);
}

void dj_graph_node_enable(dj_graph_node_t *node, bool enable)
{
    dj_control_non_null(node, );
    node->m_enabled = enable;
}

/* ******************************************* Public callback functions declarations ************************************ */
