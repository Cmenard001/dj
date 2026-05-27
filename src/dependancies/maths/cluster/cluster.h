/**
 * @file components/utils/maths/cluster/cluster.h
 * @author Théo Magne
 * @date 29/05/2025
 * @see components/utils/maths/cluster/cluster.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ************************************************ Includes **************************************** */

#include "utils/maths/point/point.h"
#include "utils/maths/polygon/polygon.h"
#include "utils/maths/segment/segment.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ********************************************** Public macros ************************************* */

#define CLUSTER_MAX_POINTS 128 /**< Maximum number of points in a cluster. */

/* ******************************************* Public types definition ****************************** */

/**
 * @brief Structure representing a cluster of points.
 */
typedef struct
{
    point_t points[CLUSTER_MAX_POINTS]; /**< Array of points in the cluster. */
    size_t size;                        /**< Number of points in the cluster. */
} cluster_t;

/* **************************************** Public functions declarations *************************** */

/**
 * @brief Search a cluster in a buffer of points.
 * @param[in] buff buffer of points to search in
 * @param[in] size size of the provided buffer
 * @param[out] cluster Output cluster found
 * @param[in] max_dist_point_mm Maximum distance between points to be in the same cluster
 * @param[in] max_cluster_size_mm The maximum size of a cluster in mm (if the cluster is bigger than
 * this size, it will be split in multiple clusters)
 * @param[in] min_points Minimum number of points to form a cluster
 * @param[in,out] rectangle Rectangle to filter points (will be normalized)
 * @return this is the read index of the buffer (in case you want to chain them)
 */
size_t cluster_find_cluster(const point_t *buff,
                            size_t size,
                            cluster_t *cluster,
                            distance_t max_dist_point_mm,
                            distance_t max_cluster_size_mm,
                            uint16_t min_points,
                            rectangle_t *rectangle);

/**
 * @brief Finds clusters in the given dataset based on the specified parameters.
 * This function analyzes the input data and identifies clusters according to the
 * clustering algorithm implemented. The results are stored in the provided output structure.
 * @param[in] buff Pointer to the input data array.
 * @param[in] buff_size The number of elements in the input data array.
 * @param[out] clusters Pointer to the output structure where the found clusters will be stored.
 * @param[in] max_clusters The maximum number of clusters to find (the array must be allocated with this size).
 * @param[in] max_dist_point_mm The maximum distance between elements to be considered in the same cluster.
 * @param[in] max_cluster_size_mm The maximum size of a cluster in mm (if the cluster is bigger than
 * this size, it will be split into multiple clusters).
 * @param[in] min_points The minimum number of elements required to form a cluster.
 * @param[in,out] rectangle Rectangle to filter points (will be normalized)
 * @return The number of clusters found, or a negative value if an error occurred.
 */
int32_t cluster_find_clusters(const point_t *buff,
                              size_t buff_size,
                              cluster_t *clusters,
                              size_t max_clusters,
                              distance_t max_dist_point_mm,
                              distance_t max_cluster_size_mm,
                              uint16_t min_points,
                              rectangle_t *rectangle);

/**
 * @brief Calculate the diameter of a cluster.
 * @param[in] cluster The cluster for which to calculate the diameter.
 * @return The diameter of the cluster in millimeters.
 */
distance_t cluster_calculate_diameter(const cluster_t *cluster);

/**
 * @brief Calculate the center of a cluster.
 * @param [in] cluster The cluster for which to calculate the center.
 * @param [out] center The calculated center of the cluster.
 */
void cluster_calculate_center(const cluster_t *cluster, point_t *center);

/* ************************************ Public callback functions declarations ********************** */
