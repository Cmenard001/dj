/**
 * @file polygon.h
 * @brief Polygon utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see polygon.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/distance/distance.h"
#include "utils/maths/point/point.h"
#include "utils/maths/segment/segment.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Macro to create a polygon
 * @param ... All points of the polygon in order (point_t)
 */
#define POLYGON_CREATE(...)                                                                        \
    (polygon_t)                                                                                    \
    {                                                                                              \
        .points = (point_t[]){__VA_ARGS__},                                                        \
        .nb_points = sizeof((point_t[]){__VA_ARGS__}) / sizeof(point_t)                            \
    }

/**
 * @brief Create an empty polygon with
 */
#define POLYGON_CREATE_N_SUMMIT(_n, _polygon_name)                                                 \
    point_t _polygon_name##weird_not_used[_n];                                                     \
    polygon_t _polygon_name = {.points = _polygon_name##weird_not_used, .nb_points = _n}

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a rectangle
 * @details A rectangle is defined by two points
 */
typedef struct
{
    point_t p1;
    point_t p2;
} rectangle_t;

/**
 * @brief Structure representing a polygon
 * @details A polygon is defined by a number of points and an array of points
 */
typedef struct
{
    uint16_t nb_points;
    point_t *points;
} polygon_t;

/**
 * @brief Work data for the polygon_from_cluster function to avoid dynamic allocation in the function
 * @details A buffer must be allocated by the caller with the size equal to the number of points
 */
typedef struct
{
    int32_t furthest_idx;
    float max_dist_sq;
} polygon_from_cluster_work_data_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Fonction qui permet de savoir si un point est dans un rectangle
 * @param[in] rect Le rectangle
 * @param[in] current Le point à tester
 * @return true Si le point est dans le rectangle
 * @return false Si le point n'est pas dans le rectangle
 */
bool is_in_rectangle(const rectangle_t *rect, const point_t *current);

/**
 * @brief is_in_quadri
 * @pre Les points doivent être donnés pour que le quadrilatère soit connexe (chaque point voit tous les points)
 * @param[in] points Le tableau de point du quadrilatère (doit être dans l'ordre)
 * @param[in] tested_point Le point à tester
 * @return si le point est dans le quadrilatère
 */
bool is_in_quadri(const point_t points[4], const point_t *tested_point);

/**
 * @brief is_in_polygon
 * @param[in] polygon Les points du polygone
 * @param[in] nb_summits Le nombre de sommets
 * @param[in] tested_point Le point que l'on veut tester si il est dans le polygone
 * @param[in] out_point Un point dont on est sûr qu'il est en dehors du polygone (par exemple en dehors du terrain)
 * @param[out] stock_intersections Stockage du nombre d'intersections
 * @return true si le point est dans le polygone, false sinon
 */
bool is_in_polygon(const point_t polygon[],
                   uint8_t nb_summits,
                   const point_t *tested_point,
                   const point_t *out_point,
                   uint8_t *stock_intersections);

/**
 * @brief Test if a point is strictly in a polygon
 * @note If the point is on the edge of the polygon, it is considered as outside the polygon
 * @param[in] polygon Les points du polygone
 * @param[in] nb_summits Le nombre de sommets
 * @param[in] tested_point Le point que l'on veut tester si il est dans le polygone
 * @param[in] out_point Un point dont on est sûr qu'il est en dehors du polygone (par exemple en dehors du terrain)
 * @param[out] stock_intersections Stockage du nombre d'intersections
 * @return true si le point est strictement dans le polygone, false sinon
 */
bool is_strictly_in_polygon(const point_t polygon[],
                            uint8_t nb_summits,
                            const point_t *tested_point,
                            const point_t *out_point,
                            uint8_t *stock_intersections);

/**
 * @brief Get the intersection points between a segment and a polygon
 * @param[in] polygon The polygon to test
 * @param[in] nb_summits The number of summits in the polygon
 * @param[in] segment The segment to test
 * @param[out] intersection_points The points of intersection between the segment and the polygon (must
 * be allocated by the caller with a size of at least nb_summits)
 * @param[out] nb_intersection_points The number of intersection points found
 */
void polygon_segment_get_intersection(const point_t polygon[],
                                      uint16_t nb_summits,
                                      const segment_t *segment,
                                      point_t *intersection_points,
                                      uint16_t *nb_intersection_points);

/**
 * @brief Get the strict intersection points between a segment and a polygon
 * @note If the segment is on the edge of the polygon, it is not considered as intersecting the polygon
 * @param[in] polygon The polygon to test
 * @param[in] nb_summits The number of summits in the polygon
 * @param[in] segment The segment to test
 * @param[out] intersection_points The points of intersection between the segment and the polygon (must
 * be allocated by the caller with a size of at least nb_summits)
 * @param[out] nb_intersection_points The number of intersection points found
 */
void polygon_strict_segment_get_intersection(const point_t polygon[],
                                             uint16_t nb_summits,
                                             const segment_t *segment,
                                             point_t *intersection_points,
                                             uint16_t *nb_intersection_points);

/**
 * @brief Function to know if a segment intersects a polygon
 * @note If the segment is on the edge of the polygon, it is considered as intersecting the polygon
 * @param[in] polygon Polygon to test
 * @param[in] segment Segment to test
 * @return true If the segment intersects the polygon
 * @return false If the segment does not intersect the polygon
 */
bool polygon_intersects_segment(const polygon_t *polygon, const segment_t *segment);

/**
 * @brief Function to know if a segment strictly intersects a polygon
 * @note If the segment is on the edge of the polygon, it is not considered as intersecting the polygon
 * @warning If the segment is very close to a summit but inside the polygon,
    it can be not detected as intersecting the polygon due to approximation
 * @param[in] polygon Polygon to test
 * @param[in] segment Segment to test
 * @return true If the segment intersects the polygon
 * @return false If the segment does not intersect the polygon
 */
bool polygon_strictly_intersects_segment(const polygon_t *polygon, const segment_t *segment);

/**
 * @brief Create a polygon
 * @param nb_points number of points in the polygon
 * @return polygon_t* pointer to the polygon created
 * @note The polygon must be destroyed with polygon_destroy
 * @note Polygon points are not initialized
 */
polygon_t *polygon_create(uint16_t nb_points);

/**
 * @brief Destroy a polygon
 * @param polygon pointer to the polygon to destroy
 */
void polygon_destroy(polygon_t *polygon);

/**
 * @brief Function to check if a point is a node of a polygon
 * @param[in] polygon The polygon to check
 * @param[in] point The point to check
 * @return The index of the point in the polygon if it is a node
 * @return -1 if the point is not a node of the polygon
 */
int16_t polygon_contains_point(const polygon_t *polygon, const point_t *point);

/**
 * @brief Function to project a point on a polygon
 * @details The projected point will be the closest projection of the point on each edge of the polygon
 * @param[in] polygon The polygon to project the point on
 * @param[in] point The point to project
 * @param[out] projected The projected point on the polygon
 */
void polygon_project_point(const polygon_t *polygon, const point_t *point, point_t *projected);

/**
 * @brief Project a point on a polygon with a given angle
 * @details The projected point will be the closest projection of the point on each edge of the polygon
 * that forms an angle with the point smaller than the given angle
 * @param[in] polygon The polygon to project the point on
 * @param[in] point The point to project
 * @param[in] angle The maximum angle between the point and the edge of the polygon to consider for
 * the projection (in radians)
 * @param[out] projected The projected point on the polygon
 * @return true if a projection was found, false otherwise (if no edge of the polygon forms an angle
 * smaller than the given angle with the point)
 */
bool polygon_project_point_with_angle(const polygon_t *polygon,
                                      const point_t *point,
                                      angle_t angle,
                                      point_t *projected);

/**
 * @brief Set the points of a polygon in trigonometric order
 * @details This function will reverse the order of the points if the polygon is in hourly order
 * @param[in,out] polygon : the polygon to set in trigonometric order
 */
void polygon_set_in_trigonometric_order(polygon_t *polygon);

/**
 * @brief Fusion two polygons
 * @details This function will create a new polygon that is the fusion of the two polygons
 * @param[in,out] polygon1 : the first polygon
 * @param[in,out] polygon2 : the second polygon
 * @param[out] out_polygon : the polygon that will be the fusion of the two polygons
 * @param[in] nb_points_max_in_out_polygon : the maximum number of points in the output polygon
 * @note if the output polygon is not big enough, the function will return false
 * @return true if the fusion was successful, false otherwise
 * @warning Enough points in the output polygon must be allocated.
 * @warning The input polygons can be modified if they are not in trigonometric order
 * @warning Do not use the same polygon for an input and output
 * @note The output polygon will be in trigonometric order
 */
bool polygon_fusion(polygon_t *polygon1,
                    polygon_t *polygon2,
                    polygon_t *out_polygon,
                    uint8_t nb_points_max_in_out_polygon);

/**
 * @brief Function to find the closest point of a polygon to a point
 * @details The function can return a summit or a point on the edge of the polygon
 * @param[in] polygon The polygon to search in
 * @param[in] point The point to search for
 * @param[out] closest The closest point of the polygon to the point
 */
void polygon_find_closest_point(const polygon_t *polygon, const point_t *point, point_t *closest);

/**
 * @brief Create a regular polygon at the specifed [position]
 * @param polygon the polygon instance to use. Make sure to create the point array
 * @param nb_summit number of summit for the regular polygon
 * @param radius the "radius" for the regular polygon (from center to summit)
 * @param position the center position for the regular polygon
 */
void polygon_create_regular(polygon_t *polygon,
                            uint32_t nb_summit,
                            distance_t radius,
                            const point_t *position);

/**
 * @brief Oversize a polygon by a specified offset
 * @param polygon the polygon to oversize
 * @param output_polygon the polygon that will be the oversize of the input polygon
 * @param nb_angle_points number of angle points to use for the oversizing (more points = more precision)
 * @param max_points the maximum number of points allowed in the polygon after oversizing
 * @param offset the offset to apply (positive values increase size, negative values decrease size)
 * @return true if the oversizing was successful, false otherwise (not enough points)
 * @note The polygon points array must be big enough to hold the new points
 * @note The polygon will be modified in place
 */
bool polygon_oversize(polygon_t *polygon,
                      polygon_t *output_polygon,
                      uint8_t nb_angle_points,
                      uint16_t max_points,
                      distance_t offset);

/**
 * @brief Generate a convex hull from a set of points using the Gift Wrapping algorithm (a.k.a. Jarvis March)
 * @param points the set of points to generate the convex hull from
 * @param nb_points the number of points in the set
 * @param out_polygon the polygon that will be the convex hull of the set of points
 * @param max_points the maximum number of points allowed in the output polygon
 * @return true if the convex hull was successfully generated, false otherwise (not enough points in the output polygon)
 * @note The output polygon will be in trigonometric order
 * @note The output polygon points array must be big enough to hold the new points
 */
bool polygon_convex_hull(const point_t *points,
                         uint16_t nb_points,
                         polygon_t *out_polygon,
                         uint16_t max_points);

/**
 * @brief Create a polygon from a cluster of points
 * @param points the set of points to generate the polygon from
 * @param nb_points the number of points in the set
 * @param out_polygon the polygon that will be generated from the cluster of points
 * @param max_points the maximum number of points allowed in the output polygon
 * @param work_data a work data structure to avoid dynamic allocation in the function. A buffer must
 * be allocated by the caller with the size equal to the number of points
 * @return true if the polygon was successfully generated, false otherwise (not enough points in the output polygon)
 * @note The first difference with polygon_convex_hull is that this function will not necessarily return
 * a convex polygon, but it will try to follow the shape of the cluster of points as much as possible
 * @note The second difference with polygon_convex_hull is that this function will not fail if the number
 * of points in the output polygon is not enough to hold all the points of the convex hull, but it will
 * try to reduce the number of points.
 */
bool polygon_from_cluster(const point_t *points,
                          uint16_t nb_points,
                          polygon_t *out_polygon,
                          uint16_t max_points,
                          polygon_from_cluster_work_data_t *work_data);

/* ******************************************* Public callback functions declarations ************************************ */
