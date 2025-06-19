/**
 * @file QS_maths_cluster.h
 * @author Théo Magne
 * @date 29/05/2025
 * @see QS_maths_cluster.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#ifndef QS_MATHS_CLUSTER_H
#define QS_MATHS_CLUSTER_H

/* ************************************************ Includes **************************************** */

#include "QS_maths_polygon.h"

/* ********************************************** Public macros ************************************* */

#define GEOMETRY_CLUSTER_MAX_POINTS 128 /**< Maximum number of points in a cluster. */

/* ******************************************* Public types definition ****************************** */

typedef struct
{
    GEOMETRY_point_t points[GEOMETRY_CLUSTER_MAX_POINTS]; /**< Array of points in the cluster. */
    size_t size;                                          /**< Number of points in the cluster. */
} GEOMETRY_cluster_t;

/* **************************************** Public functions declarations *************************** */

/**
 * @brief Search a cluser in a buffer of points.
 *
 * @param buff buffer of points to search in
 * @param size size of the provided buffer
 * @return this is the read index off the buffer (in case you want to chain them)
 */
size_t GEOMETRY_CLUSTER_find_cluster(GEOMETRY_point_t *buff,
                                     size_t size,
                                     GEOMETRY_cluster_t *cluster,
                                     uint16_t max_dist_point_mm,
                                     uint16_t min_points,
                                     GEOMETRY_rectangle_t *rectangle);

/**
 * @brief Finds clusters in the given dataset based on the specified parameters.
 *
 * This function analyzes the input data and identifies clusters according to the
 * clustering algorithm implemented. The results are stored in the provided output structure.
 *
 * @param data Pointer to the input data array.
 * @param data_size The number of elements in the input data array.
 * @param min_cluster_size The minimum number of elements required to form a cluster.
 * @param max_distance The maximum distance between elements to be considered in the same cluster.
 * @param[out] clusters Pointer to the output structure where the found clusters will be stored.
 * @return The number of clusters found, or a negative value if an error occurred.
 */
int GEOMETRY_CLUSTER_find_clusters(GEOMETRY_point_t *buff,
                                   size_t buff_size,
                                   GEOMETRY_cluster_t *clusters,
                                   uint16_t max_dist_point_mm,
                                   uint16_t min_points,
                                   uint16_t nb_cluster_max,
                                   GEOMETRY_rectangle_t *rectangle);

/* ************************************ Public callback functions declarations ********************** */

#endif /* QS_MATHS_CLUSTER_H */
