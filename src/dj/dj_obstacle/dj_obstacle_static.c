/**
 * @file dj_obstacle_static.c
 * @brief Static obstacle of dj
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_static.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_obstacle_static.h"
#include "system/assert/system_assert.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include <stdlib.h>
/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_static_init(dj_obstacle_static_t *obstacle,
                             dj_polygon_t *shape,
                             dj_static_obstacle_id_t id,
                             distance_t smooth_extraction_radius,
                             bool is_enabled)
{
    SYSTEM_ASSERT(obstacle != NULL);
    SYSTEM_ASSERT(shape != NULL);

    dj_obstacle_init((dj_obstacle_t *)obstacle, DJ_OBSTACLE_TYPE_STATIC, is_enabled);
    obstacle->id = id;
    obstacle->shape.nb_points = dj_polygon_get_nb_points(shape);
    for (uint8_t i = 0; i < dj_polygon_get_nb_points(shape); i++)
    {
        obstacle->shape.points[i] = *dj_polygon_get_point(shape, i);
    }
    obstacle->smooth_extraction_radius = smooth_extraction_radius;
}

/* ******************************************* Public callback functions declarations ************************************ */
