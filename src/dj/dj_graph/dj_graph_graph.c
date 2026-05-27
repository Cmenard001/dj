/**
 * @file dj_graph_graph.c
 * @brief dj graph used in the pathfinding algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_graph.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph/dj_graph_graph.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "system/task_manager/task_manager.h"
#include "utils/dj/dj_config.h"
#include "utils/dj/dj_graph/dj_graph_node.h"
#include "utils/shape/shape.h"
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

LOG_REGISTER("utils/dj/graph_graph");

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Period to display the graph (in ms)
 */
#define DISPLAY_GRAPH_PERIOD_MS (15000)

#ifndef DJ_GRAPH_RAM_SUFFIX
#define DJ_GRAPH_RAM_SUFFIX
#endif

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

MAGIC_ARRAY_DECLARATION(node_list, dj_graph_node_t);
MAGIC_ARRAY_DECLARATION(link_list, dj_graph_link_t);

/**
 * @brief Task to display the graph
 * @param arg Unused
 */
static void dj_graph_graph_display_task(void *arg);

/* ************************************************** Private variables ************************************************** */

/**
 * @brief Shape handles for nodes (circles)
 */
static shape_handle_t node_handles[DJ_GRAPH_GRAPH_MAX_NODES] DJ_GRAPH_RAM_SUFFIX = {NULL};

/**
 * @brief Shape handles for links (vectors/lines)
 */
static shape_handle_t link_handles[DJ_GRAPH_GRAPH_MAX_LINKS] DJ_GRAPH_RAM_SUFFIX = {NULL};

static bool display_task_created = false;

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Functions to manage the list of nodes and links
 */
MAGIC_ARRAY_DEFINITION(node_list, dj_graph_node_t);
MAGIC_ARRAY_DEFINITION(link_list, dj_graph_link_t);

static void dj_graph_graph_display_task(void *arg)
{
    dj_graph_graph_t *graph = (dj_graph_graph_t *)arg;
    SYSTEM_ASSERT(graph != NULL);
    uint32_t nb_nodes = dj_graph_graph_get_num_nodes(graph);
    uint32_t nb_links = dj_graph_graph_get_num_links(graph);

    // Draw nodes as circles
    for (uint32_t i = 0; i < nb_nodes && i < DJ_GRAPH_GRAPH_MAX_NODES; i++)
    {
        dj_graph_node_t *node = dj_graph_graph_get_node(graph, i);
        if (node == NULL)
        {
            continue;
        }

        // Create handle if it doesn't exist yet
        if (node_handles[i] == NULL)
        {
            node_handles[i] = shape_new();
            if (node_handles[i] == NULL)
            {
                LOGE("Failed to create shape handle for node %d", i);
                continue;
            }
        }

        shape_handle_t shape = node_handles[i];

        // Determine color based on node type
        uint32_t color;
        float opacity;
        if (node == graph->start)
        {
            color = 0x00FF00; // Green for start
            opacity = 1.0f;
        }
        else if (node == graph->end)
        {
            color = 0xFF0000; // Red for end
            opacity = 1.0f;
        }
        else
        {
            // Check if node is one of the other end nodes
            bool is_end_node = false;
            for (uint32_t e = 0; e < graph->end_count; e++)
            {
                if (node == graph->ends[e])
                {
                    is_end_node = true;
                    break;
                }
            }
            if (is_end_node)
            {
                color = 0xFF8000; // Orange for additional end nodes
                opacity = 1.0f;
            }
            else
            {
                color = 0x0080FF; // Blue for regular nodes
                opacity = node->enabled ? 0.8f : 0.3f;
            }
        }

        // Set style properties
        shape_set_outline(shape, color, 2.0f);
        shape_set_fill(shape, color);
        shape_set_opacity(shape, opacity);

        // Set label
        char label[32];
        if (node == graph->start)
        {
            snprintf(label, sizeof(label), "Start");
        }
        else if (node == graph->end)
        {
            snprintf(label, sizeof(label), "End");
        }
        else
        {
            // Check for additional end nodes
            bool is_end_node = false;
            for (uint32_t e = 0; e < graph->end_count; e++)
            {
                if (node == graph->ends[e])
                {
                    snprintf(label, sizeof(label), "End %" PRIu32, e);
                    is_end_node = true;
                    break;
                }
            }
            if (!is_end_node)
            {
                snprintf(label, sizeof(label), "Node %" PRIu32, i);
            }
        }
        shape_set_label(shape, label);

        // Draw circle
        point_t pos = dj_graph_node_get_pos(node);
        shape_draw_circle(shape, pos, 15.0f);
    }

    // Draw links as lines between nodes
    for (uint32_t i = 0; i < nb_links && i < DJ_GRAPH_GRAPH_MAX_LINKS; i++)
    {
        dj_graph_link_t *link = dj_graph_graph_get_link(graph, i);
        if (link == NULL)
        {
            continue;
        }

        // Create handle if it doesn't exist yet
        if (link_handles[i] == NULL)
        {
            link_handles[i] = shape_new();
            if (link_handles[i] == NULL)
            {
                LOGE("Failed to create shape handle for link %d", i);
                continue;
            }
        }

        shape_handle_t shape = link_handles[i];

        // Get the two nodes
        dj_graph_node_t *node1 = dj_graph_link_get_node1(link);
        dj_graph_node_t *node2 = dj_graph_link_get_node2(link);
        if (node1 == NULL || node2 == NULL)
        {
            continue;
        }

        point_t pos1 = dj_graph_node_get_pos(node1);
        point_t pos2 = dj_graph_node_get_pos(node2);

        // Calculate vector from pos1 to pos2
        vector_2d_distance_t vector = {.x = pos2.x - pos1.x, .y = pos2.y - pos1.y};

        // Determine color and opacity based on enabled state
        uint32_t color = link->enabled ? 0xFFFF00 : 0x808080; // Yellow if enabled, gray if disabled
        float opacity = link->enabled ? 0.7f : 0.2f;

        // Set style properties
        shape_set_outline(shape, color, link->enabled ? 2.0f : 1.0f);
        shape_set_opacity(shape, opacity);

        char label[64];
        snprintf(label, sizeof(label), "Link %" PRIu32, i);
        shape_set_label(shape, label);

        // Draw vector (line) without arrow
        shape_draw_vector(shape, pos1, vector, ShapeArrowType_SHAPE_ARROW_NONE);
    }
}

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_graph_init(dj_graph_graph_t *graph, bool display)
{
    SYSTEM_ASSERT(graph != NULL);
    node_list_init(&graph->nodes);
    link_list_init(&graph->links);
    graph->start = NULL;
    graph->end = NULL;
    graph->end_count = 0;
    for (uint32_t i = 0; i < DJ_ENGINE_MAX_DESTINATIONS; i++)
    {
        graph->ends[i] = NULL;
    }
    graph->last_prebuilt_node = -1;
    graph->last_prebuilt_link = -1;
    if (display && !display_task_created)
    {
        // Create a task to display the graph
        task_manager_register_process_main("dj graph Display",
                                           DISPLAY_GRAPH_PERIOD_MS,
                                           dj_graph_graph_display_task,
                                           graph);
        display_task_created = true;
    }
}

dj_graph_node_t *dj_graph_graph_add_node(dj_graph_graph_t *graph,
                                         dj_graph_node_t *node,
                                         dj_graph_node_type_t type)
{
    SYSTEM_ASSERT(graph != NULL);
    SYSTEM_ASSERT(node != NULL);
    dj_graph_node_t *new_node = node_list_add(&graph->nodes, NULL);
    if (new_node == NULL)
    {
        return NULL;
    }
    dj_graph_node_init(new_node, &node->pos);
    new_node->enabled = node->enabled;
    switch (type)
    {
        case DJ_GRAPH_NODE_TYPE_START:
            graph->start = new_node;
            graph->last_prebuilt_node = (int32_t)node_list_size(&graph->nodes) - 1;
            break;
        case DJ_GRAPH_NODE_TYPE_END:
            graph->end = new_node;
            if (graph->end_count < DJ_ENGINE_MAX_DESTINATIONS)
            {
                graph->ends[graph->end_count] = new_node;
                graph->end_count++;
            }
            graph->last_prebuilt_node = (int32_t)node_list_size(&graph->nodes) - 1;
            break;
        case DJ_GRAPH_NODE_TYPE_PREBUILT:
            graph->last_prebuilt_node = (int32_t)node_list_size(&graph->nodes) - 1;
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
    SYSTEM_ASSERT(graph != NULL);
    SYSTEM_ASSERT(node1 != NULL);
    SYSTEM_ASSERT(node2 != NULL);
    dj_graph_link_t *new_link = link_list_add(&graph->links, NULL);
    if (new_link == NULL)
    {
        return NULL;
    }
    dj_graph_link_init(new_link, node1, node2);
    if (is_prebuilt)
    {
        graph->last_prebuilt_link = (int32_t)link_list_size(&graph->links) - 1;
    }

    // Update adjacency lists
    uint16_t link_index = (uint16_t)(link_list_size(&graph->links) - 1u);
    if (node1->adj_count < DJ_GRAPH_NODE_MAX_ADJACENCY)
    {
        node1->adj_link_indices[node1->adj_count] = link_index;
        node1->adj_count++;
    }
    else
    {
        LOGE("Node at (%f, %f) has reached max adjacency count, cannot add more links",
             node1->pos.x,
             node1->pos.y);
    }
    if (node2->adj_count < DJ_GRAPH_NODE_MAX_ADJACENCY)
    {
        node2->adj_link_indices[node2->adj_count] = link_index;
        node2->adj_count++;
    }
    else
    {
        LOGE("Node at (%f, %f) has reached max adjacency count, cannot add more links",
             node2->pos.x,
             node2->pos.y);
    }

    return new_link;
}

dj_graph_node_t *dj_graph_graph_get_node(dj_graph_graph_t *graph, uint32_t id)
{
    SYSTEM_ASSERT(graph != NULL);
    return (dj_graph_node_t *)node_list_get(&graph->nodes, id);
}

dj_graph_node_t *dj_graph_graph_get_node_from_pos(dj_graph_graph_t *graph, point_t *pos)
{
    SYSTEM_ASSERT(graph != NULL);
    SYSTEM_ASSERT(pos != NULL);
    // Search a node with the same position
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *node = node_list_get(&graph->nodes, i);
        if (node == NULL)
        {
            continue;
        }
        if (point_compare(&node->pos, pos))
        {
            return node;
        }
    }
    return NULL;
}

dj_graph_link_t *dj_graph_graph_get_link(dj_graph_graph_t *graph, uint32_t id)
{
    SYSTEM_ASSERT(graph != NULL);
    return (dj_graph_link_t *)link_list_get(&graph->links, id);
}

dj_graph_link_t *dj_graph_graph_get_link_from_nodes(dj_graph_graph_t *graph,
                                                    dj_graph_node_t *node1,
                                                    dj_graph_node_t *node2)
{
    SYSTEM_ASSERT(graph != NULL);
    SYSTEM_ASSERT(node1 != NULL);
    SYSTEM_ASSERT(node2 != NULL);
    dj_graph_link_t link_to_check;
    dj_graph_link_init(&link_to_check, node1, node2);
    // Search a link with the same nodes
    for (uint32_t i = 0; i < link_list_size(&graph->links); i++)
    {
        dj_graph_link_t *link1 = link_list_get(&graph->links, i);
        if (link1 == NULL)
        {
            continue;
        }
        if (dj_graph_link_compare(link1, &link_to_check))
        {
            return link1;
        }
    }
    return NULL;
}

uint32_t dj_graph_graph_get_num_nodes(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    return node_list_size(&graph->nodes);
}

uint32_t dj_graph_graph_get_num_links(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    return link_list_size(&graph->links);
}

void dj_graph_graph_clear_nodes(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    // Clear the nodes
    node_list_clear(&graph->nodes);
}

void dj_graph_graph_clear_links(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    // Clear the links
    link_list_clear(&graph->links);
}

void dj_graph_graph_clear(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    // Clear the nodes and the links
    node_list_clear(&graph->nodes);
    link_list_clear(&graph->links);
    graph->start = NULL;
    graph->end = NULL;
    graph->end_count = 0;
    for (uint32_t i = 0; i < DJ_ENGINE_MAX_DESTINATIONS; i++)
    {
        graph->ends[i] = NULL;
    }
}

void dj_graph_graph_clear_without_prebuilt(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    // Clear the nodes wich are not prebuilt
    // The prebuilt nodes are at the beginning of the list
    node_list_reset(&graph->nodes, (uint32_t)graph->last_prebuilt_node);
    link_list_reset(&graph->links, (uint32_t)graph->last_prebuilt_link);
}

dj_graph_node_t *dj_graph_graph_get_start(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    return graph->start;
}

dj_graph_node_t *dj_graph_graph_get_end(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    return graph->end;
}

uint32_t dj_graph_graph_get_end_count(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    return graph->end_count;
}

dj_graph_node_t *dj_graph_graph_get_end_by_index(dj_graph_graph_t *graph, uint32_t index)
{
    SYSTEM_ASSERT(graph != NULL);
    if (index >= graph->end_count)
    {
        return NULL;
    }
    return graph->ends[index];
}

uint32_t dj_graph_graph_get_index_start(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    // Search the index of the start node
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *node = node_list_get(&graph->nodes, i);
        if (node == NULL)
        {
            continue;
        }
        if (node == graph->start)
        {
            return i;
        }
    }
    return 0;
}

uint32_t dj_graph_graph_get_index_end(dj_graph_graph_t *graph)
{
    SYSTEM_ASSERT(graph != NULL);
    // Search the index of the end node
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *node = node_list_get(&graph->nodes, i);
        if (node == NULL)
        {
            continue;
        }
        if (node == graph->end)
        {
            return i;
        }
    }
    return 0;
}

uint32_t dj_graph_graph_get_node_index(dj_graph_graph_t *graph, const dj_graph_node_t *node)
{
    SYSTEM_ASSERT(graph != NULL);
    SYSTEM_ASSERT(node != NULL);
    const dj_graph_node_t *base = node_list_get(&graph->nodes, 0);
    SYSTEM_ASSERT(base != NULL);
    return (uint32_t)(node - base);
}

bool dj_graph_node_is_reachable(dj_graph_graph_t *graph, point_t *start, dj_graph_node_t *node)
{
    SYSTEM_ASSERT(graph != NULL);
    SYSTEM_ASSERT(start != NULL);
    SYSTEM_ASSERT(node != NULL);
    // Check if the start and the node are the same
    if (point_compare(start, &node->pos))
    {
        // We consider that a node isn't reachable from itself
        return false;
    }
    // shearch a node with the same position as the start
    for (uint32_t i = 0; i < node_list_size(&graph->nodes); i++)
    {
        dj_graph_node_t *current_node = node_list_get(&graph->nodes, i);
        SYSTEM_ASSERT(current_node != NULL);
        if (point_compare(start, &current_node->pos))
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

void dj_graph_graph_print(dj_graph_graph_t *graph)
{
    // Print all the nodes
    // Print the number of nodes and links
    uint32_t nb_nodes = dj_graph_graph_get_num_nodes(graph);
    uint32_t nb_links = dj_graph_graph_get_num_links(graph);
    LOGD("Graph built with %d nodes and %d links", nb_nodes, nb_links);
    LOGD("Nodes:");
    for (uint32_t i = 0; i < nb_nodes; i++)
    {
        dj_graph_node_t *node = dj_graph_graph_get_node(graph, i);
        point_t pos = dj_graph_node_get_pos(node);
        LOGD("Node %d : (%f, %f) enabled = %d", i, pos.x, pos.y, node->enabled);
    }
    // Print all the links
    LOGD("Links:");
    for (uint32_t i = 0; i < nb_links; i++)
    {
        dj_graph_link_t *link = dj_graph_graph_get_link(graph, i);
        dj_graph_node_t *node1 = dj_graph_link_get_node1(link);
        dj_graph_node_t *node2 = dj_graph_link_get_node2(link);
        point_t pos1 = dj_graph_node_get_pos(node1);
        point_t pos2 = dj_graph_node_get_pos(node2);
        LOGD("Link %d : (%f, %f) --- (%f, %f) enabled = %d",
             i,
             pos1.x,
             pos1.y,
             pos2.x,
             pos2.y,
             link->enabled);
    }
}

/* ******************************************* Public callback functions declarations ************************************ */
