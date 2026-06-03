/**
 * @file dj_graph_builder_link_ops.c
 * @brief Link operations for graph building (build all links, handle obstacles)
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder_link_ops.h"
#include "system/log/log.h"
#include "utils/dj/dj_config.h"
#include "utils/dj/dj_graph/dj_graph_link.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_intersection.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_node_ops.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_precompute.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/maths/segment/segment.h"
#include <float.h>

LOG_REGISTER("utils/dj/link_ops");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions definitions ******************************************* */

void dj_graph_builder_build_all_links(dj_graph_builder_t *builder,
                                      dj_viewer_status_t *viewer_status,
                                      bool is_prebuilt)
{
    SYSTEM_ASSERT(builder != NULL);

    static_obstacles_list_t *obstacles =
        dj_obstacle_manager_get_all_obstacles(&builder->obstacle_manager, viewer_status);
    SYSTEM_ASSERT(obstacles != NULL);

    // Precompute obstacles once for all link checks — avoids recomputing for every node
    const uint32_t nb_obstacles =
        dj_graph_builder_precompute_obstacles(obstacles, builder->precomputed);

    const uint32_t nb_nodes = dj_graph_graph_get_num_nodes(&builder->graph);

    // Iterate only (i, j) with j > i: each pair is checked exactly once.
    // This avoids the O(N^4) cost of dj_graph_graph_get_link_from_nodes (O(L) linear scan)
    // being called O(N^2) times to detect already-created links.
    for (uint32_t i = 0; i < nb_nodes; i++)
    {
        dj_graph_node_t *node_i = dj_graph_graph_get_node(&builder->graph, i);
        if (node_i == NULL || !node_i->enabled)
        {
            continue;
        }

        const point_t pos_i = dj_graph_node_get_pos(node_i);

        for (uint32_t j = i + 1; j < nb_nodes; j++)
        {
            dj_graph_node_t *node_j = dj_graph_graph_get_node(&builder->graph, j);
            if (node_j == NULL || !node_j->enabled)
            {
                continue;
            }

            const point_t pos_j = dj_graph_node_get_pos(node_j);
            const segment_t link_segment = {.a = pos_i, .b = pos_j};

            bool intersects = false;
            for (uint32_t k = 0; k < nb_obstacles; k++)
            {
                if (!builder->precomputed[k].is_enabled)
                {
                    continue;
                }

                if (!dj_graph_builder_bbox_overlap(&link_segment, &builder->precomputed[k]))
                {
                    continue;
                }

                if (dj_graph_builder_segment_intersects_obstacle(
                        &link_segment, &pos_i, &pos_j, &builder->precomputed[k]))
                {
                    intersects = true;
                    break;
                }
            }

            if (!intersects)
            {
                dj_graph_link_t *link =
                    dj_graph_graph_add_link(&builder->graph, node_i, node_j, is_prebuilt);
                SYSTEM_ASSERT(link != NULL);
                link->enabled = node_i->enabled && node_j->enabled;
            }
        }
    }
}

void dj_graph_builder_disable_intersecting_links(dj_graph_builder_t *builder,
                                                 dj_obstacle_static_t *obstacle)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(obstacle != NULL);

    polygon_t geometry_polygon;
    dj_polygon_to_geometry_polygon(&obstacle->shape, &geometry_polygon);

    // Pre-calculate bounding box
    distance_t min_x = FLT_MAX, max_x = -FLT_MAX;
    distance_t min_y = FLT_MAX, max_y = -FLT_MAX;

    for (uint32_t i = 0; i < geometry_polygon.nb_points; i++)
    {
        const point_t *pt = &geometry_polygon.points[i];
        if (pt->x < min_x)
            min_x = pt->x;
        if (pt->x > max_x)
            max_x = pt->x;
        if (pt->y < min_y)
            min_y = pt->y;
        if (pt->y > max_y)
            max_y = pt->y;
    }

    const point_t out_point = {-10000, -10000};
    const uint32_t nb_links = dj_graph_graph_get_num_links(&builder->graph);

    for (uint32_t i = 0; i < nb_links; i++)
    {
        dj_graph_link_t *link = dj_graph_graph_get_link(&builder->graph, i);
        SYSTEM_ASSERT(link != NULL);

        if (!link->enabled)
        {
            continue;
        }

        dj_graph_node_t *node1 = dj_graph_link_get_node1(link);
        dj_graph_node_t *node2 = dj_graph_link_get_node2(link);
        SYSTEM_ASSERT(node1 != NULL);
        SYSTEM_ASSERT(node2 != NULL);

        // Bounding box early exit
        const distance_t link_min_x = MIN(node1->pos.x, node2->pos.x);
        const distance_t link_max_x = MAX(node1->pos.x, node2->pos.x);
        const distance_t link_min_y = MIN(node1->pos.y, node2->pos.y);
        const distance_t link_max_y = MAX(node1->pos.y, node2->pos.y);

        if (link_max_x < min_x || link_min_x > max_x || link_max_y < min_y || link_min_y > max_y)
        {
            continue;
        }

        // Check if nodes are strictly inside obstacle
        if (is_strictly_in_polygon(geometry_polygon.points,
                                   (uint8_t)geometry_polygon.nb_points,
                                   &node1->pos,
                                   &out_point,
                                   NULL) ||
            is_strictly_in_polygon(geometry_polygon.points,
                                   (uint8_t)geometry_polygon.nb_points,
                                   &node2->pos,
                                   &out_point,
                                   NULL))
        {
            link->enabled = false;
            continue;
        }

        // Check if link intersects obstacle
        const segment_t link_seg = {node1->pos, node2->pos};
        if (polygon_strictly_intersects_segment(&geometry_polygon, &link_seg))
        {
            link->enabled = false;
        }
    }
}

void dj_graph_builder_add_obstacle_nodes(dj_graph_builder_t *builder,
                                         dj_obstacle_static_t *obstacle,
                                         dj_viewer_status_t *viewer_status,
                                         static_obstacles_list_t *obstacles,
                                         bool is_prebuilt)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(obstacle != NULL);
    SYSTEM_ASSERT(obstacles != NULL);

    dj_polygon_t *polygon = &obstacle->shape;
    const uint32_t nb_points = dj_polygon_get_nb_points(polygon);

#ifdef DJ_ENABLE_SUPER_LOGGING
    const bool debug_logs = true;
#else
    const bool debug_logs = false;
#endif

    for (uint32_t i = 0; i < nb_points; i++)
    {
        point_t *point = dj_polygon_get_point(polygon, (uint16_t)i);
        if (point == NULL)
        {
            continue;
        }

        dj_graph_node_t node;
        dj_graph_node_init(&node, point);
        dj_graph_node_t *new_node = dj_graph_builder_add_node_with_verif(builder,
                                                                         &node,
                                                                         DJ_GRAPH_NODE_TYPE_NORMAL,
                                                                         viewer_status);

        if (new_node != NULL)
        {
            dj_graph_builder_link_node(builder, new_node, obstacles, is_prebuilt, debug_logs);
        }
    }
}

void dj_graph_builder_build_with_obstacle(dj_graph_builder_t *builder,
                                          dj_obstacle_static_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          bool is_prebuilt,
                                          bool add_obstacle_as_node)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(obstacle != NULL);

#ifdef DJ_ENABLE_SUPER_LOGGING
    LOGD("Rebuild graph with a new obstacle");
#endif

    // Disable intersecting links
    dj_graph_builder_disable_intersecting_links(builder, obstacle);

    // Add obstacle vertices as nodes if requested
    if (add_obstacle_as_node)
    {
        static_obstacles_list_t *obstacles =
            dj_obstacle_manager_get_all_obstacles(&builder->obstacle_manager, viewer_status);
        dj_graph_builder_add_obstacle_nodes(
            builder, obstacle, viewer_status, obstacles, is_prebuilt);
    }

#ifdef DJ_ENABLE_SUPER_LOGGING
    dj_graph_graph_print(&builder->graph);
    dj_print_all_durations();
#endif
}

/* ******************************************* Public callback functions definitions ************************************* */
