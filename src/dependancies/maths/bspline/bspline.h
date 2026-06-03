/**
 * @file bspline.h
 * @brief Gestion des B-Splines.
 * @author Théo Magne
 * @date 04/06/2022
 * @see bspline.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/vector/vector.h"
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum number of crossing points that can be handled by the B-Spline functions.
 * @note if you want bigger ones, just make two splines one after the other ^^
 */
#define CROSSING_POINTS_NB_MAX (4)

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure that defines a B-Spline.
 */
typedef struct
{
    uint8_t
        nb_points; /* Number of points the robot has to go trough (including start and arrival) */
    float *P;      /* Crossing points matrix */
    float *Q;      /* Control points matrix */
    vector_2d_float_t start_vector; /* Vector that indicates the starting orientation */
    vector_2d_float_t end_vector;   /* Vector that indicates the final orientation */
} bspline_t;

/**
 * @brief Structure that defines a B-Spline curve.
 */
typedef struct
{
    float resolution;          /* the lower the value is the more precise the curve will be */
    vector_2d_float_t *points; /* points that draws the curve */
    uint16_t nb_points;        /* Number of points in the curve */
    uint16_t nb_points_max;    /* Max number of points */
} bspline_curve_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Calcul les points de controle de la spline à partir de points de passage prédéfinis.
 * Fonction à appeler en premier avec pour la spline:
 * spline->P qui pointe vers un tableau de float de taille: 2 * nb_points
 * spline->P est initialisé comme suit: P[0] = x0, P[1] = y0, P[2] = x1, P[3] = y1, ...
 * spline->Q qui pointe vers un tableau de float de taille: 2 * (nb_points + 2)
 * spline->nb_points initialisé avec le nombre de point de passage
 * spline->start_vector et spline->end_vector initilisés avec les vecteurs de départ et d'arrivée
 * @param spline la spline concernée
 */
void bspline_compute_control_points(bspline_t *spline);

/**
 * @brief Calcul les points de controle de la spline à partir de points de passage prédéfinis.
 * Fonction à appeler après BSPLINE_compute_control_points() avec:
 * spline le résultat de BSPLINE_compute_control_points()
 * curve->resolution initialisé avec la résolution souhaitée. Pour X points de passage prédéfinis, on obtient (X -1) * 1 / resolution points
 *       exemple: 30 points pour une spline à 4 points de passage prédéfini
 * curve->points qui pointe vers un tableau de vector_2d_t
 * curve->nb_points_max initialisé avec la taille du tableau sur lequel pointe curve->points
 * curve->nb_points est initialisé à 0 par cette fonction
 * @param spline la spline concernée
 * @param curve la courbe qui sera parcourue par le robot
 */
bool bspline_compute_curve(const bspline_t *spline, bspline_curve_t *curve);

/* ******************************************* Public callback functions declarations ************************************ */
