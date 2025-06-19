/**
 * @file dj_graph_graph.c
 * @brief dj graph used in the pathfinding algorithm
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_graph_graph.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_graph_graph.h"
#include "../dj_config.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_graph/dj_graph_node.h"
#include "../dj_logs/dj_logs.h"

#include <stdbool.h>
#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief The radius of the robot (used to check if a node is on the playground)
 */
#define ROBOT_RADIUS                                                                                                        \
    (int16_t)(sqrt(SQUARE(DJ_ROBOT_WIDTH / 2) + SQUARE(MAX(DJ_ROBOT_FRONT_BORDER_DISTANCE, DJ_ROBOT_BACK_BORDER_DISTANCE))))

/**
 * @brief The oversize of the playground (used to check if a node is on the playground)
 */
#define EDGE_OVERSIZE (ROBOT_RADIUS + ROBOT_MARGIN)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Functions to manage the list of nodes and links
 */
MAGIC_ARRAY_SRC(node_list, dj_graph_node_t);
MAGIC_ARRAY_SRC(link_list, dj_graph_link_t);

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_graph_init(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, );
    node_list_init(&graph->nodes);
    link_list_init(&graph->links);
    graph->start = NULL;
    graph->end = NULL;
    graph->last_prebuilt_node = -1;
    graph->last_prebuilt_link = -1;
}

void dj_graph_graph_deinit(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, );
    // Deinitialize all the nodes and links
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *node = node_list_get(&graph->nodes, i);
        dj_control_non_null(node, );
        dj_graph_node_deinit(node);
    }
    for (uint32_t i = 0; i < link_list_size(&graph->links); i++)
    {
        dj_graph_link_t *link = link_list_get(&graph->links, i);
        dj_control_non_null(link, );
        dj_graph_link_deinit(link);
    }
}

dj_graph_node_t *dj_graph_graph_add_node(dj_graph_graph_t *graph, dj_graph_node_t *node, dj_graph_node_type_t type)
{
    dj_control_non_null(graph, NULL);
    dj_control_non_null(node, NULL);
    dj_graph_node_t *new_node = node_list_add(&graph->nodes, NULL);
    dj_control_non_null(new_node, NULL);
    dj_graph_node_init(new_node, &node->m_pos);
    new_node->m_enabled = node->m_enabled;
    switch (type)
    {
        case DJ_GRAPH_NODE_TYPE_START:
            graph->start = new_node;
            graph->last_prebuilt_node = node_list_size(&graph->nodes) - 1;
            break;
        case DJ_GRAPH_NODE_TYPE_END:
            graph->end = new_node;
            graph->last_prebuilt_node = node_list_size(&graph->nodes) - 1;
            break;
        case DJ_GRAPH_NODE_TYPE_PREBUILT:
            graph->last_prebuilt_node = node_list_size(&graph->nodes) - 1;
            break;
        default:
            break;
    }
    return new_node;
}

dj_graph_link_t *dj_graph_graph_add_link(dj_graph_graph_t *graph,
                                         dj_graph_node_t *node1,
                                         dj_graph_node_t *node2,
                                         bool is_prebuilt)
{
    dj_control_non_null(graph, NULL);
    dj_control_non_null(node1, NULL);
    dj_control_non_null(node2, NULL);
    dj_graph_link_t *new_link = link_list_add(&graph->links, NULL);
    dj_control_non_null(new_link, NULL);
    dj_graph_link_init(new_link, node1, node2);
    if (is_prebuilt)
    {
        graph->last_prebuilt_link = link_list_size(&graph->links) - 1;
    }
    return new_link;
}

dj_graph_node_t *dj_graph_graph_get_node(dj_graph_graph_t *graph, uint32_t id)
{
    dj_control_non_null(graph, NULL);
    return (dj_graph_node_t *)node_list_get(&graph->nodes, id);
}

dj_graph_node_t *dj_graph_graph_get_node_from_pos(dj_graph_graph_t *graph, GEOMETRY_point_t *pos)
{
    dj_control_non_null(graph, NULL);
    dj_control_non_null(pos, NULL);
    // Search a node with the same position
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *node = node_list_get(&graph->nodes, i);
        dj_control_non_null(node, NULL);
        if (GEOMETRY_point_compare(node->m_pos, *pos))
        {
            return node;
        }
    }
    return NULL;
}

dj_graph_link_t *dj_graph_graph_get_link(dj_graph_graph_t *graph, uint32_t id)
{
    if (graph == NULL)
    {
        return NULL;
    }
    return (dj_graph_link_t *)link_list_get(&graph->links, id);
}

dj_graph_link_t *dj_graph_graph_get_link_from_nodes(dj_graph_graph_t *graph, dj_graph_node_t *node1, dj_graph_node_t *node2)
{
    dj_control_non_null(graph, NULL);
    dj_control_non_null(node1, NULL);
    dj_control_non_null(node2, NULL);
    dj_graph_link_t link_to_check;
    dj_graph_link_init(&link_to_check, node1, node2);
    // Search a link with the same nodes
    for (uint32_t i = 0; i < link_list_size(&graph->links); i++)
    {
        dj_graph_link_t *link1 = link_list_get(&graph->links, i);
        dj_control_non_null(link1, NULL);
        if (dj_graph_link_compare(link1, &link_to_check))
        {
            dj_graph_link_deinit(&link_to_check);
            return link1;
        }
    }
    dj_graph_link_deinit(&link_to_check);
    return NULL;
}

uint32_t dj_graph_graph_get_num_nodes(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, 0);
    return node_list_size(&graph->nodes);
}

uint32_t dj_graph_graph_get_num_links(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, 0);
    return link_list_size(&graph->links);
}

void dj_graph_graph_clear_nodes(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, );
    // Clear the nodes
    node_list_clear(&graph->nodes);
}

void dj_graph_graph_clear_links(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, );
    // Clear the links
    link_list_clear(&graph->links);
}

void dj_graph_graph_clear(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, );
    // Clear the nodes and the links
    node_list_clear(&graph->nodes);
    link_list_clear(&graph->links);
    graph->start = NULL;
    graph->end = NULL;
}

void dj_graph_graph_clear_without_prebuilt(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, );
    // Clear the nodes wich are not prebuilt
    // The prebuilt nodes are at the beginning of the list
    node_list_reset(&graph->nodes, graph->last_prebuilt_node);
    link_list_reset(&graph->links, graph->last_prebuilt_link);
}

dj_graph_node_t *dj_graph_graph_get_start(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, NULL);
    return graph->start;
}

dj_graph_node_t *dj_graph_graph_get_end(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, NULL);
    return graph->end;
}

uint32_t dj_graph_graph_get_index_start(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, 0);
    // Search the index of the start node
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *node = node_list_get(&graph->nodes, i);
        dj_control_non_null(node, 0);
        if (node == graph->start)
        {
            return i;
        }
    }
    return 0;
}

uint32_t dj_graph_graph_get_index_end(dj_graph_graph_t *graph)
{
    dj_control_non_null(graph, 0);
    // Search the index of the end node
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *node = node_list_get(&graph->nodes, i);
        dj_control_non_null(node, 0);
        if (node == graph->end)
        {
            return i;
        }
    }
    return 0;
}

bool dj_graph_node_is_reachable(dj_graph_graph_t *graph, GEOMETRY_point_t *start, dj_graph_node_t *node)
{
    dj_control_non_null(graph, false);
    dj_control_non_null(start, false);
    dj_control_non_null(node, false);
    // Check if the start and the node are the same
    if (GEOMETRY_point_compare(*start, node->m_pos))
    {
        // We consider that a node isn't reachable from itself
        return false;
    }
    // shearch a node with the same position as the start
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *current_node = node_list_get(&graph->nodes, i);
        dj_control_non_null(current_node, false);
        if (GEOMETRY_point_compare(*start, current_node->m_pos))
        {
            // check if a link exists between the two nodes
            dj_graph_link_t *link = dj_graph_graph_get_link_from_nodes(graph, current_node, node);
            if (link != NULL)
            {
                return true;
            }
            return false;
        }
    }
    return false;
}

bool dj_graph_graph_is_node_on_playground(GEOMETRY_point_t *point)
{
    // Check if the point is on the playground
    return point->x > EDGE_OVERSIZE && point->x < DJ_PLAYGROUND_X_SIZE - EDGE_OVERSIZE && point->y > EDGE_OVERSIZE
           && point->y < DJ_PLAYGROUND_Y_SIZE - EDGE_OVERSIZE;
}

void dj_graph_graph_print(dj_graph_graph_t *graph)
{
#if DJ_ENABLE_DEBUG_LOGS
    // Print all the nodes
    // Print the number of nodes and links
    int nb_nodes = dj_graph_graph_get_num_nodes(graph);
    int nb_links = dj_graph_graph_get_num_links(graph);
    dj_debug_printf("Graph built with %d nodes and %d links\n", nb_nodes, nb_links);
    dj_debug_printf("Nodes:\n");
    for (int i = 0; i < nb_nodes; i++)
    {
        dj_graph_node_t *node = dj_graph_graph_get_node(graph, i);
        GEOMETRY_point_t pos = dj_graph_node_get_pos(node);
        dj_debug_printf("Node %d : (%d, %d) enabled = %d\n", i, pos.x, pos.y, node->m_enabled);
    }
    // Print all the links
    dj_debug_printf("Links:\n");
    for (int i = 0; i < nb_links; i++)
    {
        dj_graph_link_t *link = dj_graph_graph_get_link(graph, i);
        dj_graph_node_t *node1 = dj_graph_link_get_node1(link);
        dj_graph_node_t *node2 = dj_graph_link_get_node2(link);
        GEOMETRY_point_t pos1 = dj_graph_node_get_pos(node1);
        GEOMETRY_point_t pos2 = dj_graph_node_get_pos(node2);
        dj_debug_printf(
            "Link %d : (%d, %d) --- (%d, %d) enabled = %d\n", i, pos1.x, pos1.y, pos2.x, pos2.y, link->m_enabled);
    }
#endif
}

/* ******************************************* Public callback functions declarations ************************************ */
