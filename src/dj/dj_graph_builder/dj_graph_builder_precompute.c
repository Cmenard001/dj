/**
 * @file dj_graph_builder_precompute.c
 * @brief Precomputed polygon data structures for graph building optimization
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder_precompute.h"
#include "system/assert/system_assert.h"
#include "utils/dj/dj_config.h"
#include <float.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions definitions ******************************************* */

uint32_t dj_graph_builder_precompute_obstacles(static_obstacles_list_t *obstacles,
                                               precomputed_polygon_t *precomputed)
{
    SYSTEM_ASSERT(obstacles != NULL);
    SYSTEM_ASSERT(precomputed != NULL);

    const uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    SYSTEM_ASSERT(nb_obstacles <= DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES);

    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        if (obstacle == NULL || !obstacle->is_enabled)
        {
            precomputed[i].is_enabled = false;
            continue;
        }

        precomputed[i].is_enabled = true;
        dj_polygon_t *polygon = &obstacle->shape;
        dj_polygon_to_geometry_polygon(polygon, &precomputed[i].geometry);
        precomputed[i].nb_points = dj_polygon_get_nb_points(polygon);

        // Compute bounding box
        precomputed[i].min_x = FLT_MAX;
        precomputed[i].max_x = -FLT_MAX;
        precomputed[i].min_y = FLT_MAX;
        precomputed[i].max_y = -FLT_MAX;

        for (uint32_t j = 0; j < precomputed[i].geometry.nb_points; j++)
        {
            const point_t *pt = &precomputed[i].geometry.points[j];
            if (pt->x < precomputed[i].min_x)
                precomputed[i].min_x = pt->x;
            if (pt->x > precomputed[i].max_x)
                precomputed[i].max_x = pt->x;
            if (pt->y < precomputed[i].min_y)
                precomputed[i].min_y = pt->y;
            if (pt->y > precomputed[i].max_y)
                precomputed[i].max_y = pt->y;
        }
    }

    return nb_obstacles;
}

bool dj_graph_builder_bbox_overlap(const segment_t *link_segment,
                                   const precomputed_polygon_t *precomp)
{
    SYSTEM_ASSERT(link_segment != NULL);
    SYSTEM_ASSERT(precomp != NULL);

    const distance_t link_min_x = MIN(link_segment->a.x, link_segment->b.x);
    const distance_t link_max_x = MAX(link_segment->a.x, link_segment->b.x);
    const distance_t link_min_y = MIN(link_segment->a.y, link_segment->b.y);
    const distance_t link_max_y = MAX(link_segment->a.y, link_segment->b.y);

    // Check if bounding boxes overlap
    if (link_max_x < precomp->min_x || link_min_x > precomp->max_x || link_max_y < precomp->min_y ||
        link_min_y > precomp->max_y)
    {
        return false; // No overlap
    }

    return true; // Bounding boxes overlap
}

/* ******************************************* Public callback functions definitions ************************************* */
