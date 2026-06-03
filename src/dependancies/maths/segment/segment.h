/**
 * @file segment.h
 * @brief Segment utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see segment.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/point/point.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @struct segment_t
 * @brief Structure représentant un segment dans un plan 2D
 */
typedef struct
{
    point_t a;
    point_t b;
} segment_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Fonction permettant d'optenir l'intersection de deux segments
 * @param[in] s1 Premier segment
 * @param[in] s2 Deuxième segment
 * @param[out] intersection Le point d'intersection
 * @return true Si les segments se croisent
 * @warning Cette fonction ne semble pas précise
 * @todo À vérifier et corriger si besoin
 */
bool segments_get_intersection(const segment_t *s1, const segment_t *s2, point_t *intersection);

/**
 * @brief Fonction permettant de savoir si deux segments se croisent
 * @param[in] s1 Premier segment
 * @param[in] s2 Deuxième segment
 * @return true Si les segments se croisent
 * @return false Si les segments ne se croisent pas
 */
bool segments_intersects(const segment_t *s1, const segment_t *s2);

/**
 * @brief Fonction permettant de savoir si un point est sur un segment
 * @param[in] seg Le segment
 * @param[in] tested_point Le point à tester
 * @return true Si le point est sur le segment
 * @return false Si le point n'est pas sur le segment
 */
bool point_on_segment(const segment_t *seg, const point_t *tested_point);

/**
 * @brief Fonction permettant d'optenir l'intersection stricte de deux segments
 * @param[in] s1 Premier segment
 * @param[in] s2 Deuxième segment
 * @param[out] intersection Le point d'intersection
 * @return true Si les segments se croisent
 */
bool segments_get_strictly_intersection(const segment_t *s1,
                                        const segment_t *s2,
                                        point_t *intersection);

/**
 * @brief Fonction permettant de savoir si deux segments se croisent strictement
 * @note Deux segments se croisent strictement si ils se croisent mais ne se superposent pas
 * @param[in] s1 Premier segment
 * @param[in] s2 Deuxième segment
 * @return true Si les segments se croisent strictement
 * @return false Si les segments ne se croisent pas strictement
 */
bool segments_strictly_intersects(const segment_t *s1, const segment_t *s2);

/**
 * @brief Fonction permettant de savoir si deux segments sont parallèles
 * @param[in] seg1 Premier segment
 * @param[in] seg2 Deuxième segment
 * @return true Si les segments sont parallèles
 * @return false Si les segments ne sont pas parallèles
 */
bool segments_parallel(const segment_t *seg1, const segment_t *seg2);

/**
 * @brief Fonction permettant de récupérer le point milieu d'un segment
 * @param[in] s Le segment dont on veut le point milieu
 * @param[out] middle Le point milieu du segment
 */
void segment_middle(const segment_t *s, point_t *middle);

/**
 * @brief proj_on_line
 * @param[in] seg Le segment définissant la droite sur laquelle faire le projeté orthogonal
 * @param[in] pointToProj Le point à projeter
 * @param[out] proj Le projeté orthogonal
 */
void proj_on_line(const segment_t *seg, const point_t *pointToProj, point_t *proj);

/**
 * @brief proj_strictly_behind_line
 * @param[in] seg Le segment définissant la droite sur laquelle faire le projeté orthogonal
 * @param[in] pointToProj Le point à projeter
 * @param[out] proj Le projeté orthogonal
 *
 * @warning Observation faite le 11/05/2025 : La fonction ne semble pas fonctionner correctement. Pas le temps de fix sorry.
 */
void proj_strictly_behind_line(const segment_t *seg, const point_t *pointToProj, point_t *proj);

/**
 * @brief Fonction permettant de savoir si deux segments sont colinéaires
 *
 * @param[in] seg1 Premier segment
 * @param[in] seg2 Deuxième segment
 * @return true Si les segments sont colinéaires
 * @return false Si les segments ne sont pas colinéaires
 */
bool segments_colinear(const segment_t *seg1, const segment_t *seg2);

/* ******************************************* Public callback functions declarations ************************************ */
