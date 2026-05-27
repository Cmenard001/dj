/**
 * @file dj_graph_rebuilder.c
 * @brief Rebuilder is used to rebuild the graph when the viewer moves
 * @see dj_graph_builder.c
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_rebuilder.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_rebuilder.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "utils/dj/dj_config.h"
#include "utils/dj/dj_graph/dj_graph_graph.h"
#include "utils/dj/dj_graph/dj_graph_link.h"
#include "utils/dj/dj_graph/dj_graph_node.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_node_ops.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_importer.h"
#include <stdbool.h>
#include <stdint.h>

LOG_REGISTER("utils/dj/graph_rebuilder");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static void copy_prebuilt_graph(dj_graph_builder_t *builder, dj_graph_graph_t *prebuilt_graph);
static void enable_all_links_and_nodes(dj_graph_builder_t *builder);
static void disable_links_on_solution(dj_graph_builder_t *builder,
                                      static_obstacles_list_t *obstacles,
                                      dj_viewer_status_t *viewer_status);
static void add_viewer_as_node(dj_graph_builder_t *builder,
                               dj_viewer_status_t *viewer_status,
                               static_obstacles_list_t *obstacles);
/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Function to copy the prebuilt graph to the builder's graph
 * @param builder Builder of the graph to rebuild
 * @param prebuilt_graph Prebuilt graph to copy
 */
static void copy_prebuilt_graph(dj_graph_builder_t *builder, dj_graph_graph_t *prebuilt_graph)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(prebuilt_graph != NULL);

    // Clear the graph
    dj_graph_graph_clear(&builder->graph);

    // Copy all nodes of the prebuilt graph
    uint32_t nb_nodes = dj_graph_graph_get_num_nodes(prebuilt_graph);
    for (uint32_t node_index = 0; node_index < nb_nodes; node_index++)
    {
        // Get the node
        dj_graph_node_t *node = dj_graph_graph_get_node(prebuilt_graph, node_index);
        if (node == NULL)
        {
            continue;
        }
        // Add the node to the graph
        dj_graph_node_t *new_node =
            dj_graph_graph_add_node(&builder->graph, node, DJ_GRAPH_NODE_TYPE_NORMAL);
        if (new_node == NULL)
        {
            continue;
        }
        new_node->enabled = node->enabled;
    }

    // Copy all links of the prebuilt graph using index mapping (O(L) instead of O(L*N))
    uint32_t nb_links = dj_graph_graph_get_num_links(prebuilt_graph);
    for (uint32_t link_index = 0; link_index < nb_links; link_index++)
    {
        // Get the link
        dj_graph_link_t *prebuild_link = dj_graph_graph_get_link(prebuilt_graph, link_index);
        if (prebuild_link == NULL)
        {
            continue;
        }
        // Get the nodes of the link
        dj_graph_node_t *prebuild_node1 = dj_graph_link_get_node1(prebuild_link);
        if (prebuild_node1 == NULL)
        {
            continue;
        }
        dj_graph_node_t *prebuild_node2 = dj_graph_link_get_node2(prebuild_link);
        if (prebuild_node2 == NULL)
        {
            continue;
        }
        // Use index in prebuilt graph to directly access new node at same index
        uint32_t idx1 = dj_graph_graph_get_node_index(prebuilt_graph, prebuild_node1);
        uint32_t idx2 = dj_graph_graph_get_node_index(prebuilt_graph, prebuild_node2);
        dj_graph_node_t *new_node1 = dj_graph_graph_get_node(&builder->graph, idx1);
        if (new_node1 == NULL)
        {
            continue;
        }
        dj_graph_node_t *new_node2 = dj_graph_graph_get_node(&builder->graph, idx2);
        if (new_node2 == NULL)
        {
            continue;
        }
        // Add the link to the graph
        dj_graph_link_t *new_link =
            dj_graph_graph_add_link(&builder->graph, new_node1, new_node2, true);
        if (new_link == NULL)
        {
            continue;
        }
        new_link->enabled = prebuild_link->enabled;
    }
}

/**
 * @brief Function to enable all the links and nodes of the graph
 * @param builder The graph builder
 */
static void enable_all_links_and_nodes(dj_graph_builder_t *builder)
{
    SYSTEM_ASSERT(builder != NULL);

    dj_mark_start_time(REBUILD_ENABLE_ALL);

    // Enable all the nodes
    uint32_t first_rebuilt_node_index = (uint32_t)(builder->graph.last_prebuilt_link + 1);
    uint32_t nb_nodes = dj_graph_graph_get_num_nodes(&builder->graph);
    for (uint32_t node_index = first_rebuilt_node_index; node_index < nb_nodes; node_index++)
    {
        dj_graph_node_t *node = dj_graph_graph_get_node(&builder->graph, node_index);
        if (node == NULL)
        {
            continue;
        }
        dj_graph_node_enable(node, true);
    }

    // Enable all the links
    uint32_t first_rebuilt_link_index = (uint32_t)(builder->graph.last_prebuilt_link + 1);
    uint32_t nb_links = dj_graph_graph_get_num_links(&builder->graph);
    for (uint32_t link_index = first_rebuilt_link_index; link_index < nb_links; link_index++)
    {
        dj_graph_link_t *link = dj_graph_graph_get_link(&builder->graph, link_index);
        if (link == NULL)
        {
            continue;
        }
        dj_graph_link_enable(link, true);
    }

    dj_mark_end_time(REBUILD_ENABLE_ALL);
}

/**
 * @brief Disable all the links which are on the solution of dynamic obstacles
 * @warning A link connected to the viewer should not be disabled
 *
 * @param builder The graph builder
 * @param obstacles The list of obstacles
 * @param viewer_status The viewer status
 */
static void disable_links_on_solution(dj_graph_builder_t *builder,
                                      static_obstacles_list_t *obstacles,
                                      dj_viewer_status_t *viewer_status)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(obstacles != NULL);
    SYSTEM_ASSERT(viewer_status != NULL);

    dj_mark_start_time(REBUILD_DISABLE_LINKS);

    // Get all the obstacles
    uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    for (uint32_t obstacle_index = 0; obstacle_index < nb_obstacles; obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        SYSTEM_ASSERT(obstacle != NULL);

        // Check if the obstacle is enabled and if it is generated by a dynamic obstacle
        if (!obstacle->is_enabled || obstacle->id != STATIC_OBSTACLE_UNKNOWN_ID)
        {
            continue;
        }

        // Rbuild the graph with the obstacle
        dj_mark_start_time(BUILD_WITH_OBSTACLE);
        dj_graph_builder_build_with_obstacle(
            builder, obstacle, viewer_status, false, !DJ_ENABLE_REBUILD_OPTIMIZATION);
        dj_mark_end_time(BUILD_WITH_OBSTACLE);
    }

    dj_mark_end_time(REBUILD_DISABLE_LINKS);
}

/**
 * @brief Function to add the viewer as a node in the graph (if it is not already present)
 * @note The new node is linked to all the other nodes of the graph
 *
 * @param builder The graph builder
 * @param viewer_status The viewer status
 * @param obstacles The list of obstacles to use to do verifications
 */
static void add_viewer_as_node(dj_graph_builder_t *builder,
                               dj_viewer_status_t *viewer_status,
                               static_obstacles_list_t *obstacles)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(viewer_status != NULL);
    SYSTEM_ASSERT(obstacles != NULL);

    dj_mark_start_time(REBUILD_ADD_VIEWER_AS_NODE);

    // Add the viewer status as a node in the graph if it is not already present
    dj_graph_node_t *viewer_node =
        dj_graph_graph_get_node_from_pos(&builder->graph, &viewer_status->position);
    if (viewer_node == NULL)
    {
        // Add the node to the graph
        dj_graph_node_t new_node;
        viewer_node =
            dj_graph_graph_add_node(&builder->graph, &new_node, DJ_GRAPH_NODE_TYPE_NORMAL);
        SYSTEM_ASSERT(viewer_node != NULL);
        dj_graph_node_init(viewer_node, &viewer_status->position);
    }

    // Link the viewer status with all the other nodes of the graph
    // This is nedded only if the viewer is not on an dynamic obstacle
    // So to optimize the calculation, we first check if the viewer is on an dynamic obstacle
    for (uint32_t obstacle_index = 0; obstacle_index < static_obstacles_list_size(obstacles);
         obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        SYSTEM_ASSERT(obstacle != NULL);

        // Check if the obstacle is enabled and if it is generated by a dynamic obstacle
        if (!obstacle->is_enabled || obstacle->id != STATIC_OBSTACLE_UNKNOWN_ID)
        {
            continue;
        }

        // Get the polygon of the obstacle
        dj_polygon_t *polygon = &obstacle->shape;

        // Check if the viewer is on the polygon
        point_t out_point = (point_t){-10000, -10000};
        polygon_t geometry_polygon;
        dj_polygon_to_geometry_polygon(polygon, &geometry_polygon);
        if (is_strictly_in_polygon(geometry_polygon.points,
                                   (uint8_t)geometry_polygon.nb_points,
                                   &viewer_node->pos,
                                   &out_point,
                                   NULL))
        {
            // The viewer is on a dynamic obstacle, so we want to force link it
#ifdef DJ_ENABLE_SUPER_LOGGING
            const bool debug_logs = true;
#else
            const bool debug_logs = false;
#endif
            dj_graph_builder_force_link_node(builder, viewer_node, obstacles, false, debug_logs);
            break;
        }
    }

    dj_mark_end_time(REBUILD_ADD_VIEWER_AS_NODE);
}

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_builder_rebuild(dj_graph_builder_t *builder,
                              dj_graph_graph_t *prebuilt_graph,
                              dj_viewer_status_t *viewer_status)
{
    if (builder == NULL || prebuilt_graph == NULL)
    {
        return;
    }
    dj_mark_start_time(REBUILD);

    // Fast path: when all dynamic obstacles have null kinematics their positions are
    // viewer-independent. After the first full rebuild, only the viewer node changes, so
    // skip obstacle recomputation and link disabling entirely.
    if (builder->null_kinematics_detected && builder->null_kinematics_first_rebuild_done)
    {
        dj_mark_end_time(REBUILD);
        return;
    }

    // Clear the graph (only nodes and links generated by dynamic obstacles)
    dj_graph_graph_clear_without_prebuilt(&builder->graph);
    // Enable all the nodes and links
    enable_all_links_and_nodes(builder);
    // Copy all the nodes wich are not on solution of dynamic obstacles
    static_obstacles_list_t *obstacles =
        dj_obstacle_manager_get_all_obstacles(&builder->obstacle_manager, viewer_status);
#ifdef DJ_ENABLE_SUPER_LOGGING
    uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    uint32_t nb_nodes_prebuilt_graph = dj_graph_graph_get_num_nodes(prebuilt_graph);
    point_t point_of_view_to_print =
        viewer_status == NULL ? (point_t){-10000, -10000} : viewer_status->position;
    LOGD("Start rebuilding the graph from position (%f, %f)",
         point_of_view_to_print.x,
         point_of_view_to_print.y);
    LOGD("Number of obstacles: %d", (int)nb_obstacles);
    for (uint32_t obstacle_index = 0; obstacle_index < nb_obstacles; obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        // Get the polygon of the obstacle
        dj_polygon_t *polygon = &obstacle->shape;
        LOGD("Obstacle %d : %d points, enabled = %d",
             (int)obstacle_index,
             (int)dj_polygon_get_nb_points(polygon),
             (int)obstacle->is_enabled);
        for (uint32_t point_index = 0; point_index < dj_polygon_get_nb_points(polygon);
             point_index++)
        {
            point_t *point = dj_polygon_get_point(polygon, (uint16_t)point_index);
            LOGD("Point %d : (%f, %f)", (int)point_index, point->x, point->y);
        }
    }
    LOGD("Number of nodes in the prebuilt graph: %d", (int)nb_nodes_prebuilt_graph);
#endif

    // Disable all the links which are on the solution of dynamic obstacles
    disable_links_on_solution(builder, obstacles, viewer_status);

    // Add the viewer as a node in the graph
    add_viewer_as_node(builder, viewer_status, obstacles);

    // Mark first full rebuild as done for the null-kinematics fast path
    if (builder->null_kinematics_detected)
    {
        builder->null_kinematics_first_rebuild_done = true;
    }

    dj_mark_end_time(REBUILD);

// Print the graph
#ifdef DJ_ENABLE_SUPER_LOGGING
    dj_graph_graph_print(&builder->graph);
#endif
}

void dj_graph_builder_first_rebuild(dj_graph_builder_t *builder,
                                    dj_graph_graph_t *prebuilt_graph,
                                    const point_t *start,
                                    const point_t *ends,
                                    uint32_t end_count)
{
    if (builder == NULL || prebuilt_graph == NULL)
    {
        return;
    }
    SYSTEM_ASSERT(ends != NULL);
    SYSTEM_ASSERT(end_count > 0 && end_count <= DJ_ENGINE_MAX_DESTINATIONS);

    dj_mark_start_time(FIRST_REBUILD);
    // Copy the prebuilt graph
    dj_mark_start_time(COPY_PREBUILT_GRAPH);
    copy_prebuilt_graph(builder, prebuilt_graph);
    dj_mark_end_time(COPY_PREBUILT_GRAPH);

    // Add the start node
    dj_graph_node_t start_node;
    dj_graph_node_init(&start_node, start);
    dj_graph_node_t *start_node_in_graph =
        dj_graph_graph_add_node(&builder->graph, &start_node, DJ_GRAPH_NODE_TYPE_START);
    SYSTEM_ASSERT(start_node_in_graph != NULL);

    // Add all end nodes
    for (uint32_t i = 0; i < end_count; i++)
    {
        dj_graph_node_t end_node;
        dj_graph_node_init(&end_node, &ends[i]);
        dj_graph_node_t *end_node_in_graph =
            dj_graph_graph_add_node(&builder->graph, &end_node, DJ_GRAPH_NODE_TYPE_END);
        SYSTEM_ASSERT(end_node_in_graph != NULL);
    }

    // Link the start and all end nodes
    static_obstacles_list_t *obstacles =
        dj_obstacle_manager_get_all_obstacles(&builder->obstacle_manager, NULL);
    dj_mark_start_time(FORCE_LINK_START_END);

#ifdef DJ_ENABLE_SUPER_LOGGING
    const bool debug_logs_start_end = true;
#else
    const bool debug_logs_start_end = false;
#endif

    dj_graph_builder_force_link_node(
        builder, start_node_in_graph, obstacles, true, debug_logs_start_end);
    for (uint32_t i = 0; i < end_count; i++)
    {
        dj_graph_node_t *end_node_in_graph = dj_graph_graph_get_end_by_index(&builder->graph, i);
        SYSTEM_ASSERT(end_node_in_graph != NULL);
        dj_graph_builder_force_link_node(
            builder, end_node_in_graph, obstacles, true, debug_logs_start_end);
    }
    dj_mark_end_time(FORCE_LINK_START_END);

    // Detect null kinematics: if all dynamic obstacles have null speed and acceleration,
    // dj_graph_builder_rebuild can skip obstacle recomputation on every subsequent call.
    builder->null_kinematics_detected = dj_obstacle_importer_all_dynamic_null_kinematics(
        builder->obstacle_manager.obstacle_importer);
    builder->null_kinematics_first_rebuild_done = false;
    builder->obstacle_manager.all_dynamic_null_kinematics = builder->null_kinematics_detected;

    dj_mark_end_time(FIRST_REBUILD);

// Print the graph
#ifdef DJ_ENABLE_SUPER_LOGGING
    dj_graph_graph_print(&builder->graph);
#endif
}

/* ******************************************* Public callback functions definitions *************************************** */
