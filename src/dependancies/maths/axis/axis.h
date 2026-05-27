/**
 * @file axis.h
 * @brief Possible axis
 * @author Cyprien Ménard
 * @date 12/10/2025
 * @see axis.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/angle/angle.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Enumeration of the possible axis
 */
typedef enum
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
    AXIS_COUNT
} axis_t;

/**
 * @brief Structure representing a signed axis
 */
typedef struct
{
    axis_t axis;
    bool is_positive;
} signed_axis_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Convert an axis to the corresponding angle
 * @param axis The axis to convert
 * @return angle_t The corresponding angle
 * @warning Z axis is isn't defined in 2D space, so does not have a real meaning here
 */
angle_t axis_to_angle(axis_t axis);

/**
 * @brief Convert an signed axis to the corresponding angle
 * @param axis The axis to convert
 * @return angle_t The corresponding angle
 * @warning Z axis is isn't defined in 2D space, so does not have a real meaning here
 */
angle_t signed_axis_to_angle(signed_axis_t axis);

/**
 * @brief Convert an angle to the corresponding signed axis
 * @param angle The angle to convert
 * @param[out] signed_axis Pointer to the signed_axis_t structure where the result will be stored
 * @note the chosen axis is the closest one to the given angle
 * @note the Z axis is isn't defined in 2D space, so does not have a real meaning here
 */
void angle_to_signed_axis(angle_t angle, signed_axis_t *signed_axis);

/**
 * @brief Convert an angle to the corresponding axis
 * @param angle The angle to convert
 * @return axis_t The corresponding axis
 * @note the chosen axis is the closest one to the given angle
 * @note the Z axis is isn't defined in 2D space, so does not have a real meaning here
 */
axis_t angle_to_axis(angle_t angle);

/* ******************************************* Public callback functions declarations ************************************ */
