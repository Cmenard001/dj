/**
 * @file dj_graph_link.c
 * @brief dj link of graph
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_link.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph/dj_graph_link.h"
#include "system/assert/system_assert.h"
#include <stdlib.h>
/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_link_init(dj_graph_link_t *link, dj_graph_node_t *node1, dj_graph_node_t *node2)
{
    SYSTEM_ASSERT(link != NULL);
    SYSTEM_ASSERT(node1 != NULL);
    SYSTEM_ASSERT(node2 != NULL);
    link->node1 = node1;
    link->node2 = node2;
    link->enabled = true;
}

dj_graph_node_t *dj_graph_link_get_node1(dj_graph_link_t *link)
{
    SYSTEM_ASSERT(link != NULL);
    return link->node1;
}

dj_graph_node_t *dj_graph_link_get_node2(dj_graph_link_t *link)
{
    SYSTEM_ASSERT(link != NULL);
    return link->node2;
}

inline bool dj_graph_link_compare(dj_graph_link_t *link1, dj_graph_link_t *link2)
{
    SYSTEM_ASSERT(link1 != NULL);
    SYSTEM_ASSERT(link2 != NULL);
    if ((link1 == link2) || ((dj_graph_node_compare(link1->node1, link2->node1) &&
                              dj_graph_node_compare(link1->node2, link2->node2)) ||
                             (dj_graph_node_compare(link1->node1, link2->node2) &&
                              dj_graph_node_compare(link1->node2, link2->node1))))
    {
        return true;
    }
    return false;
}

bool dj_graph_link_is_connected_to_node(dj_graph_link_t *link, dj_graph_node_t *node)
{
    SYSTEM_ASSERT(link != NULL);
    SYSTEM_ASSERT(node != NULL);
    if ((link->node1 == node || link->node2 == node) && link->enabled)
    {
        return true;
    }
    return false;
}

dj_graph_node_t *dj_graph_link_is_connected_to_pos(dj_graph_link_t *link, point_t *pos)
{
    SYSTEM_ASSERT(link != NULL);
    SYSTEM_ASSERT(pos != NULL);
    if (!link->enabled)
    {
        return NULL;
    }
    if (point_compare(&link->node1->pos, pos))
    {
        return link->node2;
    }
    if (point_compare(&link->node2->pos, pos))
    {
        return link->node1;
    }
    return NULL;
}

void dj_graph_link_enable(dj_graph_link_t *link, bool enable)
{
    SYSTEM_ASSERT(link != NULL);
    link->enabled = enable;
}

/* ******************************************* Public callback functions declarations ************************************ */
