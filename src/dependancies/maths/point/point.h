/**
 * @file point.h
 * @brief Point utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see point.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/distance/distance.h"
#include "utils/maths/vector/vector.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type définissant un point en 2D
 * @note Hérite de vector_2d_distance_t car un point est un vecteur avec une origine en (0,0)
 */
typedef vector_2d_distance_t point_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Fonction permettant de comparer les coordonnées de deux points
 * @param[in] a Premier point
 * @param[in] b Deuxième point
 * @param[in] epsilon La marge d'erreur autorisée entre les deux coordonnées
 * @return true Si les deux points ont les mêmes coordonnées, false sinon
 */
bool point_compare_with_epsilon(const point_t *a, const point_t *b, distance_t epsilon);

/**
 * @brief Fonction permettant de comparer les coordonnées de deux points
 * @param[in] a Premier point
 * @param[in] b Deuxième point
 * @return true Si les deux points ont les mêmes coordonnées, false sinon
 * @note Les distances étant des floats, une marge d'erreur par défaut est appliquée
 * @see DISTANCE_COMPARISON_EPSILON
 * @see point_compare_with_epsilon pour une version avec une epsilon personnalisée
 */
bool point_compare(const point_t *a, const point_t *b);

/**
 * @brief Fonction permettant de récupérer la distance entre deux points
 * @param[in] a Premier point
 * @param[in] b Deuxième point
 * @return La distance entre les deux points
 */
distance_t point_distance(const point_t *a, const point_t *b);

/**
 * @brief Fonction permettant de récupérer la distance au carré entre deux points
 * @param[in] a Premier point
 * @param[in] b Deuxième point
 * @return La distance au carré entre les deux points
 */
float point_distance_square(const point_t *a, const point_t *b);

/**
 * @brief Fonction permettant de récupérer la distance de Manhattan entre deux points
 * @param[in] a Premier point
 * @param[in] b Deuxième point
 * @param[out] result La distance de Manhattan entre les deux points
 * @return true si le calcul a réussi, false sinon
 */
distance_t point_manhattan_distance(const point_t *a, const point_t *b);

/**
 * @brief Fonction permettant de calculer le produit vectoriel entre trois points
 * @param[in] a Premier point
 * @param[in] b Deuxième point
 * @param[in] c Troisième point
 * @return Le produit vectoriel entre les trois points
 */
float point_cross_product(const point_t *a, const point_t *b, const point_t *c);

/**
 * @brief Change la norme d'un vecteur défini par son point d'arrivé et de départ
 * @param[in] end point de fin du vecteur
 * @param[in] start point de départ du vecteur
 * @param[out] new_end point calculé pour la nouvelle "fin" du vecteur
 * @param[in] norme la nouvelle norme du vecteur
 * @return true si le calcul a réussi, false si il y a une erreur dans les maths (point superposé par exemple)
 */
bool vector_resize_absolute(const point_t *end,
                            const point_t *start,
                            point_t *new_end,
                            distance_t norme);

/**
 * @brief Change la norme d'un vecteur défini par son point d'arrivé et de départ en prenant un compte un ofsset
 * @param[in] end point de fin du vecteur
 * @param[in] start point de départ du vecteur
 * @param[out] new_end point calculé pour la nouvelle "fin" du vecteur
 * @param[in] offset l'offset à appliquer sur la norme du vecteur
 * @return true si le calcul a réussi, false si il y a une erreur dans les maths (point superposé par exemple)
 */
bool vector_resize_relative(const point_t *end,
                            const point_t *start,
                            point_t *new_end,
                            distance_t offset);

/**
 * @brief Translate un point selon un vecteur
 * @param[in,out] point Le point à traduire
 * @param[in] vector Le vecteur de translation
 */
void point_translate(point_t *point, const vector_2d_distance_t *vector);

/**
 * @brief Compute the absolute angle between a source point and a target point
 * @param source The source point
 * @param target The target point
 * @return The absolute angle between the source point and the target point in radians
 */
angle_t point_compute_angle_to(const point_t *source, const point_t *target);

/* ******************************************* Public callback functions declarations ************************************ */
