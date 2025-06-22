/**
 * @file QS_maths_cluster.c
 * @author Théo Magne
 * @date 29/05/2025
 * @see QS_maths_cluster.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ************************************************ Includes **************************************** */
#include "QS_maths.h"

#include "QS_maths_cluster.h"

/* ********************************************* Private macros ************************************* */

/* ***************************************** Private type definition ******************************** */

/* ************************************** Private functions declarations **************************** */

/* ******************************************* Private variables ************************************ */

/* *************************************** Private functions definitions **************************** */

/**
 * @brief Square distance between two points.
 *
 * @param p1 pointer to the first point
 * @param p2 pointer to the second point
 * @return uint32_t Square of the distance between the two points.
 */
static inline uint32_t _square_distance(const GEOMETRY_point_t *p1, const GEOMETRY_point_t *p2)
{
    // Calculate the square of the distance between two points
    int dx = p1->x - p2->x;
    int dy = p1->y - p2->y;
    return (uint32_t)(dx * dx + dy * dy);
}

/* *************************************** Public functions definitions ***************************** */

size_t GEOMETRY_CLUSTER_find_cluster(GEOMETRY_point_t *buff,
                                     size_t buff_size,
                                     GEOMETRY_cluster_t *cluster,
                                     uint16_t max_dist_point_mm,
                                     uint16_t min_points,
                                     GEOMETRY_rectangle_t *rectangle)
{
    // Check input parameters
    if (buff == NULL || cluster == NULL)
    {
        assert(false);
        return 0;
    }

    // Force the rectangle with valid coordinates to make sure x1 < x2 and y1 < y2
    if (rectangle != NULL)
    {
        int16_t min_x = MIN(rectangle->x1, rectangle->x2);
        int16_t max_x = MAX(rectangle->x1, rectangle->x2);
        int16_t min_y = MIN(rectangle->y1, rectangle->y2);
        int16_t max_y = MAX(rectangle->y1, rectangle->y2);

        rectangle->x1 = min_x;
        rectangle->x2 = max_x;
        rectangle->y1 = min_y;
        rectangle->y2 = max_y;
    }

    // Reset the cluster size
    cluster->size = 0;

    // Initialize variables
    GEOMETRY_point_t *read_buff_ptr = buff;

    // This is the write index for the buffer of the cluster
    GEOMETRY_point_t *cluster_buff = cluster->points;
    size_t cluster_w_idx = 0;

    // Square of the maximum distance between two points to be considered in the same cluster
    uint32_t max_dist_point_sq_mm = max_dist_point_mm * max_dist_point_mm;

    // Initialize the cluster buffer
    cluster_buff[0] = *read_buff_ptr++;
    cluster_w_idx = 1;

    // Iterate through the points in the buffer
    while (read_buff_ptr < (buff + buff_size))
    {
        // Filter points outside the rectangle if provided
        if (rectangle != NULL)
        {
            if ((read_buff_ptr->x < rectangle->x1) || (read_buff_ptr->x > rectangle->x2)
                || (read_buff_ptr->y < rectangle->y1) || (read_buff_ptr->y > rectangle->y2))
            {
                // Skip the points outside of the rectangle
                read_buff_ptr++;
                continue;
            }
        }

        uint32_t square_distance_with_previous_point = _square_distance(read_buff_ptr, &cluster_buff[cluster_w_idx - 1]);
        if (square_distance_with_previous_point < max_dist_point_sq_mm)
        {
            // The new point is part of the cluster
            cluster_buff[cluster_w_idx] = *read_buff_ptr;
            cluster_w_idx++;
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
    return read_buff_ptr - buff;
}

int GEOMETRY_CLUSTER_find_clusters(GEOMETRY_point_t *buff,
                                   size_t buff_size,
                                   GEOMETRY_cluster_t *clusters,
                                   uint16_t max_dist_point_mm,
                                   uint16_t min_points,
                                   uint16_t nb_cluster_max,
                                   GEOMETRY_rectangle_t *rectangle)
{
    // Check input parameters
    if (buff == NULL || buff_size == 0 || clusters == NULL)
    {
        assert(false);
        return -1;
    }

    // Number of cluster found or/and write index for the array of clusters
    size_t cluster_idx = 0;

    // Initialize var
    size_t remaining_size = buff_size;
    GEOMETRY_point_t *buff_read_ptr = buff;

    while (remaining_size != 0)
    {
        size_t size_read = GEOMETRY_CLUSTER_find_cluster(
            buff_read_ptr, remaining_size, &clusters[cluster_idx], max_dist_point_mm, min_points, rectangle);

        buff_read_ptr += size_read;
        remaining_size -= size_read;

        if (clusters[cluster_idx].size > 0)
        {
            // Buffer found
            cluster_idx++;
        }
    }

    return cluster_idx;
}

/* ********************************** Public callback functions definitions ************************* */
