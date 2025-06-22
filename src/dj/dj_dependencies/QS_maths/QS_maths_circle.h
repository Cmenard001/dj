#ifndef QS_MATHS_CIRCLE_H
#define QS_MATHS_CIRCLE_H

#include "../QS_macro.h"
#include "../QS_types.h"
#include "QS_maths_point.h"
#include "QS_maths_segment.h"


typedef struct
{
    GEOMETRY_point_t c;
    uint16_t r;
} GEOMETRY_circle_t;

/**
 * @brief Fonction permettant de récupérer un cercle à partir de son centre et de son rayon
 *
 * @param diameter Le diamètre du cercle
 * @return GEOMETRY_circle_t Le cercle récupéré
 */
GEOMETRY_circle_t GEOMETRY_circle_from_diameter(GEOMETRY_segment_t diameter);

/**
 * @brief Fonction permettant de récupérer un cercle à partir de trois points
 *
 * @param A, B, C Les trois points
 * @return GEOMETRY_point_t Le cercle récupéré
 */
GEOMETRY_point_t GEOMETRY_circle_from_3_points(GEOMETRY_point_t A, GEOMETRY_point_t B, GEOMETRY_point_t C);

/**
 * @brief Fonction permettant de récupérer les intersections de deux cercles
 *
 * @param c0, c1 Les deux cercles
 * @return GEOMETRY_segment_t Les intersections des deux cercles
 */
GEOMETRY_segment_t GEOMETRY_circle_intersections(GEOMETRY_circle_t c0, GEOMETRY_circle_t c1);

/**
 * @brief Fonction permettant de savoir si un point est dans un cercle
 *
 * @param current Le point à tester
 * @param circle Le cercle
 * @return true Si le point est dans le cercle
 * @return false Si le point n'est pas dans le cercle
 */
bool GEOMETRY_is_in_circle(GEOMETRY_point_t current, GEOMETRY_circle_t circle);

#endif
