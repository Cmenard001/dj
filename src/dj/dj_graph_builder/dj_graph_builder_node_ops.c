/**
 * @file dj_graph_builder_node_ops.c
 * @brief Node operations for graph building (add, link, force link)
 * @author Cyprien Ménard
 * @date 01/2025
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder_node_ops.h"
#include "system/log/log.h"
#include "utils/dj/dj_config.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_intersection.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_precompute.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/maths/polygon/polygon.h"
#include "utils/maths/segment/segment.h"
#include <float.h>
#include <math.h>

LOG_REGISTER("utils/dj/node_ops");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/**
 * @brief Precomputed bounding box for fast obstacle rejection during extraction
 */
typedef dj_extraction_bbox_t extraction_bbox_t;

/* ********************************************* Private functions declarations ****************************************** */

static void precompute_obstacle_bboxes(static_obstacles_list_t *obstacles,
                                       extraction_bbox_t *bboxes,
                                       uint32_t nb_obstacles);

static float extraction_bbox_distance_sq(const extraction_bbox_t *bbox, point_t pos);

static void update_extraction_candidate(point_t candidate,
                                        point_t node_pos,
                                        static_obstacles_list_t *obstacles,
                                        const extraction_bbox_t *bboxes,
                                        uint32_t nb_obstacles,
                                        extraction_result_t *result);

static int circle_segment_intersections(
    point_t center, float radius_sq, const point_t *a, const point_t *b, point_t out[2]);

static bool is_smooth_link_valid(point_t node_pos,
                                 point_t other_pos,
                                 static_obstacles_list_t *obstacles,
                                 const extraction_bbox_t *bboxes,
                                 uint32_t nb_obstacles,
                                 const bool *is_source_obstacle,
                                 float smooth_radius_sq);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Precompute bounding boxes for all obstacles
 * @param[in] obstacles List of static obstacles
 * @param[out] bboxes Array to fill with bounding boxes
 * @param[in] nb_obstacles Number of obstacles in the list
 */
static void precompute_obstacle_bboxes(static_obstacles_list_t *obstacles,
                                       extraction_bbox_t *bboxes,
                                       uint32_t nb_obstacles)
{
    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        if (obstacle == NULL || !obstacle->is_enabled)
        {
            bboxes[i].is_enabled = false;
            continue;
        }

        bboxes[i].is_enabled = true;
        dj_polygon_t *polygon = &obstacle->shape;
        const uint32_t nb_points = dj_polygon_get_nb_points(polygon);

        point_t *first = dj_polygon_get_point(polygon, 0);
        SYSTEM_ASSERT(first != NULL);

        bboxes[i].min_x = first->x;
        bboxes[i].max_x = first->x;
        bboxes[i].min_y = first->y;
        bboxes[i].max_y = first->y;

        for (uint32_t j = 1; j < nb_points; j++)
        {
            point_t *p = dj_polygon_get_point(polygon, (uint16_t)j);
            if (p == NULL)
            {
                continue;
            }
            if (p->x < bboxes[i].min_x)
            {
                bboxes[i].min_x = p->x;
            }
            if (p->x > bboxes[i].max_x)
            {
                bboxes[i].max_x = p->x;
            }
            if (p->y < bboxes[i].min_y)
            {
                bboxes[i].min_y = p->y;
            }
            if (p->y > bboxes[i].max_y)
            {
                bboxes[i].max_y = p->y;
            }
        }
    }
}

/**
 * @brief Compute squared distance from a point to a bounding box
 * @return 0 if the point is inside the bbox, squared distance otherwise
 */
static float extraction_bbox_distance_sq(const extraction_bbox_t *bbox, point_t pos)
{
    float dx = 0.0f;
    float dy = 0.0f;

    if (pos.x < bbox->min_x)
    {
        dx = bbox->min_x - pos.x;
    }
    else if (pos.x > bbox->max_x)
    {
        dx = pos.x - bbox->max_x;
    }

    if (pos.y < bbox->min_y)
    {
        dy = bbox->min_y - pos.y;
    }
    else if (pos.y > bbox->max_y)
    {
        dy = pos.y - bbox->max_y;
    }

    return dx * dx + dy * dy;
}

/**
 * @brief Update extraction result if candidate is closer and not inside any obstacle
 * @details Uses bounding box prefiltering for fast obstacle rejection.
 *          For each obstacle, first checks if the candidate falls within its bounding box
 *          before performing the expensive point-in-polygon test.
 */
static void update_extraction_candidate(point_t candidate,
                                        point_t node_pos,
                                        static_obstacles_list_t *obstacles,
                                        const extraction_bbox_t *bboxes,
                                        uint32_t nb_obstacles,
                                        extraction_result_t *result)
{
    const float distance = point_distance_square(&node_pos, &candidate);
    if (distance >= result->distance)
    {
        return;
    }

    // Push the candidate slightly outward (away from node_pos) to avoid floating-point
    // boundary issues. This ensures the point is mathematically outside the obstacle
    // and prevents "slightly inside" bugs later in the algorithm.
    point_t test_point = candidate;
    const float push_margin = 1.0f; // Push by 1 mm outwards
    if (distance > 0.001f)          // Ensure stable normalization
    {
        const float dist_sqrt = sqrtf(distance);
        test_point.x += ((candidate.x - node_pos.x) / dist_sqrt) * push_margin;
        test_point.y += ((candidate.y - node_pos.y) / dist_sqrt) * push_margin;
    }

    // Check if candidate is strictly inside any obstacle (with bbox prefiltering)
    const point_t out_point = {-10000.0f, -10000.0f};

    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        if (!bboxes[i].is_enabled)
        {
            continue;
        }

        // Fast bounding box rejection: skip obstacles whose bbox doesn't contain the candidate
        if (test_point.x < bboxes[i].min_x || test_point.x > bboxes[i].max_x ||
            test_point.y < bboxes[i].min_y || test_point.y > bboxes[i].max_y)
        {
            continue;
        }

        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        dj_polygon_t *polygon = &obstacle->shape;

        if (is_strictly_in_polygon(polygon->points,
                                   (uint8_t)dj_polygon_get_nb_points(polygon),
                                   &test_point,
                                   &out_point,
                                   NULL))
        {
            return; // Candidate is inside an obstacle, not valid
        }
    }

    // Candidate is valid: not strictly inside any obstacle and closer than current best
    result->found = true;
    result->extraction = test_point; // Return the pushed point!
    result->distance = distance;     // keep original distance for comparison
}

/**
 * @brief Computes the closest point on a segment [A, B] from point P using scalar projection.
 * Guaranteed to return a point strictly belonging to the finite segment.
 */
static point_t closest_point_on_segment(const point_t *p, const point_t *a, const point_t *b)
{
    SYSTEM_ASSERT(p != NULL);
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    const float dx = b->x - a->x;
    const float dy = b->y - a->y;
    const float l2 = dx * dx + dy * dy; // Segment length squared

    if (l2 == 0.0f)
    {
        // A and B are the same point
        return *a;
    }

    // Dot product to find orthogonal projection parameter t
    float t = ((p->x - a->x) * dx + (p->y - a->y) * dy) / l2;

    // Clamp t to [0, 1] to stay bounded on the segment
    if (t < 0.0f)
    {
        t = 0.0f;
    }
    else if (t > 1.0f)
    {
        t = 1.0f;
    }

    point_t closest = {.x = a->x + t * dx, .y = a->y + t * dy};

    return closest;
}

/**
 * @brief Compute intersection points between a circle and a segment [A, B]
 * @details Uses parametric form P(t) = A + t*(B-A), solves quadratic |P(t)-C|^2 = R^2,
 *          and keeps only solutions with t in [0, 1].
 * @param[in]  center    Center of the circle
 * @param[in]  radius_sq Squared radius of the circle (mm^2)
 * @param[in]  a         First endpoint of the segment
 * @param[in]  b         Second endpoint of the segment
 * @param[out] out       Output array for at most 2 intersection points
 * @return Number of valid intersection points found (0, 1, or 2)
 */
static int circle_segment_intersections(
    point_t center, float radius_sq, const point_t *a, const point_t *b, point_t out[2])
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);

    const float dx = b->x - a->x;
    const float dy = b->y - a->y;
    const float ax = a->x - center.x;
    const float ay = a->y - center.y;

    const float a_coef = dx * dx + dy * dy;
    if (a_coef < 0.0001f)
    {
        return 0; // Degenerate segment (zero length)
    }

    const float b_coef = 2.0f * (ax * dx + ay * dy);
    const float c_coef = ax * ax + ay * ay - radius_sq;

    const float discriminant = b_coef * b_coef - 4.0f * a_coef * c_coef;
    if (discriminant < 0.0f)
    {
        return 0; // No real intersection
    }

    const float sqrt_d = sqrtf(discriminant);
    int count = 0;

    const float t1 = (-b_coef - sqrt_d) / (2.0f * a_coef);
    if (t1 >= 0.0f && t1 <= 1.0f)
    {
        out[count].x = a->x + t1 * dx;
        out[count].y = a->y + t1 * dy;
        count++;
    }

    // Avoid duplicate point for tangent case (discriminant ~ 0)
    if (discriminant > 0.0001f)
    {
        const float t2 = (-b_coef + sqrt_d) / (2.0f * a_coef);
        if (t2 >= 0.0f && t2 <= 1.0f)
        {
            out[count].x = a->x + t2 * dx;
            out[count].y = a->y + t2 * dy;
            count++;
        }
    }

    return count;
}

/**
 * @brief Check if a link from node_pos to other_pos is valid for smooth extraction
 * @details A link [N->X] is valid if:
 *   - X is not inside or on the boundary of any SOURCE obstacle (prevents interior diagonals
 *     and segments staying entirely inside the obstacle)
 *   - Every intersection of the segment with a SOURCE obstacle edge is at distance <= smooth_radius_sq from N
 *   - No NON-source obstacle intersects the segment
 * @param[in] node_pos           Position of the node being extracted
 * @param[in] other_pos          Position of the target node
 * @param[in] obstacles          List of all obstacles
 * @param[in] bboxes             Precomputed bounding boxes (from builder->extraction_bboxes)
 * @param[in] nb_obstacles       Number of obstacles
 * @param[in] is_source_obstacle Array: true if obstacle i strictly contains node_pos
 * @param[in] smooth_radius_sq   Squared smooth extraction radius (mm^2)
 * @return true if the link is valid for smooth extraction, false otherwise
 */
static bool is_smooth_link_valid(point_t node_pos,
                                 point_t other_pos,
                                 static_obstacles_list_t *obstacles,
                                 const extraction_bbox_t *bboxes,
                                 uint32_t nb_obstacles,
                                 const bool *is_source_obstacle,
                                 float smooth_radius_sq)
{
    SYSTEM_ASSERT(obstacles != NULL);
    SYSTEM_ASSERT(bboxes != NULL);
    SYSTEM_ASSERT(is_source_obstacle != NULL);

    const segment_t link_seg = {.a = node_pos, .b = other_pos};

    // Bounding box of the link segment for fast obstacle rejection
    const float seg_min_x = (node_pos.x < other_pos.x) ? node_pos.x : other_pos.x;
    const float seg_max_x = (node_pos.x > other_pos.x) ? node_pos.x : other_pos.x;
    const float seg_min_y = (node_pos.y < other_pos.y) ? node_pos.y : other_pos.y;
    const float seg_max_y = (node_pos.y > other_pos.y) ? node_pos.y : other_pos.y;

    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        if (!bboxes[i].is_enabled)
        {
            continue;
        }

        // Fast bbox rejection: skip if the segment's bbox doesn't overlap the obstacle's bbox
        if (seg_max_x < bboxes[i].min_x || seg_min_x > bboxes[i].max_x ||
            seg_max_y < bboxes[i].min_y || seg_min_y > bboxes[i].max_y)
        {
            continue;
        }

        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        dj_polygon_t *polygon = &obstacle->shape;
        const uint32_t nb_points = dj_polygon_get_nb_points(polygon);

        // Guard: for source obstacles, reject if other_pos is inside or on the polygon boundary.
        // Prevents links that traverse the polygon interior, e.g.:
        //   - segment staying entirely inside the obstacle (no edge crossing detected)
        //   - diagonal to a boundary vertex of the source polygon (vertex is within smooth_radius
        //     so the edge-crossing check passes, but the path goes through the interior)
        if (is_source_obstacle[i])
        {
            const point_t out_point = {-10000.0f, -10000.0f};
            if (is_in_polygon(polygon->points, (uint8_t)nb_points, &other_pos, &out_point, NULL))
            {
                return false;
            }
        }

        for (uint32_t j = 0; j < nb_points; j++)
        {
            point_t *p1 = dj_polygon_get_point(polygon, (uint16_t)j);
            point_t *p2 = dj_polygon_get_point(polygon, (uint16_t)((j + 1) % nb_points));
            if (p1 == NULL || p2 == NULL)
            {
                continue;
            }

            const segment_t edge = {.a = *p1, .b = *p2};
            point_t inter;
            if (!segments_get_intersection(&link_seg, &edge, &inter))
            {
                continue;
            }

            if (is_source_obstacle[i])
            {
                // Source obstacle: crossing allowed only within the smooth radius
                if (point_distance_square(&node_pos, &inter) > smooth_radius_sq)
                {
                    return false;
                }
            }
            else
            {
                // Non-source obstacle: any crossing is blocking
                return false;
            }
        }
    }

    return true;
}

/* *********************************************** Public functions definitions ******************************************* */

void dj_graph_builder_build_nodes(dj_graph_builder_t *builder, dj_viewer_status_t *viewer_status)
{
    SYSTEM_ASSERT(builder != NULL);

    static_obstacles_list_t *obstacles =
        dj_obstacle_manager_get_all_obstacles(&builder->obstacle_manager, viewer_status);
    SYSTEM_ASSERT(obstacles != NULL);

    const uint32_t nb_obstacles = static_obstacles_list_size(obstacles);

    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        if (obstacle == NULL || !obstacle->is_enabled)
        {
            continue;
        }

        dj_polygon_t *polygon = &obstacle->shape;
        const uint32_t nb_points = dj_polygon_get_nb_points(polygon);

        for (uint32_t j = 0; j < nb_points; j++)
        {
            point_t *point = dj_polygon_get_point(polygon, (uint16_t)j);
            if (point == NULL)
            {
                continue;
            }

            dj_graph_node_t node;
            dj_graph_node_init(&node, point);
            dj_graph_builder_add_node_with_verif(builder,
                                                 &node,
                                                 DJ_GRAPH_NODE_TYPE_PREBUILT,
                                                 viewer_status);
        }
    }
}

dj_graph_node_t *dj_graph_builder_add_node_with_verif(dj_graph_builder_t *builder,
                                                      dj_graph_node_t *node,
                                                      dj_graph_node_type_t type,
                                                      dj_viewer_status_t *viewer_status)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(node != NULL);

    // Check if node is within restricted workspace polygon (if set)
    if (builder->restricted_workspace_polygon != NULL)
    {
        const point_t out_point = {-10000.0f, -10000.0f};
        if (!is_strictly_in_polygon(builder->restricted_workspace_polygon->points,
                                    (uint8_t)builder->restricted_workspace_polygon->nb_points,
                                    &node->pos,
                                    &out_point,
                                    NULL))
        {
            return NULL; // Node is outside the restricted workspace, do not add
        }
    }

    // Check if node already exists
    for (uint32_t i = 0; i < dj_graph_graph_get_num_nodes(&builder->graph); i++)
    {
        dj_graph_node_t *existing = dj_graph_graph_get_node(&builder->graph, i);
        if (dj_graph_node_compare(node, existing))
        {
            return NULL;
        }
    }

    // Check if node is not strictly on an obstacle
    static_obstacles_list_t *obstacles =
        dj_obstacle_manager_get_all_obstacles(&builder->obstacle_manager, viewer_status);

    if (dj_graph_builder_pos_on_obstacle(node->pos, obstacles))
    {
        return NULL;
    }

    return dj_graph_graph_add_node(&builder->graph, node, type);
}

void dj_graph_builder_link_node(dj_graph_builder_t *builder,
                                dj_graph_node_t *node,
                                static_obstacles_list_t *obstacles,
                                bool is_prebuilt,
                                bool verbose)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(node != NULL);
    SYSTEM_ASSERT(obstacles != NULL);

    if (!node->enabled)
    {
        return;
    }

    const point_t node_pos = dj_graph_node_get_pos(node);

    // Precompute obstacles
    const uint32_t nb_obstacles =
        dj_graph_builder_precompute_obstacles(obstacles, builder->precomputed);

    // Link to all other valid nodes
    const uint32_t nb_nodes = dj_graph_graph_get_num_nodes(&builder->graph);

    // Build quick linked check array to avoid O(N^3) complexity
    bool already_linked[DJ_GRAPH_GRAPH_MAX_NODES] = {false};
    dj_graph_node_t *node_array = builder->graph.nodes.array;
    const uint32_t nb_links = dj_graph_graph_get_num_links(&builder->graph);
    for (uint32_t i = 0; i < nb_links; i++)
    {
        dj_graph_link_t *link = builder->graph.links.array + i;
        if (link == NULL)
        {
            continue;
        }

        if (link->node1 == node)
        {
            uint32_t idx = (uint32_t)(link->node2 - node_array);
            if (idx < DJ_GRAPH_GRAPH_MAX_NODES)
            {
                already_linked[idx] = true;
            }
        }
        else if (link->node2 == node)
        {
            uint32_t idx = (uint32_t)(link->node1 - node_array);
            if (idx < DJ_GRAPH_GRAPH_MAX_NODES)
            {
                already_linked[idx] = true;
            }
        }
    }

    for (uint32_t i = 0; i < nb_nodes; i++)
    {
        dj_graph_node_t *other_node = dj_graph_graph_get_node(&builder->graph, i);
        if (other_node == NULL || !other_node->enabled)
        {
            continue;
        }

        if (dj_graph_node_compare(node, other_node))
        {
            continue; // Same node
        }

        if (already_linked[i])
        {
            continue; // Link already exists
        }

        const point_t other_node_pos = dj_graph_node_get_pos(other_node);
        const segment_t link_segment = {.a = node_pos, .b = other_node_pos};

        // Check intersection with all obstacles
        bool intersects = false;
        for (uint32_t j = 0; j < nb_obstacles; j++)
        {
            if (!builder->precomputed[j].is_enabled)
            {
                continue;
            }

            // Bounding box early exit
            if (!dj_graph_builder_bbox_overlap(&link_segment, &builder->precomputed[j]))
            {
                continue;
            }

            if (dj_graph_builder_segment_intersects_obstacle(
                    &link_segment, &node_pos, &other_node_pos, &builder->precomputed[j]))
            {
                intersects = true;
                break;
            }
        }

        if (!intersects)
        {
            dj_graph_link_t *link =
                dj_graph_graph_add_link(&builder->graph, node, other_node, is_prebuilt);
            if (link != NULL)
            {
                link->enabled = node->enabled && other_node->enabled;
                if (verbose)
                {
                    LOGD("Linked node (%f, %f) with node (%f, %f), enabled = %d",
                         node_pos.x,
                         node_pos.y,
                         other_node_pos.x,
                         other_node_pos.y,
                         link->enabled);
                }
            }
        }
    }
}

extraction_result_t dj_graph_builder_find_extraction_point(dj_graph_builder_t *builder,
                                                           point_t node_pos,
                                                           static_obstacles_list_t *obstacles)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(obstacles != NULL);

    extraction_result_t result = {.found = false, .distance = FLT_MAX};
    const uint32_t nb_obstacles = static_obstacles_list_size(obstacles);

    // Precompute bounding boxes for all obstacles (used for fast rejection)
    extraction_bbox_t *bboxes = builder->extraction_bboxes;
    precompute_obstacle_bboxes(obstacles, bboxes, nb_obstacles);

    // Phase 1: Check vertices and edge projections of each obstacle.
    // For each obstacle, skip it entirely if its bounding box is farther
    // than the current best candidate distance (distance pruning).
    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        if (!bboxes[i].is_enabled)
        {
            continue;
        }

        // Distance pruning: skip obstacles too far to improve the result
        if (extraction_bbox_distance_sq(&bboxes[i], node_pos) >= result.distance)
        {
            continue;
        }

        dj_obstacle_static_t *obstacle = static_obstacles_list_get(obstacles, i);
        dj_polygon_t *polygon = &obstacle->shape;
        const uint32_t nb_points = dj_polygon_get_nb_points(polygon);

        // Check projections on all edges (the parameter t in [0, 1] approach naturally handles vertices)
        for (uint32_t j = 0; j < nb_points; j++)
        {
            point_t *p1 = dj_polygon_get_point(polygon, (uint16_t)j);
            point_t *p2 = dj_polygon_get_point(polygon, (uint16_t)((j + 1) % nb_points));

            if (p1 == NULL || p2 == NULL)
            {
                continue;
            }

            point_t projection = closest_point_on_segment(&node_pos, p1, p2);
            update_extraction_candidate(
                projection, node_pos, obstacles, bboxes, nb_obstacles, &result);
        }
    }

    // Phase 2: Check edge-edge intersections between obstacle pairs.
    // Only consider pairs whose bounding boxes overlap (no intersection possible
    // otherwise) and that are close enough to improve the current best.
    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        if (!bboxes[i].is_enabled)
        {
            continue;
        }

        if (extraction_bbox_distance_sq(&bboxes[i], node_pos) >= result.distance)
        {
            continue;
        }

        dj_obstacle_static_t *obs1 = static_obstacles_list_get(obstacles, i);
        dj_polygon_t *poly1 = &obs1->shape;
        const uint32_t np1 = dj_polygon_get_nb_points(poly1);

        for (uint32_t k = i + 1; k < nb_obstacles; k++)
        {
            if (!bboxes[k].is_enabled)
            {
                continue;
            }

            // Skip non-overlapping obstacle pairs
            if (bboxes[i].min_x > bboxes[k].max_x || bboxes[i].max_x < bboxes[k].min_x ||
                bboxes[i].min_y > bboxes[k].max_y || bboxes[i].max_y < bboxes[k].min_y)
            {
                continue;
            }

            if (extraction_bbox_distance_sq(&bboxes[k], node_pos) >= result.distance)
            {
                continue;
            }

            dj_obstacle_static_t *obs2 = static_obstacles_list_get(obstacles, k);
            dj_polygon_t *poly2 = &obs2->shape;
            const uint32_t np2 = dj_polygon_get_nb_points(poly2);

            for (uint32_t e1 = 0; e1 < np1; e1++)
            {
                point_t *p11 = dj_polygon_get_point(poly1, (uint16_t)e1);
                point_t *p12 = dj_polygon_get_point(poly1, (uint16_t)((e1 + 1) % np1));

                if (p11 == NULL || p12 == NULL)
                {
                    continue;
                }

                segment_t seg1 = {*p11, *p12};

                for (uint32_t e2 = 0; e2 < np2; e2++)
                {
                    point_t *p21 = dj_polygon_get_point(poly2, (uint16_t)e2);
                    point_t *p22 = dj_polygon_get_point(poly2, (uint16_t)((e2 + 1) % np2));

                    if (p21 == NULL || p22 == NULL)
                    {
                        continue;
                    }

                    segment_t seg2 = {*p21, *p22};

                    point_t intersection;
                    if (segments_get_intersection(&seg1, &seg2, &intersection))
                    {
                        update_extraction_candidate(
                            intersection, node_pos, obstacles, bboxes, nb_obstacles, &result);
                    }
                }
            }
        }
    }

    return result;
}

void dj_graph_builder_force_link_node(dj_graph_builder_t *builder,
                                      dj_graph_node_t *node,
                                      static_obstacles_list_t *obstacles,
                                      bool is_prebuilt,
                                      bool verbose)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(node != NULL);
    SYSTEM_ASSERT(obstacles != NULL);

    dj_mark_start_time(FORCE_LINK_NODE);

    if (verbose)
    {
        LOGD("Try to force link node (%f, %f)", node->pos.x, node->pos.y);
    }

    // Check if node is on an obstacle
    if (!dj_graph_builder_pos_on_obstacle(node->pos, obstacles))
    {
        if (verbose)
        {
            LOGD("Node (%f, %f) is not on an obstacle, make a normal linking",
                 node->pos.x,
                 node->pos.y);
        }
        dj_graph_builder_link_node(builder, node, obstacles, is_prebuilt, verbose);
        dj_mark_end_time(FORCE_LINK_NODE);
        return;
    }

    // Find extraction point (also fills builder->extraction_bboxes, reused by smooth mode)
    extraction_result_t extraction =
        dj_graph_builder_find_extraction_point(builder, node->pos, obstacles);

    if (!extraction.found)
    {
        LOGE("No valid extraction point found for node (%f, %f)", node->pos.x, node->pos.y);
        dj_mark_end_time(FORCE_LINK_NODE);
        return;
    }

    const extraction_bbox_t *bboxes = builder->extraction_bboxes;
    const uint32_t nb_obstacles = static_obstacles_list_size(obstacles);
    const point_t out_point = {-10000.0f, -10000.0f};

    distance_t smooth_extraction_radius = FLT_MAX;
    // Determine which obstacles are source obstacles (those strictly containing the node)
    bool is_source[DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES] = {false};
    for (uint32_t i = 0; i < nb_obstacles; i++)
    {
        if (!bboxes[i].is_enabled)
        {
            continue;
        }
        dj_obstacle_static_t *obs = static_obstacles_list_get(obstacles, i);
        dj_polygon_t *poly = &obs->shape;
        is_source[i] = is_strictly_in_polygon(
            poly->points, (uint8_t)dj_polygon_get_nb_points(poly), &node->pos, &out_point, NULL);
        // If the node is inside this obstacle, update the smooth extraction radius
        if (is_source[i])
        {
            distance_t current_smooth_radius = obs->smooth_extraction_radius;
            if (current_smooth_radius < smooth_extraction_radius)
            {
                smooth_extraction_radius = current_smooth_radius;
            }
        }
    }

    // Check that at least one source obstacle is found
    if (smooth_extraction_radius == FLT_MAX)
    {
        smooth_extraction_radius = 0.0f;
    }

    // Smooth extraction mode: if the nearest extraction point is within smooth_extraction_radius,
    // place nodes at circle/obstacle-edge intersections and create smooth links from the original
    // node to any reachable node whose segment only crosses the source obstacle within the radius.
    if (smooth_extraction_radius > 0.0f &&
        extraction.distance <= smooth_extraction_radius * smooth_extraction_radius)
    {
        const float radius_sq = smooth_extraction_radius * smooth_extraction_radius;
        // Phase A: for each obstacle edge, find circle/edge intersection points, validate them,
        // add them as new nodes and link them normally to the rest of the graph.
        bool smooth_node_added = false;
        const uint32_t nb_nodes_before_smooth = dj_graph_graph_get_num_nodes(&builder->graph);
        for (uint32_t i = 0; i < nb_obstacles; i++)
        {
            if (!bboxes[i].is_enabled)
            {
                continue;
            }
            dj_obstacle_static_t *obs = static_obstacles_list_get(obstacles, i);
            dj_polygon_t *poly = &obs->shape;
            const uint32_t nb_pts = dj_polygon_get_nb_points(poly);

            for (uint32_t j = 0; j < nb_pts; j++)
            {
                point_t *p1 = dj_polygon_get_point(poly, (uint16_t)j);
                point_t *p2 = dj_polygon_get_point(poly, (uint16_t)((j + 1) % nb_pts));
                if (p1 == NULL || p2 == NULL)
                {
                    continue;
                }

                point_t circle_pts[2];
                const int nb_circle_pts =
                    circle_segment_intersections(node->pos, radius_sq, p1, p2, circle_pts);

                for (int k = 0; k < nb_circle_pts; k++)
                {
                    // Validate: push point outward and verify it is not inside any obstacle
                    extraction_result_t candidate = {.found = false, .distance = FLT_MAX};
                    update_extraction_candidate(
                        circle_pts[k], node->pos, obstacles, bboxes, nb_obstacles, &candidate);
                    if (!candidate.found)
                    {
                        continue;
                    }

                    dj_graph_node_t circle_node;
                    dj_graph_node_init(&circle_node, &candidate.extraction);
                    dj_graph_node_t *new_circle_node =
                        dj_graph_graph_add_node(&builder->graph,
                                                &circle_node,
                                                is_prebuilt ? DJ_GRAPH_NODE_TYPE_PREBUILT
                                                            : DJ_GRAPH_NODE_TYPE_NORMAL);

                    if (new_circle_node != NULL)
                    {
                        dj_graph_graph_add_link(&builder->graph,
                                                node,
                                                new_circle_node,
                                                is_prebuilt);
                        dj_graph_builder_link_node(
                            builder, new_circle_node, obstacles, is_prebuilt, verbose);
                        smooth_node_added = true;
                        if (verbose)
                        {
                            LOGD("Smooth extraction: added circle node (%f, %f)",
                                 new_circle_node->pos.x,
                                 new_circle_node->pos.y);
                        }
                    }
                }
            }
        }

        // Phase B: create smooth links from the original node to pre-existing reachable nodes.
        // A link [N->X] is valid if every intersection with a source obstacle edge is within
        // the smooth radius, and no non-source obstacle blocks the segment.
        bool smooth_link_added = false;
        for (uint32_t i = 0; i < nb_nodes_before_smooth; i++)
        {
            dj_graph_node_t *other_node = dj_graph_graph_get_node(&builder->graph, i);
            if (other_node == NULL || !other_node->enabled)
            {
                continue;
            }
            if (dj_graph_node_compare(node, other_node))
            {
                continue;
            }

            const point_t other_pos = dj_graph_node_get_pos(other_node);
            if (is_smooth_link_valid(
                    node->pos, other_pos, obstacles, bboxes, nb_obstacles, is_source, radius_sq))
            {
                dj_graph_link_t *smooth_link =
                    dj_graph_graph_add_link(&builder->graph, node, other_node, is_prebuilt);
                if (smooth_link != NULL)
                {
                    smooth_link_added = true;
                    if (verbose)
                    {
                        LOGD("Smooth extraction: linked (%f, %f) to (%f, %f)",
                             node->pos.x,
                             node->pos.y,
                             other_pos.x,
                             other_pos.y);
                    }
                }
            }
        }

        if (smooth_node_added || smooth_link_added)
        {
            dj_mark_end_time(FORCE_LINK_NODE);
            return;
        }
        LOGW("Smooth extraction yielded no links for node (%f, %f), falling back to classic",
             node->pos.x,
             node->pos.y);
    }

    // Classic extraction: create a single node at the nearest valid boundary point
    dj_graph_node_t extraction_node;
    dj_graph_node_init(&extraction_node, &extraction.extraction);
    dj_graph_node_t *new_node = dj_graph_graph_add_node(&builder->graph,
                                                        &extraction_node,
                                                        is_prebuilt ? DJ_GRAPH_NODE_TYPE_PREBUILT
                                                                    : DJ_GRAPH_NODE_TYPE_NORMAL);
    SYSTEM_ASSERT(new_node != NULL);

    // Link original node to extraction node
    dj_graph_link_t *link = dj_graph_graph_add_link(&builder->graph, node, new_node, is_prebuilt);
    SYSTEM_ASSERT(link != NULL);

    if (verbose)
    {
        LOGD("Force link node (%f, %f) with extraction point (%f, %f), enabled = %d",
             node->pos.x,
             node->pos.y,
             extraction.extraction.x,
             extraction.extraction.y,
             link->enabled);
    }

    // Link extraction node normally
    dj_graph_builder_link_node(builder, new_node, obstacles, is_prebuilt, verbose);
    dj_mark_end_time(FORCE_LINK_NODE);
}

/* ******************************************* Public callback functions definitions ************************************* */
