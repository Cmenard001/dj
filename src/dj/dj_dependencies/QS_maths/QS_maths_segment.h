#ifndef QS_MATHS_SEGMENT_H
#define QS_MATHS_SEGMENT_H

#include "../QS_macro.h"
#include "../QS_types.h"
#include "QS_maths_point.h"

typedef struct
{
    GEOMETRY_point_t a;
    GEOMETRY_point_t b;
} GEOMETRY_segment_t;

/**
 * @brief Fonction permettant d'optenir l'intersection de deux segments
 *
 * @param s1, s2 Les deux segments
 * @param intersection Le point d'intersection
 * @return true Si les segments se croisent
 */
bool GEOMETRY_segments_get_intersection(GEOMETRY_segment_t s1, GEOMETRY_segment_t s2, GEOMETRY_point_t *intersection);

/**
 * @brief Fonction permettant de savoir si deux segments se croisent
 *
 * @param s1, s2 Les deux segments
 * @return true Si les segments se croisent
 * @return false Si les segments ne se croisent pas
 */
bool GEOMETRY_segments_intersects(GEOMETRY_segment_t s1, GEOMETRY_segment_t s2);

/**
 * @brief Fonction permettant de savoir si un point est sur un segment
 *
 * @param seg Le segment
 * @param tested_point Le point à tester
 * @return true Si le point est sur le segment
 * @return false Si le point n'est pas sur le segment
 */
bool GEOMETRY_point_on_segment(GEOMETRY_segment_t seg, GEOMETRY_point_t tested_point);

/**
 * @brief Fonction permettant d'optenir l'intersection stricte de deux segments
 *
 * @param s1, s2 Les deux segments
 * @param intersection Le point d'intersection
 * @return true Si les segments se croisent
 */
bool GEOMETRY_segments_get_strictly_intersection(GEOMETRY_segment_t s1,
                                                 GEOMETRY_segment_t s2,
                                                 GEOMETRY_point_t *intersection);

/**
 * @brief Fonction permettant de savoir si deux segments se croisent strictement
 * @note Deux segments se croisent strictement si ils se croisent mais ne se superposent pas
 *
 * @param s1, s2 Les deux segments
 * @return true Si les segments se croisent strictement
 * @return false Si les segments ne se croisent pas strictement
 */
bool GEOMETRY_segments_strictly_intersects(GEOMETRY_segment_t s1, GEOMETRY_segment_t s2);

/**
 * @brief Fonction permettant de savoir si deux segments sont parallèles
 *
 * @param seg1, seg2 Les deux segments
 * @return true Si les segments sont parallèles
 * @return false Si les segments ne sont pas parallèles
 */
bool GEOMETRY_segments_parallel(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2);

/**
 * @brief Fonction permettant de récupérer le point milieu d'un segment
 *
 * @param s Le segment dont on veut le point milieu
 * @return GEOMETRY_point_t Le point milieu du segment
 */
GEOMETRY_point_t GEOMETRY_segment_middle(GEOMETRY_segment_t s);

/**
 * @brief GEOMETRY_proj_on_line
 * @param seg Le segment définissat la droite sur laquelle faire le projeté orthogonal
 * @param pointToProj Le point à projeté
 * @return Le projeté orthogonale
 */
GEOMETRY_point_t GEOMETRY_proj_on_line(GEOMETRY_segment_t seg, GEOMETRY_point_t pointToProj);

/**
 * @brief GEOMETRY_proj_strictly_behind_line
 * @param seg Le segment définissat la droite sur laquelle faire le projeté orthogonal
 * @param pointToProj Le point à projeté
 * @return Le projeté orthogonale
 *
 * @warning Observation faite le 11/05/2025 : La fonction ne semble pas fonctionner correctement. Pas le temps de fix sorry.
 */
GEOMETRY_point_t GEOMETRY_proj_strictly_behind_line(GEOMETRY_segment_t seg, GEOMETRY_point_t pointToProj);

/**
 * @brief Fonction permettant de savoir si deux segments sont colinéaires
 *
 * @param seg1, seg2 Les deux segments
 * @return true Si les segments sont colinéaires
 * @return false Si les segments ne sont pas colinéaires
 */
bool GEOMETRY_segments_colinear(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2);

#endif
