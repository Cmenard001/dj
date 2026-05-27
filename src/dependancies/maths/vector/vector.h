/**
 * @file vector.h
 * @brief Vector utils
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see vector.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/angle/angle.h"
#include "utils/maths/distance/distance.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Macro permettant de définir un type de vecteur 2D avec des composantes de type custom
 * @param type Type des composantes du vecteur (ex: int, float, double, etc.)
 * @param vector_type Nom du type de vecteur à définir (ex: vector_int_t, vector_float_t, etc.)
 */
#define DEFINE_2D_VECTOR_TYPE(type, vector_type)                                                   \
    typedef struct                                                                                 \
    {                                                                                              \
        type x;                                                                                    \
        type y;                                                                                    \
    } vector_type;

/**
 * @brief Macro permettant de définir un type de vecteur 3D avec des composantes de type custom
 * @param type Type des composantes du vecteur (ex: int, float, double, etc.)
 * @param vector_type Nom du type de vecteur à définir (ex: vector_int_t, vector_float_t, etc.)
 */
#define DEFINE_3D_VECTOR_TYPE(type, vector_type)                                                   \
    typedef struct                                                                                 \
    {                                                                                              \
        type x;                                                                                    \
        type y;                                                                                    \
        type z;                                                                                    \
    } vector_type;

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure représentant un vecteur 2D générique en coordonnées flottantes
 */
DEFINE_2D_VECTOR_TYPE(float, vector_2d_float_t)

/**
 * @brief Structure représentant un vecteur 2D de distances
 */
DEFINE_2D_VECTOR_TYPE(distance_t, vector_2d_distance_t)

/**
 * @brief Structure représentant un vecteur 3D générique en coordonnées flottantes
 */
DEFINE_3D_VECTOR_TYPE(float, vector_3d_float_t)

/**
 * @brief Structure représentant un vecteur 3D de distances
 */
DEFINE_3D_VECTOR_TYPE(distance_t, vector_3d_distance_t)

/**
 * @brief Type représentant un vecteur 2D Polaire
 */
typedef struct
{
    distance_t magnitude; /**< Magnitude (longueur) du vecteur */
    angle_t angle;        /**< Angle du vecteur en radians */
} vector_2d_polar_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Calcule le produit scalaire de deux vecteurs 2D à composantes flottantes
 * @param v Premier vecteur
 * @param u Deuxième vecteur
 * @return float Le produit scalaire des deux vecteurs
 */
float dot_product(const vector_2d_float_t *v, const vector_2d_float_t *u);

/**
 * @brief Convertit un vecteur 2D cartésien en vecteur 2D polaire
 * @param cartesian Vecteur cartésien à convertir
 * @param polar Vecteur polaire résultant
 */
void cartesian_to_polar(const vector_2d_distance_t *cartesian, vector_2d_polar_t *polar);

/**
 * @brief Convertit un vecteur 2D polaire en vecteur 2D cartésien
 * @param polar Vecteur polaire à convertir
 * @param cartesian Vecteur cartésien résultant
 */
void polar_to_cartesian(const vector_2d_polar_t *polar, vector_2d_distance_t *cartesian);

/**
 * @brief Calcule la longueur d'un vecteur 2D de distances
 * @param v Vecteur dont on veut calculer la longueur
 * @return distance_t La longueur du vecteur
 */
distance_t vector_2d_distance_length(const vector_2d_distance_t *v);

/**
 * @brief Calcule la distance au carré entre deux vecteurs 2D de distances
 * @param v Premier vecteur
 * @param u Deuxième vecteur
 * @return float La distance au carré entre les deux vecteurs
 */
float vector_2d_distance_square(const vector_2d_distance_t *v, const vector_2d_distance_t *u);

/**
 * @brief Calcule la distance entre deux points représentés par des vecteurs 2D de distances
 * @param v Premier point
 * @param u Deuxième point
 * @return distance_t La distance entre les deux points
 */
distance_t vector_2d_distance(const vector_2d_distance_t *v, const vector_2d_distance_t *u);

/* ******************************************* Public callback functions declarations ************************************ */
