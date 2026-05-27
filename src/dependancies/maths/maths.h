/**
 * @file maths.h
 * @brief Regroupement de toutes les fonctions mathématiques utiles
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see maths.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "axis/axis.h"
#include "point/point.h"
#include "utils/maths/angle/angle.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

// IMPORTANT IL FAUT UTILISER UN TABLEAU DE TYPE SINT32, sinon ça va merder violement !

// L'utilisation d'un filtre à temps futur ou passé introduit un déphasage (avance pour temps futur, retard pour temps passé)
// Pour contrer ce déphasage il est conseillé d'utiliser les deux filtres à la fois.
// ex :
//      filter_future_time(values, 30, (float[]){1/2., 1/2.}, 2);
//      filter_past_time(values, 30, (float[]){1/2., 1/2.}, 2);
//
//	Subtilité pour dérivé votre tableau de donnée faite :
//      filter_future_time(values, 30, (float[]){1, -1}, 2);

/**
 * @brief Filtre à temps future
 * @param values	: tableau de donnée é traiter
 * @param nb_value	: nombre de valeur é traiter dans le tableau
 * @param factor	: tableau des facteurs
 * @param nb_factor	: nombre de facteurs
 * @example filtre moyenneur sur deux valeurs :
 *            filter_future_time(values, 30, (float[]){1/2., 1/2.}, 2);
 *            résultat pour la premiére valeur :	values[0] = 1/2 * values[0] + 1/2 values[1];
 *            effet de bord : values[29] = values[29];
 */
void filter_future_time(int32_t values[],
                        uint16_t nb_value,
                        const float factor[],
                        uint8_t nb_factor);

/**
 * @brief Filtre à temps passé
 * @param values	: tableau de donnée à traiter
 * @param nb_value	: nombre de valeur à traiter dans le tableau
 * @param factor	: tableau des facteurs
 * @param nb_factor	: nombre de facteurs
 * @example : filtre moyenneur sur deux valeurs :
 *              filter_past_time(values, 30, (float[]){1/2., 1/2.}, 2);
 *              résultat pour la premiére valeur :	values[29] = 1/2 * values[29] + 1/2 values[28];
 *              effet de bord : values[0] = values[0];
 */
void filter_past_time(int32_t values[], uint16_t nb_value, const float factor[], uint8_t nb_factor);

/** Recherche de la valeur minimale dans un tableau
 * @param values	: tableau de donnée à traiter
 * @param nbValue	: nombre de valeur à traiter dans le tableau
 * @return indice de la valeur minimale
 */
uint16_t searchMin(const int32_t values[], uint16_t nbValue);

/** Recherche de la valeur maximale dans un tableau
 * @param values	: tableau de donnée à traiter
 * @param nbValue	: nombre de valeur à traiter dans le tableau
 * @return indice de la valeur maximale
 */
uint16_t searchMax(const int32_t values[], uint16_t nbValue);

/** Calcul de la factorielle
 * @param nb	: nombre dont on veut calculer la factorielle
 * @return factorielle de nb
 */
uint64_t factorielle(uint32_t nb);

/** Calcul de la régression linéaire
 * @param angle		: angle de la droite
 * @param offset	: ordonnée à l'origine
 * @param quality	: qualité de la régression
 * @param points	: tableau des points
 * @param n			: nombre de points
 * @param axis		: axe sur lequel on veut faire la régression
 * @return true si la régression est possible, false sinon
 */
bool linear_regression(angle_t *angle,
                       distance_t *offset,
                       int8_t *quality,
                       const point_t *points,
                       int32_t n,
                       axis_t axis);

/* ******************************************* Public callback functions declarations ************************************ */
