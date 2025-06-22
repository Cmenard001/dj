/**
 * @file dj_obstacle_static.c
 * @brief Static obstacle of dj
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_static.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle_static.h"
#include "../dj_logs/dj_logs.h"
#include "dj_polygon.h"

#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_static_init(dj_obstacle_static_t *obstacle,
                             dj_polygon_t *shape,
                             dj_static_obstacle_id_e id,
                             bool is_enabled)
{
    dj_control_non_null(obstacle, );
    dj_control_non_null(shape, );

    dj_obstacle_init((dj_obstacle_t *)obstacle, DJ_OBSTACLE_TYPE_STATIC, is_enabled);
    obstacle->m_id = id;
    obstacle->m_shape.nb_points = dj_polygon_get_nb_points(shape);
    for (uint8_t i = 0; i < dj_polygon_get_nb_points(shape); i++)
    {
        obstacle->m_shape.points[i] = *dj_polygon_get_point(shape, i);
    }
}

void dj_obstacle_static_deinit(dj_obstacle_static_t *obstacle)
{
    dj_control_non_null(obstacle, );

    // Call the mother deinit
    dj_obstacle_deinit((dj_obstacle_t *)obstacle);
}

/* ******************************************* Public callback functions declarations ************************************ */
