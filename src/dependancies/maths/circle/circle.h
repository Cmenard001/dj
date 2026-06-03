/**
 * @file circle.h
 * @brief Circles utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see circle.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/angle/angle.h"
#include "utils/maths/point/point.h"
#include "utils/maths/segment/segment.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @struct circle_t
 * @brief Structure représentant un cercle
 * Un cercle est défini par son centre et son rayon
 */
typedef struct
{
    point_t c;
    angle_t r;
} circle_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Fonction permettant de récupérer un cercle à partir de son centre et de son rayon
 * @param[in] diameter Le diamètre du cercle
 * @param[out] out Le cercle récupéré
 */
void circle_from_diameter(const segment_t *diameter, circle_t *out);

/**
 * @brief Fonction permettant de récupérer un cercle à partir de trois points
 * @param[in] a Premier point
 * @param[in] b Deuxième point
 * @param[in] c Troisième point
 * @param[out] out Le cercle récupéré
 * @return true Si le cercle a pu être calculé
 */
bool circle_from_3_points(const point_t *a, const point_t *b, const point_t *c, circle_t *out);

/**
 * @brief Fonction permettant de récupérer les intersections de deux cercles
 * @param[in] c0 Premier cercle
 * @param[in] c1 Deuxième cercle
 * @param[out] out Les points d'intersection (0,0) si pas d'intersection
 * @return true Si les cercles se croisent
 * @return false Si les cercles ne se croisent pas
 */
bool circle_intersections(const circle_t *c0, const circle_t *c1, segment_t *out);

/**
 * @brief Fonction permettant de savoir si un point est dans un cercle
 * @param[in] current Le point à tester
 * @param[in] circle Le cercle
 * @return true Si le point est dans le cercle
 * @return false Si le point n'est pas dans le cercle
 */
bool is_in_circle(const point_t *current, const circle_t *circle);

/* ******************************************* Public callback functions declarations ************************************ */
