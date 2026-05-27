/**
 * @file position.h
 * @brief Position utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see position.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/angle/angle.h"
#include "utils/maths/distance/distance.h"
#include "utils/maths/point/point.h"
#include "utils/maths/segment/segment.h"
#include "utils/maths/vector/vector.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type représentant une position (point + angle)
 */
typedef struct
{
    point_t point;
    angle_t angle;
} position_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Calcul de la distance que l'on a à parcourir entre la position actuelle et la position finale ( en mm )
 * et calcul de l'angle
 * @param[in] start Position actuelle
 * @param[in] destination Position finale
 * @param[out] distance Distance à parcourir
 * @note La distance est négative si le point final est derrière le robot
 */
void viewing_algebric_distance(const position_t *start,
                               const point_t *destination,
                               distance_t *distance);

/**
 * @brief Calcul le point pour une distance dist à partir d'un point donné et dans un angle donné.
 * @param[in] start_pos Position de référence
 * @param[in] dist distance entre la référence et le point calculé
 * @param[out] point point calculé
 */
void calculate_point_from_distance_and_position(const position_t *start_pos,
                                                distance_t dist,
                                                point_t *point);

/**
 * @brief Calcul de l'angle absolue quand on regarde un point depuis un autre
 * @param[in] start Point regardant
 * @param[in] destination Point regardé
 * @param[out] angle Angle calculé
 */
void viewing_angle(const point_t *start, const point_t *destination, angle_t *angle);

/**
 * @brief Translate a position by a vector in the frame of the position
 * @param[in] pos Position to translatete
 * @param[in] translation Translation vector (x forward, y left)
 * @param[out] translated_point Translated point
 */
void translate_position(const position_t *pos,
                        const vector_2d_distance_t *translation,
                        point_t *translated_point);

/**
 * @brief Project a position on a segment
 * @param[in] pos Position to project
 * @param[in] seg Segment to project on
 * @param[out] projected_pos Projected position
 * @return true if the projection is on the segment, false if the projection is outside the segment
 */
bool project_position_on_segment(const position_t *pos,
                                 const segment_t *seg,
                                 point_t *projected_pos);

/* ******************************************* Public callback functions declarations ************************************ */
