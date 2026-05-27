/**
 * @file dj_polygon.h
 * @brief Polygon used for dj
 * @author Cyprien Ménard
 * @date 16/12/2024
 * @see dj_polygon.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include <stdint.h>

LOG_REGISTER("utils/dj/polygon");

/* **************************************************** Private macros *************************************************** */

#define DJ_POLYGON_CONTROL_POLYGON(polygon, to_return)                                             \
    SYSTEM_ASSERT(polygon != NULL);                                                                \
    if (polygon->nb_points > DJ_POLYGON_MAX_POINTS)                                                \
    {                                                                                              \
        LOGD("dj polygon error: too many points");                                                 \
        return to_return;                                                                          \
    }

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_polygon_to_geometry_polygon(const dj_polygon_t *polygon, polygon_t *out_polygon)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, );
    out_polygon->nb_points = polygon->nb_points;
    out_polygon->points = (point_t *)polygon->points;
}

void dj_geometry_polygon_to_dj_polygon(const polygon_t *polygon, dj_polygon_t *out_polygon)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(out_polygon != NULL);
    if (polygon->nb_points > DJ_POLYGON_MAX_POINTS)
    {
        LOGD("dj polygon error: too many points");
        return;
    }
    out_polygon->nb_points = polygon->nb_points;
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        out_polygon->points[i] = polygon->points[i];
    }
}

point_t *dj_polygon_get_point(dj_polygon_t *polygon, uint16_t index)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, NULL);
    if (index < polygon->nb_points)
    {
        return &polygon->points[index];
    }
    else
    {
        LOGD("dj polygon error: index out of bounds");
        return NULL;
    }
}

void dj_polygon_set_point(dj_polygon_t *polygon, uint16_t index, point_t point)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, );
    if (index < polygon->nb_points)
    {
        polygon->points[index] = point;
    }
    else
    {
        LOGD("dj polygon error: index out of bounds");
    }
}

uint16_t dj_polygon_get_nb_points(const dj_polygon_t *polygon)
{
    DJ_POLYGON_CONTROL_POLYGON(polygon, 0);
    return polygon->nb_points;
}

void dj_polygon_copy(dj_polygon_t *dest, const dj_polygon_t *src)
{
    DJ_POLYGON_CONTROL_POLYGON(src, );
    dest->nb_points = src->nb_points;
    for (uint16_t i = 0; i < src->nb_points; i++)
    {
        dest->points[i] = src->points[i];
    }
}

/* ***************************************** Public callback functions definitions *************************************** */
