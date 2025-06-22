/**
 * @file dj_graph_rebuilder.c
 * @brief Rebuilder is used to rebuild the graph when the viewer moves
 * @see dj_graph_builder.c
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_rebuilder.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_graph_rebuilder.h"
#include "../dj_config.h"
#include "../dj_graph/dj_graph_graph.h"
#include "../dj_graph/dj_graph_link.h"
#include "../dj_graph/dj_graph_node.h"
#include "../dj_logs/dj_time_marker.h"
#include "../dj_obstacle/dj_polygon.h"

#include <stdbool.h>
#include <stdint.h>

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
 *
 * @param builder Builder of the graph to rebuild
 * @param prebuilt_graph Prebuilt graph to copy
 */
static void copy_prebuilt_graph(dj_graph_builder_t *builder, dj_graph_graph_t *prebuilt_graph)
{
    dj_control_non_null(builder, );
    dj_control_non_null(prebuilt_graph, );

    // Clear the graph
    dj_graph_graph_clear(&builder->m_graph);

    // Copy all nodes of the prebuilt graph
    uint32_t nb_nodes = dj_graph_graph_get_num_nodes(prebuilt_graph);
    for (uint32_t node_index = 0; node_index < nb_nodes; node_index++)
    {
        // Get the node
        dj_graph_node_t *node = dj_graph_graph_get_node(prebuilt_graph, node_index);
        dj_control_non_null(node, );
        // Add the node to the graph
        dj_graph_node_t *new_node = dj_graph_graph_add_node(&builder->m_graph, node, DJ_GRAPH_NODE_TYPE_NORMAL);
        dj_control_non_null(new_node, );
        new_node->m_enabled = node->m_enabled;
    }

    // Copy all links of the prebuilt graph
    uint32_t nb_links = dj_graph_graph_get_num_links(prebuilt_graph);
    for (uint32_t link_index = 0; link_index < nb_links; link_index++)
    {
        // Get the link
        dj_graph_link_t *prebuild_link = dj_graph_graph_get_link(prebuilt_graph, link_index);
        dj_control_non_null(prebuild_link, );
        // Get the nodes of the link
        dj_graph_node_t *prebuild_node1 = dj_graph_link_get_node1(prebuild_link);
        dj_control_non_null(prebuild_node1, );
        dj_graph_node_t *prebuild_node2 = dj_graph_link_get_node2(prebuild_link);
        dj_control_non_null(prebuild_node2, );
        // Get the nodes in the new graph
        dj_graph_node_t *new_node1 = dj_graph_graph_get_node_from_pos(&builder->m_graph, &prebuild_node1->m_pos);
        dj_control_non_null(new_node1, );
        dj_graph_node_t *new_node2 = dj_graph_graph_get_node_from_pos(&builder->m_graph, &prebuild_node2->m_pos);
        dj_control_non_null(new_node2, );
        // Add the link to the graph
        dj_graph_link_t *new_link = dj_graph_graph_add_link(&builder->m_graph, new_node1, new_node2, true);
        dj_control_non_null(new_link, );
        new_link->m_enabled = prebuild_link->m_enabled;
    }
}

/**
 * @brief Function to enable all the links and nodes of the graph
 *
 * @param builder The graph builder
 */
static void enable_all_links_and_nodes(dj_graph_builder_t *builder)
{
    dj_control_non_null(builder, );

    dj_mark_start_time(DJ_MARK_REBUILD_ENABLE_ALL);

    // Enable all the nodes
    uint32_t first_rebuilt_node_index = builder->m_graph.last_prebuilt_link + 1;
    uint32_t nb_nodes = dj_graph_graph_get_num_nodes(&builder->m_graph);
    for (uint32_t node_index = first_rebuilt_node_index; node_index < nb_nodes; node_index++)
    {
        dj_graph_node_t *node = dj_graph_graph_get_node(&builder->m_graph, node_index);
        dj_control_non_null(node, );
        dj_graph_node_enable(node, true);
    }

    // Enable all the links
    uint32_t first_rebuilt_link_index = builder->m_graph.last_prebuilt_link + 1;
    uint32_t nb_links = dj_graph_graph_get_num_links(&builder->m_graph);
    for (uint32_t link_index = first_rebuilt_link_index; link_index < nb_links; link_index++)
    {
        dj_graph_link_t *link = dj_graph_graph_get_link(&builder->m_graph, link_index);
        dj_control_non_null(link, );
        dj_graph_link_enable(link, true);
    }

    dj_mark_end_time(DJ_MARK_REBUILD_ENABLE_ALL);
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
    dj_control_non_null(builder, );
    dj_control_non_null(obstacles, );
    dj_control_non_null(viewer_status, );

    dj_mark_start_time(DJ_MARK_REBUILD_DISABLE_LINKS);

    // Get all the obstacles
    uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    for (uint32_t obstacle_index = 0; obstacle_index < nb_obstacles; obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        dj_control_non_null(obstacle, );

        // Check if the obstacle is enabled and if it is generated by a dynamic obstacle
        if (!obstacle->m_is_enabled || obstacle->m_id != STATIC_OBSTACLE_UNKNOWN_ID)
        {
            continue;
        }

        // Rbuild the graph with the obstacle
        dj_graph_builder_build_with_obstacle(builder, obstacle, viewer_status, false, !DJ_ENABLE_REBUILD_OPTIMIZATION);
    }

    dj_mark_end_time(DJ_MARK_REBUILD_DISABLE_LINKS);
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
    dj_control_non_null(builder, );
    dj_control_non_null(viewer_status, );
    dj_control_non_null(obstacles, );

    dj_mark_start_time(DJ_MARK_REBUILD_ADD_VIEWER_AS_NODE);

    // Add the viewer status as a node in the graph if it is not already present
    dj_graph_node_t *viewer_node = dj_graph_graph_get_node_from_pos(&builder->m_graph, &viewer_status->m_position);
    if (viewer_node == NULL)
    {
        // Add the node to the graph
        dj_graph_node_t new_node;
        viewer_node = dj_graph_graph_add_node(&builder->m_graph, &new_node, DJ_GRAPH_NODE_TYPE_NORMAL);
        dj_control_non_null(viewer_node, );
        dj_graph_node_init(viewer_node, &viewer_status->m_position);
    }

    // Link the viewer status with all the other nodes of the graph
    // This is nedded only if the viewer is not on an dynamic obstacle
    // So to optimize the calculation, we first check if the viewer is on an dynamic obstacle
    for (uint32_t obstacle_index = 0; obstacle_index < static_obstacles_list_size(obstacles); obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        dj_control_non_null(obstacle, );

        // Check if the obstacle is enabled and if it is generated by a dynamic obstacle
        if (!obstacle->m_is_enabled || obstacle->m_id != STATIC_OBSTACLE_UNKNOWN_ID)
        {
            continue;
        }

        // Get the polygon of the obstacle
        dj_polygon_t *polygon = &obstacle->m_shape;

        // Check if the viewer is on the polygon
        GEOMETRY_point_t out_point = (GEOMETRY_point_t){-10000, -10000};
        GEOMETRY_polygon_t geometry_polygon;
        dj_polygon_to_geometry_polygon(polygon, &geometry_polygon);
        if (GEOMETRY_is_strictly_in_polygon(
                geometry_polygon.points, geometry_polygon.nb_points, viewer_node->m_pos, out_point, NULL))
        {
            // The viewer is on a dynamic obstacle, so we want to force link it
            dj_graph_builder_force_link_node(builder, viewer_node, obstacles, false, DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS);
            break;
        }
    }

    dj_mark_end_time(DJ_MARK_REBUILD_ADD_VIEWER_AS_NODE);
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
    dj_mark_start_time(DJ_MARK_REBUILD);

    // Clear the graph (only nodes and links generated by dynamic obstacles)
    dj_graph_graph_clear_without_prebuilt(&builder->m_graph);
    // Enable all the nodes and links
    enable_all_links_and_nodes(builder);
    // Copy all the nodes wich are not on solution of dynamic obstacles
    static_obstacles_list_t *obstacles = dj_obstacle_manager_get_all_obstacles(&builder->m_obstacle_manager, viewer_status);
#if DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS
    uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    uint32_t nb_nodes_prebuilt_graph = dj_graph_graph_get_num_nodes(prebuilt_graph);
    GEOMETRY_point_t point_of_view_to_print
        = viewer_status == NULL ? (GEOMETRY_point_t){-10000, -10000} : viewer_status->m_position;
    dj_debug_printf(
        "Start rebuilding the graph from position (%d, %d)\n", (int)point_of_view_to_print.x, (int)point_of_view_to_print.y);
    dj_debug_printf("Number of obstacles: %d\n", (int)nb_obstacles);
    for (uint32_t obstacle_index = 0; obstacle_index < nb_obstacles; obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        // Get the polygon of the obstacle
        dj_polygon_t *polygon = &obstacle->m_shape;
        dj_debug_printf("Obstacle %d : %d points, enabled = %d\n",
                        (int)obstacle_index,
                        (int)dj_polygon_get_nb_points(polygon),
                        (int)obstacle->m_is_enabled);
        for (uint32_t point_index = 0; point_index < dj_polygon_get_nb_points(polygon); point_index++)
        {
            GEOMETRY_point_t *point = dj_polygon_get_point(polygon, point_index);
            dj_debug_printf("Point %d : (%d, %d)\n", (int)point_index, (int)point->x, (int)point->y);
        }
    }
    dj_debug_printf("Number of nodes in the prebuilt graph: %d\n", (int)nb_nodes_prebuilt_graph);
#endif

    // Disable all the links which are on the solution of dynamic obstacles
    disable_links_on_solution(builder, obstacles, viewer_status);

    // Add the viewer as a node in the graph
    add_viewer_as_node(builder, viewer_status, obstacles);

    dj_mark_end_time(DJ_MARK_REBUILD);

    // Print the graph
#if DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS
    dj_graph_graph_print(&builder->m_graph);
#endif
}

void dj_graph_builder_first_rebuild(dj_graph_builder_t *builder,
                                    dj_graph_graph_t *prebuilt_graph,
                                    GEOMETRY_point_t start,
                                    GEOMETRY_point_t end)
{
    if (builder == NULL || prebuilt_graph == NULL)
    {
        return;
    }
    dj_mark_start_time(DJ_MARK_FIRST_REBUILD);
    // Copy the prebuilt graph
    copy_prebuilt_graph(builder, prebuilt_graph);

    // Add the start and end nodes
    dj_graph_node_t start_node;
    dj_graph_node_init(&start_node, &start);
    dj_graph_node_t *start_node_in_graph = dj_graph_graph_add_node(&builder->m_graph, &start_node, DJ_GRAPH_NODE_TYPE_START);
    dj_graph_node_deinit(&start_node);
    dj_control_non_null(start_node_in_graph, );

    dj_graph_node_t end_node;
    dj_graph_node_init(&end_node, &end);
    dj_graph_node_t *end_node_in_graph = dj_graph_graph_add_node(&builder->m_graph, &end_node, DJ_GRAPH_NODE_TYPE_END);
    dj_graph_node_deinit(&end_node);
    dj_control_non_null(end_node_in_graph, );

    // Link the start and end nodes
    static_obstacles_list_t *obstacles = dj_obstacle_manager_get_all_obstacles(&builder->m_obstacle_manager, NULL);
    dj_mark_start_time(DJ_MARK_FORCE_LINK_START_END);
    dj_graph_builder_force_link_node(builder, start_node_in_graph, obstacles, true, DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS);
    dj_graph_builder_force_link_node(builder, end_node_in_graph, obstacles, true, DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS);
    dj_mark_end_time(DJ_MARK_FORCE_LINK_START_END);

    dj_mark_end_time(DJ_MARK_FIRST_REBUILD);

    // Print the graph
#if DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS
    dj_graph_graph_print(&builder->m_graph);
#endif
}

/* ******************************************* Public callback functions declarations ************************************ */
