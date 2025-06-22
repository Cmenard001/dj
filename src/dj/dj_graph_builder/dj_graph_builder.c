/**
 * @file dj_graph_builder.h
 * @brief Graph builder is used to build the graph from the obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_builder.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_graph_builder.h"
#include "../dj_graph/dj_graph_graph.h"
#include "../dj_logs/dj_time_marker.h"
#include "../dj_obstacle/dj_obstacle_manager.h"
#include "../dj_obstacle/dj_polygon.h"
#include "../dj_viewer/dj_viewer_status.h"

#include <math.h>
#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static bool dj_builder_pos_on_an_obstacle(dj_graph_builder_t *builder,
                                          GEOMETRY_point_t pos,
                                          static_obstacles_list_t *obstacles);
static void dj_graph_builder_build_nodes(dj_graph_builder_t *builder, dj_viewer_status_t *viewer_status);
static void dj_graph_builder_build_all_links(dj_graph_builder_t *builder,
                                             dj_viewer_status_t *viewer_status,
                                             bool is_prebuilt);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Function to check if a position is on an obstacle
 *
 * @param builder Builder to check the position
 * @param pos Position to check
 * @param obstacles List of obstacles to check
 * @return true if the position is on an obstacle
 * @return false if the position is not on an obstacle
 */
static bool dj_builder_pos_on_an_obstacle(dj_graph_builder_t *builder,
                                          GEOMETRY_point_t pos,
                                          static_obstacles_list_t *obstacles)
{
    dj_control_non_null(builder, false);
    // Check if the position is on an obstacle
    if (obstacles != NULL)
    {
        for (uint32_t i = 0; i < static_obstacles_list_size(obstacles); i++)
        {
            dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
            dj_control_non_null(obstacle, false);
            if (!obstacle->m_is_enabled)
            {
                continue;
            }
            GEOMETRY_point_t out_point = (GEOMETRY_point_t){-10000, -10000};
            dj_polygon_t *polygon = &obstacle->m_shape;
            if (GEOMETRY_is_strictly_in_polygon(polygon->points, dj_polygon_get_nb_points(polygon), pos, out_point, NULL))
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Function to build the nodes of the graph
 *
 * @param builder Builder to build the nodes
 * @param viewer_status The status of the viewer (used to build dynamic polygons) (NULL if you want to a graph without dynamic obstacles)
 */
static void dj_graph_builder_build_nodes(dj_graph_builder_t *builder, dj_viewer_status_t *viewer_status)
{
    dj_control_non_null(builder, );
    // Get all the obstacles
    static_obstacles_list_t *obstacles = dj_obstacle_manager_get_all_obstacles(&builder->m_obstacle_manager, viewer_status);
    dj_control_non_null(obstacles, );
    uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    for (uint32_t obstacle_index = 0; obstacle_index < nb_obstacles; obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        dj_control_non_null(obstacle, );

        // Check if the obstacle is enabled
        if (!obstacle->m_is_enabled)
        {
            continue;
        }

        // Get the polygon of the obstacle
        dj_polygon_t *current_polygon = &obstacle->m_shape;

        // Add every point of the polygon as a node
        for (uint32_t point_index = 0; point_index < dj_polygon_get_nb_points(current_polygon); point_index++)
        {
            // Get the point
            GEOMETRY_point_t *point = dj_polygon_get_point(current_polygon, point_index);
            dj_control_non_null(point, );

            // Add the point as a node
            dj_graph_node_t node;
            dj_graph_node_init(&node, point);
            dj_graph_builder_add_node_with_verif(builder, &node, DJ_GRAPH_NODE_TYPE_PREBUILT, viewer_status);
            dj_graph_node_deinit(&node);
        }
    }
}

/**
 * @brief Function to build the links of the graph
 *
 * @param builder Builder to build the links
 * @param viewer_status The status of the viewer (used to build dynamic polygons) (NULL if you want to a graph without dynamic obstacles)
 */
static void dj_graph_builder_build_all_links(dj_graph_builder_t *builder,
                                             dj_viewer_status_t *viewer_status,
                                             bool is_prebuilt)
{
    dj_control_non_null(builder, );
    // Links all nodes which do not intersect with an obstacle

    // Get all the obstacles
    static_obstacles_list_t *obstacles = dj_obstacle_manager_get_all_obstacles(&builder->m_obstacle_manager, viewer_status);
    dj_control_non_null(obstacles, );

    // Get all the nodes
    for (uint32_t node_index = 0; node_index < dj_graph_graph_get_num_nodes(&builder->m_graph); node_index++)
    {
        // Get the node
        dj_graph_node_t *node = dj_graph_graph_get_node(&builder->m_graph, node_index);
        dj_control_non_null(node, );

        // Link the node
        dj_graph_builder_link_node(builder, node, obstacles, is_prebuilt, false);
    }
}

void dj_graph_builder_init(dj_graph_builder_t *builder)
{
    dj_control_non_null(builder, );

    // Initialize the graph
    dj_graph_graph_init(&builder->m_graph);

    // Initialize the obstacle manager
    dj_obstacle_manager_init(&builder->m_obstacle_manager);
}

void dj_graph_builder_deinit(dj_graph_builder_t *builder)
{
    dj_control_non_null(builder, );

    // Deinitialize the graph
    dj_graph_graph_deinit(&builder->m_graph);

    // Deinitialize the obstacle manager
    dj_obstacle_manager_deinit(&builder->m_obstacle_manager);
}

void dj_graph_builder_link_node(
    dj_graph_builder_t *builder, dj_graph_node_t *node, static_obstacles_list_t *obstacles, bool is_prebuilt, bool verbose)
{
    dj_control_non_null(builder, );
    dj_control_non_null(node, );
    dj_control_non_null(obstacles, );

    // Check if the node is enabled
    if (!node->m_enabled)
    {
        return; // We do not want to link a disabled node
    }

    // Get the position of the node
    GEOMETRY_point_t node_pos = dj_graph_node_get_pos(node);

    // Get all other nodes
    uint32_t nb_nodes = dj_graph_graph_get_num_nodes(&builder->m_graph);
    uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    for (uint32_t other_node_index = 0; other_node_index < nb_nodes; other_node_index++)
    {
        // Get the other node
        dj_graph_node_t *other_node = dj_graph_graph_get_node(&builder->m_graph, other_node_index);
        dj_control_non_null(other_node, );

        // Check if the other node is enabled
        if (!other_node->m_enabled)
        {
            continue; // We do not want to link a disabled node
        }

        // Check if their is already a link between the two nodes
        // Or if the node is the same as the other node
        if (dj_graph_graph_get_link_from_nodes(&builder->m_graph, node, other_node) != NULL)
        {
            continue;
        }
        if (dj_graph_node_compare(node, other_node))
        {
            continue;
        }

        // Get the position of the other node
        GEOMETRY_point_t other_node_pos = dj_graph_node_get_pos(other_node);

        // Convert the link to a segment
        GEOMETRY_segment_t link_segment;
        link_segment.a = node_pos;
        link_segment.b = other_node_pos;

        // Check if the link intersects with an obstacle
        bool intersects = false;
        for (uint32_t obstacle_index = 0; obstacle_index < nb_obstacles; obstacle_index++)
        {
            // Get the obstacle as a static obstacle
            dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
            dj_control_non_null(obstacle, );

            // Check if the obstacle is enabled
            if (!obstacle->m_is_enabled)
            {
                continue;
            }

            // Get the polygon of the obstacle
            dj_polygon_t *polygon = &obstacle->m_shape;

            // Check if the segment intersects with the polygon
            GEOMETRY_polygon_t polygon_geometry;
            dj_polygon_to_geometry_polygon(polygon, &polygon_geometry);
            if (GEOMETRY_polygon_strictly_intersects_segment(&polygon_geometry, link_segment))
            {
                intersects = true;
            }
            else
            {
                // Special case : We want to check if the link is a diagonal of the polygon
                // For that we take a point just after the start of the segment and a point just before the end of the segment
                // Then we check if that point is on the polygon
                int16_t divisor
                    = MIN(absolute(link_segment.a.x - link_segment.b.x), absolute(link_segment.a.y - link_segment.b.y)) - 1;
                divisor = MAX(divisor, 2); // Avoid division by zero and too small values

                GEOMETRY_point_t just_after_start
                    = {.x = link_segment.a.x + (link_segment.b.x - link_segment.a.x) / divisor,
                       .y = link_segment.a.y + (link_segment.b.y - link_segment.a.y) / divisor};
                GEOMETRY_point_t just_before_end = {.x = link_segment.b.x - (link_segment.b.x - link_segment.a.x) / divisor,
                                                    .y = link_segment.b.y - (link_segment.b.y - link_segment.a.y) / divisor};

                GEOMETRY_point_t out_point = (GEOMETRY_point_t){-10000, -10000};
                if (GEOMETRY_is_strictly_in_polygon(
                        polygon_geometry.points, polygon_geometry.nb_points, just_after_start, out_point, NULL)
                    || GEOMETRY_is_strictly_in_polygon(
                        polygon_geometry.points, polygon_geometry.nb_points, just_before_end, out_point, NULL))
                {
                    intersects = true;
                }
            }
            // Special case 2 : We want to link the edges of polygons
            int16_t node_index = GEOMETRY_polygon_contains_point(&polygon_geometry, node_pos);
            int16_t other_node_index = GEOMETRY_polygon_contains_point(&polygon_geometry, other_node_pos);
            // Check if the extremities of the link are on the polygon
            if (node_index != -1 && other_node_index != -1)
            {
                int16_t diff = node_index - other_node_index;
                if (other_node_index == 0 && node_index == dj_polygon_get_nb_points(polygon) - 1)
                {
                    diff = 1;
                }
                else if (node_index == 0 && other_node_index == dj_polygon_get_nb_points(polygon) - 1)
                {
                    diff = -1;
                }
                if (diff == 1 || diff == -1)
                {
                    // Special case 1 if the link is an edge of the polygon
                    intersects = false; // We want to create a link on the edge of the polygon
                }
            }
            if (intersects)
            {
                break; // We can skip the rest of the obstacles because the link intersects with one of them
            }
        }
        // If the link does not intersect with any obstacle add it to the graph
        if (!intersects)
        {
            // Add the link to the graph
            dj_graph_link_t *link = dj_graph_graph_add_link(&builder->m_graph, node, other_node, is_prebuilt);
            dj_control_non_null(link, );
            link->m_enabled = node->m_enabled && other_node->m_enabled;
            if (verbose)
            {
                dj_debug_printf("Link node (%d, %d) with (%d, %d), enabled = %d\n",
                                node_pos.x,
                                node_pos.y,
                                other_node_pos.x,
                                other_node_pos.y,
                                link->m_enabled);
            }
        }
    }
}

void dj_graph_builder_force_link_node(
    dj_graph_builder_t *builder, dj_graph_node_t *node, static_obstacles_list_t *obstacles, bool is_prebuilt, bool verbose)
{
    dj_control_non_null(builder, );
    dj_control_non_null(node, );
    dj_control_non_null(obstacles, );

    if (verbose)
    {
        dj_debug_printf("Try to force link node (%d, %d)\n", node->m_pos.x, node->m_pos.y);
    }

    // Check if the node is on an obstacle
    if (!dj_builder_pos_on_an_obstacle(builder, node->m_pos, obstacles))
    {
        if (verbose)
        {
            dj_debug_printf("Node (%d, %d) is not on an obstacle, make a normal linking\n", node->m_pos.x, node->m_pos.y);
        }
        // Make a normal linking of the node
        dj_graph_builder_link_node(builder, node, obstacles, is_prebuilt, verbose);
        return;
    }

    // Get all the obstacles
    uint32_t closest_distance = UINT32_MAX;
    GEOMETRY_point_t extraction_point;
    bool found = false;
    uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    for (uint32_t obstacle_index = 0; obstacle_index < nb_obstacles; obstacle_index++)
    {
        // Get the obstacle as a static obstacle
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, obstacle_index);
        dj_control_non_null(obstacle, );

        // Check if the obstacle is enabled
        if (!obstacle->m_is_enabled)
        {
            continue;
        }

        // Get the polygon of the obstacle
        dj_polygon_t *polygon = &obstacle->m_shape;

        // Get all the submit of the polygon
        for (uint32_t point_index = 0; point_index < dj_polygon_get_nb_points(polygon); point_index++)
        {
            // Get the point
            GEOMETRY_point_t *point = dj_polygon_get_point(polygon, point_index);
            dj_control_non_null(point, );

            // Check if the point is closer than the closest point
            uint32_t distance = GEOMETRY_distance_square(node->m_pos, *point);
            if (distance < closest_distance)
            {

                // Check if the point is a valid extraction point
                // To be valid, the point must be on the playground and not on an obstacle
                if (dj_graph_graph_is_node_on_playground(point))
                {
                    if (!dj_builder_pos_on_an_obstacle(builder, *point, obstacles))
                    {
                        closest_distance = distance;
                        extraction_point = *point;
                        found = true;
                    }
                }
            }
        }

        // Do the same for ortogonal projections of the node on the edges of the polygon
        for (uint32_t point_index = 0; point_index < dj_polygon_get_nb_points(polygon); point_index++)
        {
            // Get the point
            GEOMETRY_point_t *point1 = dj_polygon_get_point(polygon, point_index);
            dj_control_non_null(point1, );
            GEOMETRY_point_t *point2 = dj_polygon_get_point(polygon, (point_index + 1) % dj_polygon_get_nb_points(polygon));
            dj_control_non_null(point2, );

            // Get the projection of the node on the edge
            GEOMETRY_segment_t segment = {*point1, *point2};
            GEOMETRY_point_t projection = GEOMETRY_proj_on_line(segment, node->m_pos);

            // Check if the projection is on the segment
            if (projection.x >= MIN(point1->x, point2->x) && projection.x <= MAX(point1->x, point2->x)
                && projection.y >= MIN(point1->y, point2->y) && projection.y <= MAX(point1->y, point2->y))
            {
                // Check if the projection is closer than the closest point
                uint32_t distance = GEOMETRY_distance_square(node->m_pos, projection);
                if (distance < closest_distance)
                {
                    // Check if the projection is a valid extraction point
                    // To be valid, the point must be on the playground and not on an obstacle
                    if (dj_graph_graph_is_node_on_playground(&projection))
                    {
                        if (!dj_builder_pos_on_an_obstacle(builder, projection, obstacles))
                        {
                            closest_distance = distance;
                            extraction_point = projection;
                            found = true;
                        }
                    }
                }
            }
        }
    }

    // Do the same for the intersection of each polygon edege with each other polygon edge
    // We only do that if any point has been found because it is more expensive
    if (!found)
    {
        for (uint32_t obstacle_index1 = 0; obstacle_index1 < nb_obstacles; obstacle_index1++)
        {
            // Get the first obstacle as a static obstacle
            dj_obstacle_static_t *obstacle1 = static_obstacles_list_get(obstacles, obstacle_index1);
            dj_control_non_null(obstacle1, );

            // Check if the obstacle is enabled
            if (!obstacle1->m_is_enabled)
            {
                continue;
            }

            // Get the polygon of the first obstacle
            dj_polygon_t *polygon1 = &obstacle1->m_shape;

            for (uint32_t point_index1 = 0; point_index1 < dj_polygon_get_nb_points(polygon1); point_index1++)
            {
                // Get the first point of the edge
                GEOMETRY_point_t *point11 = dj_polygon_get_point(polygon1, point_index1);
                dj_control_non_null(point11, );
                GEOMETRY_point_t *point12
                    = dj_polygon_get_point(polygon1, (point_index1 + 1) % dj_polygon_get_nb_points(polygon1));
                dj_control_non_null(point12, );

                // Create the segment of the first edge
                GEOMETRY_segment_t segment1 = {*point11, *point12};

                for (uint32_t obstacle_index2 = obstacle_index1 + 1; obstacle_index2 < nb_obstacles; obstacle_index2++)
                {
                    // Get the second obstacle as a static obstacle
                    dj_obstacle_static_t *obstacle2 = static_obstacles_list_get(obstacles, obstacle_index2);
                    dj_control_non_null(obstacle2, );

                    // Check if the obstacle is enabled
                    if (!obstacle2->m_is_enabled)
                    {
                        continue;
                    }

                    // Get the polygon of the second obstacle
                    dj_polygon_t *polygon2 = &obstacle2->m_shape;

                    for (uint32_t point_index2 = 0; point_index2 < dj_polygon_get_nb_points(polygon2); point_index2++)
                    {
                        // Get the second point of the edge
                        GEOMETRY_point_t *point21 = dj_polygon_get_point(polygon2, point_index2);
                        dj_control_non_null(point21, );
                        GEOMETRY_point_t *point22
                            = dj_polygon_get_point(polygon2, (point_index2 + 1) % dj_polygon_get_nb_points(polygon2));
                        dj_control_non_null(point22, );
                        // Create the segment of the second edge
                        GEOMETRY_segment_t segment2 = {*point21, *point22};
                        // Check if the segments intersect
                        GEOMETRY_point_t intersection;
                        if (GEOMETRY_segments_get_intersection(segment1, segment2, &intersection))
                        {
                            // Check if the intersection is closer than the closest point
                            uint32_t distance = GEOMETRY_distance_square(node->m_pos, intersection);
                            if (distance < closest_distance)
                            {
                                // Check if the intersection is a valid extraction point
                                // To be valid, the point must be on the playground and not on an obstacle
                                if (dj_graph_graph_is_node_on_playground(&intersection))
                                {
                                    if (!dj_builder_pos_on_an_obstacle(builder, intersection, obstacles))
                                    {
                                        closest_distance = distance;
                                        extraction_point = intersection;
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (found)
    {
        // Add the link to the graph
        dj_graph_node_t node_extraction;
        dj_graph_node_init(&node_extraction, &extraction_point);
        dj_graph_node_t *new_node = dj_graph_graph_add_node(
            &builder->m_graph, &node_extraction, is_prebuilt ? DJ_GRAPH_NODE_TYPE_PREBUILT : DJ_GRAPH_NODE_TYPE_NORMAL);
        dj_graph_node_deinit(&node_extraction);
        dj_control_non_null(new_node, );
        // Add the link to the graph
        dj_graph_link_t *link = dj_graph_graph_add_link(&builder->m_graph, node, new_node, is_prebuilt);
        dj_control_non_null(link, );
        if (verbose)
        {
            dj_debug_printf("Force link node (%d, %d) with (%d, %d), enabled = %d\n",
                            node->m_pos.x,
                            node->m_pos.y,
                            extraction_point.x,
                            extraction_point.y,
                            link->m_enabled);
        }

        // Make a normal linking of the new node
        dj_graph_builder_link_node(builder, new_node, obstacles, is_prebuilt, verbose);
    }
    else
    {
        error_printf("No valid extraction point found for node (%d, %d)\n", node->m_pos.x, node->m_pos.y);
    }
}

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_builder_build_all(dj_graph_builder_t *builder, dj_viewer_status_t *viewer_status, bool is_prebuilt)
{
    // Build the nodes
    dj_mark_start_time(DJ_MARK_BUILD_GRAPH_NODES);
    dj_graph_builder_build_nodes(builder, viewer_status);
    dj_mark_end_time(DJ_MARK_BUILD_GRAPH_NODES);

    // Build the links
    dj_mark_start_time(DJ_MARK_BUILD_GRAPH_LINKS);
    dj_graph_builder_build_all_links(builder, viewer_status, is_prebuilt);
    dj_mark_end_time(DJ_MARK_BUILD_GRAPH_LINKS);

#if DJ_ENABLE_BUILD_GRAPH_DEBUG_LOGS
    // Print the graph
    dj_graph_graph_print(&builder->m_graph);
#endif
}

dj_graph_node_t *dj_graph_builder_add_node_with_verif(dj_graph_builder_t *builder,
                                                      dj_graph_node_t *node,
                                                      dj_graph_node_type_t type,
                                                      dj_viewer_status_t *viewer_status)
{
    dj_control_non_null(builder, NULL);
    dj_control_non_null(node, NULL);

    // Check if the node is on the playground
    if (!dj_graph_graph_is_node_on_playground(&node->m_pos))
    {
        return NULL;
    }
    // Check if the node is already in the graph
    for (uint32_t i = 0; i < dj_graph_graph_get_num_nodes(&builder->m_graph); i++)
    {
        dj_graph_node_t *current_node = dj_graph_graph_get_node(&builder->m_graph, i);
        if (dj_graph_node_compare(node, current_node))
        {
            return NULL;
        }
    }
    // Check if the node is not strictly on an obstacle
    static_obstacles_list_t *obstacles = dj_obstacle_manager_get_all_obstacles(&builder->m_obstacle_manager, viewer_status);
    if (dj_builder_pos_on_an_obstacle(builder, node->m_pos, obstacles))
    {
        return NULL;
    }
    return dj_graph_graph_add_node(&builder->m_graph, node, type);
}

void dj_graph_builder_build_with_obstacle(dj_graph_builder_t *builder,
                                          dj_obstacle_static_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          bool is_prebuilt,
                                          bool add_obstacle_as_node)
{
    dj_control_non_null(builder, );
    dj_control_non_null(obstacle, );

    dj_debug_printf("Rebuild graph with an new obstacle\n");

    dj_mark_reset_all();

    // Disable all the links of the graph on the shape of this obstacle
    GEOMETRY_polygon_t geometry_polygon;
    dj_polygon_to_geometry_polygon(&obstacle->m_shape, &geometry_polygon);

    // Get all the links
    uint32_t nb_links = dj_graph_graph_get_num_links(&builder->m_graph);
    for (uint32_t link_index = 0; link_index < nb_links; link_index++)
    {
        // Get the link
        dj_graph_link_t *link = dj_graph_graph_get_link(&builder->m_graph, link_index);
        dj_control_non_null(link, );

        // Get the nodes of the link
        dj_graph_node_t *node1 = dj_graph_link_get_node1(link);
        dj_graph_node_t *node2 = dj_graph_link_get_node2(link);
        dj_control_non_null(node1, );
        dj_control_non_null(node2, );

        // Check if one of the nodes is strictly on the obstacle or if the link intersects with the obstacle
        const GEOMETRY_point_t out_point = (GEOMETRY_point_t){-10000, -10000};
        if (GEOMETRY_is_strictly_in_polygon(
                geometry_polygon.points, geometry_polygon.nb_points, node1->m_pos, out_point, NULL)
            || GEOMETRY_is_strictly_in_polygon(
                geometry_polygon.points, geometry_polygon.nb_points, node2->m_pos, out_point, NULL)
            || GEOMETRY_polygon_strictly_intersects_segment(&geometry_polygon,
                                                            (GEOMETRY_segment_t){node1->m_pos, node2->m_pos}))
        {
            // Disable the link
            link->m_enabled = false;
        }
    }

    if (add_obstacle_as_node)
    {
        // Get the polygon of the obstacle
        dj_polygon_t *polygon = &obstacle->m_shape;

        // Get the list of obstacles
        static_obstacles_list_t *obstacles
            = dj_obstacle_manager_get_all_obstacles(&builder->m_obstacle_manager, viewer_status);

        /* Add all the points of polygons generated by dynamic obstacles as nodes in the graph and link them */
        for (uint32_t point_index = 0; point_index < dj_polygon_get_nb_points(polygon); point_index++)
        {
            // Get the point
            GEOMETRY_point_t *point = dj_polygon_get_point(polygon, point_index);

            // Add the node to the graph
            dj_graph_node_t node;
            dj_graph_node_init(&node, point);
            // Warning : The viewer status is NULL to avoid to consider the dynamic obstacles in verification
            dj_graph_node_t *new_node
                = dj_graph_builder_add_node_with_verif(builder, &node, DJ_GRAPH_NODE_TYPE_NORMAL, viewer_status);
            dj_graph_node_deinit(&node);
            if (new_node != NULL)
            {
                dj_graph_node_init(new_node, point);

                // Link the node with all the other nodes of the graph
                dj_graph_builder_link_node(builder, new_node, obstacles, is_prebuilt, DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS);
            }
        }
    }

#if DJ_ENABLE_BUILD_GRAPH_DEBUG_LOGS
    // Print the graph
    dj_graph_graph_print(&builder->m_graph);
#endif
    // Print the markers
    dj_print_all_durations();
}

void dj_graph_builder_build_without_obstacle(dj_graph_builder_t *builder,
                                             dj_obstacle_static_t *obstacle,
                                             dj_viewer_status_t *viewer_status,
                                             bool is_prebuilt)
{
    dj_control_non_null(builder, );
    dj_control_non_null(obstacle, );

    dj_debug_printf("Rebuild graph without an obstacle\n");

    dj_mark_reset_all();

    // Disable all the links of the graph that one of the nodes has been generated by the obstacle
    GEOMETRY_polygon_t geometry_polygon;
    dj_polygon_to_geometry_polygon(&obstacle->m_shape, &geometry_polygon);
    // Get all the links of the graph
    uint32_t nb_links = dj_graph_graph_get_num_links(&builder->m_graph);
    for (uint32_t link_index = 0; link_index < nb_links; link_index++)
    {
        // Get the link
        dj_graph_link_t *link = dj_graph_graph_get_link(&builder->m_graph, link_index);
        dj_control_non_null(link, );

        // Check if the link is enabled
        if (!link->m_enabled)
        {
            continue; // The link is already disabled
        }

        // Get the nodes of the link
        dj_graph_node_t *node1 = dj_graph_link_get_node1(link);
        dj_graph_node_t *node2 = dj_graph_link_get_node2(link);
        dj_control_non_null(node1, );
        dj_control_non_null(node2, );

        // Check if one of the nodes has been generated by the obstacle
        bool disable_link = false;
        if (GEOMETRY_polygon_contains_point(&geometry_polygon, node1->m_pos) >= 0)
        {
            node1->m_enabled = false;
            disable_link = true;
        }
        if (GEOMETRY_polygon_contains_point(&geometry_polygon, node2->m_pos) >= 0)
        {
            node2->m_enabled = false;
            disable_link = true;
        }
        if (disable_link)
        {
            // Disable the link
            link->m_enabled = false;
        }
    }

    // Rebuild the graph without clearing it
    // It will keek the previous links and node but will add some new ones
    dj_graph_builder_build_all(builder, viewer_status, is_prebuilt);

    // Print the markers
    dj_print_all_durations();
}

/* ******************************************* Public callback functions declarations ************************************ */
