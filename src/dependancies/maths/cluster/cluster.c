/**
 * @file QS_maths_cluster.c
 * @author Théo Magne
 * @date 29/05/2025
 * @see QS_maths_cluster.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ************************************************ Includes **************************************** */

#include "utils/maths/cluster/cluster.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "utils/macros/macros.h"
#include "utils/maths/distance/distance.h"
#include "utils/maths/point/point.h"
#include "utils/maths/segment/segment.h"
#include <float.h>
#include <math.h>

LOG_REGISTER("utils/maths/cluster");

/* ********************************************* Private macros ************************************* */

/* ***************************************** Private type definition ******************************** */

/* ************************************** Private functions declarations **************************** */

/* ******************************************* Private variables ************************************ */

/* *************************************** Private functions definitions **************************** */

/* *************************************** Public functions definitions ***************************** */

size_t cluster_find_cluster(const point_t *buff,
                            size_t buff_size,
                            cluster_t *cluster,
                            distance_t max_dist_point_mm,
                            distance_t max_cluster_size_mm,
                            uint16_t min_points,
                            rectangle_t *rectangle)
{
    // Check input parameters
    if (buff == NULL || cluster == NULL)
    {
        SYSTEM_ASSERT(false);
        return 0;
    }

    // Force the rectangle with valid coordinates to make sure p1.x < p2.x and p1.y < p2.y
    if (rectangle != NULL)
    {
        distance_t min_x = MIN(rectangle->p1.x, rectangle->p2.x);
        distance_t max_x = MAX(rectangle->p1.x, rectangle->p2.x);
        distance_t min_y = MIN(rectangle->p1.y, rectangle->p2.y);
        distance_t max_y = MAX(rectangle->p1.y, rectangle->p2.y);

        rectangle->p1.x = min_x;
        rectangle->p2.x = max_x;
        rectangle->p1.y = min_y;
        rectangle->p2.y = max_y;
    }

    // Reset the cluster size
    cluster->size = 0;

    // Initialize variables
    const point_t *read_buff_ptr = buff;

    // This is the write index for the buffer of the cluster
    point_t *cluster_buff = cluster->points;
    size_t cluster_w_idx = 0;

    // Square of the maximum distance between two points to be considered in the same cluster
    float max_dist_point_sq_mm = max_dist_point_mm * max_dist_point_mm;

    // Square of the maximum cluster size
    float max_cluster_size_sq_mm = max_cluster_size_mm * max_cluster_size_mm;

    // Initialize the cluster buffer
    cluster_buff[0] = *read_buff_ptr++;
    cluster_w_idx = 1;

    // Iterate through the points in the buffer
    while (read_buff_ptr < (buff + buff_size))
    {
        // Filter points outside the rectangle if provided
        if (rectangle != NULL)
        {
            if ((read_buff_ptr->x < rectangle->p1.x) || (read_buff_ptr->x > rectangle->p2.x) ||
                (read_buff_ptr->y < rectangle->p1.y) || (read_buff_ptr->y > rectangle->p2.y))
            {
                // Skip the points outside of the rectangle
                read_buff_ptr++;
                continue;
            }
        }

        // Check distance with the closest point in the cluster to avoid having clusters with points
        // too far from each other
        float square_distance_with_closest_point = FLT_MAX;
        for (size_t i = 0; i < cluster_w_idx; i++)
        {
            float distance_sq = point_distance_square(read_buff_ptr, &cluster_buff[i]);
            if (distance_sq < square_distance_with_closest_point)
            {
                square_distance_with_closest_point = distance_sq;
            }
        }

        // Check distance with the furthest point in the cluster to avoid having clusters bigger than
        // max_cluster_size_mm
        float square_distance_with_furthest_point = 0;
        for (size_t i = 0; i < cluster_w_idx; i++)
        {
            float distance_sq = point_distance_square(read_buff_ptr, &cluster_buff[i]);
            if (distance_sq > square_distance_with_furthest_point)
            {
                square_distance_with_furthest_point = distance_sq;
            }
        }

        // Check if the new point is part of the cluster by checking if it's close enough to the closest
        // point
        if (square_distance_with_furthest_point < max_cluster_size_sq_mm &&
            square_distance_with_closest_point < max_dist_point_sq_mm)
        {
            // The new point is part of the cluster
            cluster_buff[cluster_w_idx] = *read_buff_ptr;
            cluster_w_idx++;
            // Check if the cluster is too big, if yes, we stop here and don't increase
            if (cluster_w_idx >= CLUSTER_MAX_POINTS)
            {
                break;
            }
            read_buff_ptr++;
        }
        else if (cluster_w_idx >= min_points)
        {
            // We have enough point in the hypothetical cluster to stop here
            // We don't increase the read_buff_ptr since we didnt use the point
            cluster->size = cluster_w_idx;
            break;
        }
        else
        {
            // Start a new cluster
            cluster_buff[0] = *read_buff_ptr++;
            cluster_w_idx = 1;
        }
    }

    cluster->size = cluster_w_idx > min_points ? cluster_w_idx : 0;
    return (size_t)(read_buff_ptr - buff);
}

int32_t cluster_find_clusters(const point_t *buff,
                              size_t buff_size,
                              cluster_t *clusters,
                              size_t max_clusters,
                              distance_t max_dist_point_mm,
                              distance_t max_cluster_size_mm,
                              uint16_t min_points,
                              rectangle_t *rectangle)
{
    // Check input parameters
    if (buff == NULL || buff_size == 0 || clusters == NULL)
    {
        SYSTEM_ASSERT(false);
        return -1;
    }

    // Number of cluster found or/and write index for the array of clusters
    size_t cluster_idx = 0;

    // Initialize var
    size_t remaining_size = buff_size;
    const point_t *buff_read_ptr = buff;

    while (remaining_size != 0 && cluster_idx < max_clusters)
    {
        size_t size_read = cluster_find_cluster(buff_read_ptr,
                                                remaining_size,
                                                &clusters[cluster_idx],
                                                max_dist_point_mm,
                                                max_cluster_size_mm,
                                                min_points,
                                                rectangle);

        buff_read_ptr += size_read;
        remaining_size -= size_read;

        if (clusters[cluster_idx].size > 0)
        {
            if (cluster_idx == max_clusters - 1)
            {
                // We have reached the maximum number of cluster we can store, we stop here
                LOGW("Cluster: reached the maximum number of clusters (%zu), stopping here",
                     max_clusters);
                break;
            }
            // Buffer found
            cluster_idx++;
        }
    }

    return (int32_t)cluster_idx;
}

distance_t cluster_calculate_diameter(const cluster_t *cluster)
{
    if (cluster == NULL || cluster->size < 2)
    {
        return 0;
    }

    float max_distance_sq = 0;

    for (size_t i = 0; i < cluster->size; i++)
    {
        for (size_t j = i + 1; j < cluster->size; j++)
        {
            distance_t distance_sq =
                point_distance_square(&cluster->points[i], &cluster->points[j]);
            if (distance_sq > max_distance_sq)
            {
                max_distance_sq = distance_sq;
            }
        }
    }

    return sqrtf(max_distance_sq);
}

void cluster_calculate_center(const cluster_t *cluster, point_t *center)
{
    if (cluster == NULL || center == NULL || cluster->size == 0)
    {
        SYSTEM_ASSERT(false);
        return;
    }

    center->x = 0;
    center->y = 0;

    for (size_t i = 0; i < cluster->size; i++)
    {
        center->x += cluster->points[i].x;
        center->y += cluster->points[i].y;
    }

    center->x /= (float)cluster->size;
    center->y /= (float)cluster->size;
}

/* ********************************** Public callback functions definitions ************************* */
