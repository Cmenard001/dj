/**
 * @file angle.c
 * @brief Angle utils
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 11/10/2025
 * @see angle.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "angle.h"
#include "utils/macros/macros.h"
#include <math.h>
#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Tolérance d'égalité entre deux angles
 */
#define ANGLE_EQUALITY_EPSILON (0.0001f)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

bool angles_equal_with_epsilon(angle_t a1, angle_t a2, angle_t epsilon)
{
    // Calculate the shortest angular distance between a1 and a2
    angle_t diff = modulo_angle(a1 - a2);
    return (fabsf(diff) <= epsilon);
}

bool angles_equal(angle_t a1, angle_t a2)
{
    return angles_equal_with_epsilon(a1, a2, ANGLE_EQUALITY_EPSILON);
}

angle_deg_t rad2deg(angle_t angle)
{
    return (angle * 180.0f) / PI;
}

angle_t modulo_angle(angle_t angle)
{
    while (angle > PI)
    {
        angle -= 2 * PI;
    }
    while (angle < -PI)
    {
        angle += 2 * PI;
    }
    return angle;
}

/* ***************************************** Public callback functions definitions *************************************** */
