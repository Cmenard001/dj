/**
 * @file dj_polygon.h
 * @brief Polygon used for dj
 * @author Cyprien MÃ©nard
 * @date 16/12/2024
 * @see dj_polygon.c
 */

#ifndef __DJ_POLYGON_H__
#define __DJ_POLYGON_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_config.h"
#include "../dj_dependencies/dj_dependencies.h"

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Macro to create a dj polygon
 * @param ... Points of polygons (ordered)
 */
#define DJ_CREATE_POLYGON(...)                                                                                              \
    (dj_polygon_t)                                                                                                          \
    {                                                                                                                       \
        .nb_points = sizeof((GEOMETRY_point_t[]){__VA_ARGS__}) / sizeof(GEOMETRY_point_t), .points = { __VA_ARGS__ }        \
    }

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a polygon
 * @details A polygon is defined by a number of points and an array of points
 * @note A dj_polygon_t can be converted to a GEOMETRY_polygon_t
 * @see dj_polygon_to_geometry_polygon
 */
typedef struct
{
    uint16_t nb_points;
    GEOMETRY_point_t points[DJ_POLYGON_MAX_POINTS];
} dj_polygon_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Convert a GEOMETRY_polygon_t to a dj_polygon_t
 *
 * @param polygon Polygon to convert
 * @param out_polygon Polygon to fill
 */
void dj_polygon_to_geometry_polygon(dj_polygon_t *polygon, GEOMETRY_polygon_t *out_polygon);

/**
 * @brief Get a point of a polygon
 *
 * @param polygon Polygon to get the point from
 * @param index Index of the point to get
 * @return GEOMETRY_point_t* Point at the given index
 */
GEOMETRY_point_t *dj_polygon_get_point(dj_polygon_t *polygon, uint16_t index);

/**
 * @brief Set a point of a polygon
 *
 * @param polygon Polygon to set the point to
 * @param index Index of the point to set
 * @param point Point to set
 */
void dj_polygon_set_point(dj_polygon_t *polygon, uint16_t index, GEOMETRY_point_t point);

/**
 * @brief Get the number of points of a polygon
 *
 * @param polygon Polygon to get the number of points from
 * @return uint16_t Number of points of the polygon
 */
uint16_t dj_polygon_get_nb_points(dj_polygon_t *polygon);

/**
 * @brief Copy a polygon to another
 *
 * @param dst Polygon to copy to
 * @param src Polygon to copy from
 */
void dj_polygon_copy(dj_polygon_t *dst, dj_polygon_t *src);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
