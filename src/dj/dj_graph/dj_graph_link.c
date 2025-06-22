/**
 * @file dj_graph_link.c
 * @brief dj link of graph
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_link.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_graph_link.h"
#include "../dj_logs/dj_logs.h"

#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_link_init(dj_graph_link_t *link, dj_graph_node_t *node1, dj_graph_node_t *node2)
{
    dj_control_non_null(link, );
    dj_control_non_null(node1, );
    dj_control_non_null(node2, );
    link->m_node1 = node1;
    link->m_node2 = node2;
    link->m_enabled = true;
}

void dj_graph_link_deinit(dj_graph_link_t *link)
{
    // Nothing to do
}

dj_graph_node_t *dj_graph_link_get_node1(dj_graph_link_t *link)
{
    dj_control_non_null(link, NULL);
    return link->m_node1;
}

dj_graph_node_t *dj_graph_link_get_node2(dj_graph_link_t *link)
{
    dj_control_non_null(link, NULL);
    return link->m_node2;
}

inline bool dj_graph_link_compare(dj_graph_link_t *link1, dj_graph_link_t *link2)
{
    dj_control_non_null(link1, false);
    dj_control_non_null(link2, false);
    if ((link1 == link2)
        || ((dj_graph_node_compare(link1->m_node1, link2->m_node1) && dj_graph_node_compare(link1->m_node2, link2->m_node2))
            || (dj_graph_node_compare(link1->m_node1, link2->m_node2)
                && dj_graph_node_compare(link1->m_node2, link2->m_node1))))
    {
        return true;
    }
    return false;
}

bool dj_graph_link_is_connected_to_node(dj_graph_link_t *link, dj_graph_node_t *node)
{
    dj_control_non_null(link, false);
    dj_control_non_null(node, false);
    if ((link->m_node1 == node || link->m_node2 == node) && link->m_enabled)
    {
        return true;
    }
    return false;
}

dj_graph_node_t *dj_graph_link_is_connected_to_pos(dj_graph_link_t *link, GEOMETRY_point_t *pos)
{
    dj_control_non_null(link, NULL);
    dj_control_non_null(pos, NULL);
    if (!link->m_enabled)
    {
        return NULL;
    }
    if (GEOMETRY_point_compare(link->m_node1->m_pos, *pos))
    {
        return link->m_node2;
    }
    if (GEOMETRY_point_compare(link->m_node2->m_pos, *pos))
    {
        return link->m_node1;
    }
    return NULL;
}

void dj_graph_link_enable(dj_graph_link_t *link, bool enable)
{
    dj_control_non_null(link, );
    link->m_enabled = enable;
}

/* ******************************************* Public callback functions declarations ************************************ */
