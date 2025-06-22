#ifndef QS_MATHS_POINT_H
#define QS_MATHS_POINT_H

#include "../QS_macro.h"
#include "../QS_types.h"

typedef struct
{
    int16_t x;
    int16_t y;
} GEOMETRY_point_t;

/**
 * @brief Fonction permettant de comparer les coordonnées de deux points
 *
 * @param a Premier point
 * @param b Deuxième point
 * @return true Si les deux points ont les mêmes coordonnées, false sinon
 */
bool GEOMETRY_point_compare(GEOMETRY_point_t a, GEOMETRY_point_t b);

/**
 * @brief Fonction permettant de récupérer la distance entre deux points
 *
 * @param a, b Les deux points
 * @return uint16_t La distance entre les deux points
 */
uint16_t GEOMETRY_distance(GEOMETRY_point_t a, GEOMETRY_point_t b);

/**
 * @brief Fonction permettant de récupérer la distance au carré entre deux points
 *
 * @param a, b Les deux points
 * @return uint32_t La distance au carré entre les deux points
 */
uint32_t GEOMETRY_distance_square(GEOMETRY_point_t a, GEOMETRY_point_t b);

/**
 * @brief Fonction permettant de récupérer la distance entre deux points
 *        en utilisant le théorème de pythagore
 *
 * @param d1, d2 Les deux distances
 * @return uint32_t La distance entre les deux points
 */
uint32_t GEOMETRY_pythagore(uint32_t d1, uint32_t d2);

/**
 * @brief Fonction permettant de récupérer la distance de Manhattan entre deux points
 *
 * @param a, b Les deux points
 * @return uint16_t La distance de Manhattan entre les deux points
 */
uint16_t GEOMETRY_manhattan_distance(GEOMETRY_point_t a, GEOMETRY_point_t b);

/**
 * @brief Fonction permettant de récupérer la distance entre deux points
 *        en utilisant des float
 *
 * @param a, b Les deux points
 * @return float La distance entre les deux points
 */
float GEOMETRY_distance_f(point_2d_t *a, point_2d_t *b);

/**
 * @brief Fonction permettant de calculer le produit vectoriel entre trois points
 *
 * @param a, b, c Les trois points
 * @return int32_t Le produit vectoriel entre les trois points
 */
int32_t GEOMETRY_cross_product(GEOMETRY_point_t a, GEOMETRY_point_t b, GEOMETRY_point_t c);

#endif
