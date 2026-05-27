/**
 * @file dj_polygon.h
 * @brief Polygon used for dj
 * @author Cyprien Ménard
 * @date 16/12/2024
 * @see dj_polygon.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_config.h"
#include "utils/maths/point/point.h"
#include "utils/maths/polygon/polygon.h"
#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Macro to create a dj polygon
 * @param ... Points of polygons (ordered)
 */
#define DJ_CREATE_POLYGON(...)                                                                     \
    (dj_polygon_t)                                                                                 \
    {                                                                                              \
        .nb_points = sizeof((point_t[]){__VA_ARGS__}) / sizeof(point_t), .points = { __VA_ARGS__ } \
    }

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a polygon
 * @details A polygon is defined by a number of points and an array of points
 * @note A dj_polygon_t can be converted to a polygon_t
 * @see dj_polygon_to_geometry_polygon
 */
typedef struct
{
    uint16_t nb_points;
    point_t points[DJ_POLYGON_MAX_POINTS];
} dj_polygon_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Convert a dj polygon to a geometry polygon
 * @param polygon Polygon to convert
 * @param out_polygon Polygon to fill
 * @warning memory of polygon points is not copied, only the pointer is assigned
 * so, out_polygon points should not be freed
 */
void dj_polygon_to_geometry_polygon(const dj_polygon_t *polygon, polygon_t *out_polygon);

/**
 * @brief Convert a geometry polygon to a dj polygon
 * @param polygon Geometry polygon to convert
 * @param out_polygon Dj polygon to fill
 */
void dj_geometry_polygon_to_dj_polygon(const polygon_t *polygon, dj_polygon_t *out_polygon);

/**
 * @brief Get a point of a polygon
 * @param polygon Polygon to get the point from
 * @param index Index of the point to get
 * @return point_t* Point at the given index
 */
point_t *dj_polygon_get_point(dj_polygon_t *polygon, uint16_t index);

/**
 * @brief Set a point of a polygon
 * @param polygon Polygon to set the point to
 * @param index Index of the point to set
 * @param point Point to set
 */
void dj_polygon_set_point(dj_polygon_t *polygon, uint16_t index, point_t point);

/**
 * @brief Get the number of points of a polygon
 * @param polygon Polygon to get the number of points from
 * @return uint16_t Number of points of the polygon
 */
uint16_t dj_polygon_get_nb_points(const dj_polygon_t *polygon);

/**
 * @brief Copy a polygon to another
 * @param dst Polygon to copy to
 * @param src Polygon to copy from
 */
void dj_polygon_copy(dj_polygon_t *dst, const dj_polygon_t *src);

/* ******************************************* Public callback functions declarations ************************************ */
