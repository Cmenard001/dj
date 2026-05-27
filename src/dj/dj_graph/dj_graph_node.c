/**
 * @file dj_graph_node.c
 * @brief dj node of graph
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_node.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph/dj_graph_node.h"
#include "system/assert/system_assert.h"
#include <stdlib.h>
/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_node_init(dj_graph_node_t *node, const point_t *pos)
{
    SYSTEM_ASSERT(node != NULL);
    SYSTEM_ASSERT(pos != NULL);
    node->pos = *pos;
    node->enabled = true;
    node->adj_count = 0;
}

point_t dj_graph_node_get_pos(const dj_graph_node_t *node)
{
    SYSTEM_ASSERT(node != NULL);
    return node->pos;
}

inline bool dj_graph_node_compare(const dj_graph_node_t *node1, const dj_graph_node_t *node2)
{
    SYSTEM_ASSERT(node1 != NULL);
    SYSTEM_ASSERT(node2 != NULL);
    return point_compare(&node1->pos, &node2->pos);
}

void dj_graph_node_enable(dj_graph_node_t *node, bool enable)
{
    SYSTEM_ASSERT(node != NULL);
    node->enabled = enable;
}

/* ******************************************* Public callback functions declarations ************************************ */
