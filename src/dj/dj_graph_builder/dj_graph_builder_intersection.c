/**
 * @file dj_graph_builder_intersection.c
 * @brief Intersection and collision detection for graph building
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder_intersection.h"
#include "system/assert/system_assert.h"
#include <math.h>

/* **************************************************** Private macros *************************************************** */

#define INTERIOR_SAMPLE_MIN_LENGTH 4.0f ///< Minimum segment length for interior sampling
#define INTERIOR_SAMPLE_DIVISOR    4.0f ///< Divisor for sampling points (1/4 and 3/4)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions definitions ******************************************* */

bool dj_graph_builder_is_polygon_edge(const point_t *node_pos,
                                      const point_t *other_node_pos,
                                      const precomputed_polygon_t *precomp)
{
    SYSTEM_ASSERT(node_pos != NULL);
    SYSTEM_ASSERT(other_node_pos != NULL);
    SYSTEM_ASSERT(precomp != NULL);

    const int16_t node_index_in_poly = polygon_contains_point(&precomp->geometry, node_pos);
    const int16_t other_node_index_in_poly =
        polygon_contains_point(&precomp->geometry, other_node_pos);

    if (node_index_in_poly == -1 || other_node_index_in_poly == -1)
    {
        return false; // At least one node is not a vertex of the polygon
    }

    // Both nodes are vertices of the polygon
    int16_t diff = node_index_in_poly - other_node_index_in_poly;

    // Handle wrap-around case (first and last vertices)
    if (other_node_index_in_poly == 0 && node_index_in_poly == (int16_t)(precomp->nb_points - 1))
    {
        diff = 1;
    }
    else if (node_index_in_poly == 0 &&
             other_node_index_in_poly == (int16_t)(precomp->nb_points - 1))
    {
        diff = -1;
    }

    // Adjacent vertices form an edge
    return (diff == 1 || diff == -1);
}

bool dj_graph_builder_link_through_interior(const segment_t *segment,
                                            const precomputed_polygon_t *precomp)
{
    SYSTEM_ASSERT(segment != NULL);
    SYSTEM_ASSERT(precomp != NULL);

    const distance_t dx = segment->b.x - segment->a.x;
    const distance_t dy = segment->b.y - segment->a.y;
    const distance_t segment_length_sq = dx * dx + dy * dy;

    // Only check interior if segment is long enough
    if (segment_length_sq <= INTERIOR_SAMPLE_MIN_LENGTH)
    {
        return false;
    }

    // Calculate sampling divisor based on segment length
    const distance_t segment_length = sqrtf(segment_length_sq);
    const distance_t divisor = MAX(segment_length / INTERIOR_SAMPLE_DIVISOR, 2.0f);

    // Sample at 1/4 and 3/4 of the segment
    const point_t sample_point_1 = {.x = segment->a.x + dx / divisor,
                                    .y = segment->a.y + dy / divisor};

    const point_t sample_point_2 = {.x = segment->b.x - dx / divisor,
                                    .y = segment->b.y - dy / divisor};

    const point_t out_point = {-10000.0f, -10000.0f};

    // Check if either sample point is strictly inside the polygon
    if (is_strictly_in_polygon(precomp->geometry.points,
                               (uint8_t)precomp->geometry.nb_points,
                               &sample_point_1,
                               &out_point,
                               NULL) ||
        is_strictly_in_polygon(precomp->geometry.points,
                               (uint8_t)precomp->geometry.nb_points,
                               &sample_point_2,
                               &out_point,
                               NULL))
    {
        return true;
    }

    return false;
}

bool dj_graph_builder_segment_intersects_obstacle(const segment_t *segment,
                                                  const point_t *node_pos,
                                                  const point_t *other_node_pos,
                                                  const precomputed_polygon_t *precomp)
{
    SYSTEM_ASSERT(segment != NULL);
    SYSTEM_ASSERT(node_pos != NULL);
    SYSTEM_ASSERT(other_node_pos != NULL);
    SYSTEM_ASSERT(precomp != NULL);

    // Special case: Check if the link is an edge of the polygon (should be allowed)
    if (dj_graph_builder_is_polygon_edge(node_pos, other_node_pos, precomp))
    {
        return false; // This link is an edge, allow it
    }

    // Check if segment strictly intersects the polygon
    if (polygon_strictly_intersects_segment(&precomp->geometry, segment))
    {
        return true;
    }

    // Special case: Check if the link is a diagonal passing through the polygon interior
    if (dj_graph_builder_link_through_interior(segment, precomp))
    {
        return true;
    }

    return false;
}

bool dj_graph_builder_pos_on_obstacle(point_t pos, static_obstacles_list_t *obstacles)
{
    if (obstacles == NULL)
    {
        return false;
    }

    const point_t out_point = {-10000, -10000};

    // bbox check first for a quick rejection
    precomputed_polygon_t precomp[DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES];
    dj_graph_builder_precompute_obstacles(obstacles, precomp);

    for (uint32_t i = 0; i < static_obstacles_list_size(obstacles); i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        SYSTEM_ASSERT(obstacle != NULL);

        if (!obstacle->is_enabled)
        {
            continue;
        }

        // Quick bbox check
        if (pos.x < precomp[i].min_x || pos.x > precomp[i].max_x || pos.y < precomp[i].min_y ||
            pos.y > precomp[i].max_y)
        {
            continue; // Outside bounding box, skip detailed check
        }

        dj_polygon_t *polygon = &obstacle->shape;
        if (is_strictly_in_polygon(polygon->points,
                                   (uint8_t)dj_polygon_get_nb_points(polygon),
                                   &pos,
                                   &out_point,
                                   NULL))
        {
            return true;
        }
    }

    return false;
}

/* ******************************************* Public callback functions definitions ************************************* */
