/**
 * @file dj_polygon.h
 * @brief Polygon used for dj
 * @author Cyprien Ménard
 * @date 16/12/2024
 * @see dj_polygon.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_polygon.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_logs/dj_logs.h"

#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

#define DJ_POLYGON_CONTROL_POLYGON(polygon, to_return)                                                                      \
    dj_control_non_null(polygon, to_return) if (polygon->nb_points > DJ_POLYGON_MAX_POINTS)                                 \
    {                                                                                                                       \
        dj_debug_printf("dj polygon error: too many points\n");                                                             \
        return to_return;                                                                                                   \
    }

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_polygon_to_geometry_polygon(dj_polygon_t *polygon, GEOMETRY_polygon_t *out_polygon)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, );
    out_polygon->nb_points = polygon->nb_points;
    out_polygon->points = polygon->points;
}

GEOMETRY_point_t *dj_polygon_get_point(dj_polygon_t *polygon, uint16_t index)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, NULL);
    if (index < polygon->nb_points)
    {
        return &polygon->points[index];
    }
    else
    {
        dj_debug_printf("dj polygon error: index out of bounds\n");
        return NULL;
    }
}

void dj_polygon_set_point(dj_polygon_t *polygon, uint16_t index, GEOMETRY_point_t point)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, );
    if (index < polygon->nb_points)
    {
        polygon->points[index] = point;
    }
    else
    {
        dj_debug_printf("dj polygon error: index out of bounds\n");
    }
}

uint16_t dj_polygon_get_nb_points(dj_polygon_t *polygon)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, 0);
    return polygon->nb_points;
}

void dj_polygon_copy(dj_polygon_t *dest, dj_polygon_t *src)
{
    DJ_POLYGON_CONTROL_POLYGON(src, );
    dest->nb_points = src->nb_points;
    for (uint16_t i = 0; i < src->nb_points; i++)
    {
        dest->points[i] = src->points[i];
    }
}

/* ***************************************** Public callback functions definitions *************************************** */
