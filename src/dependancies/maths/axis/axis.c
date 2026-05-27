/**
 * @file axis.c
 * @brief Implementation of functions related to possible axis
 * @author Cyprien Ménard
 * @date 21/10/2025
 * @see axis.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "axis.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include <math.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/**
 * @brief Enumeration of signed axis for internal use
 */
enum signed_axis_enum_t
{
    AXIS_X_POS,
    AXIS_X_NEG,
    AXIS_Y_POS,
    AXIS_Y_NEG,
    AXIS_COUNT_SIGNED
};

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

LOG_REGISTER("utils/maths/axis");

static const angle_t axis_angles[AXIS_COUNT] = {
    [AXIS_X] = 0,
    [AXIS_Y] = PI / 2,
    [AXIS_Z] = 0, // Z axis is not defined in 2D space
};

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

angle_t axis_to_angle(axis_t axis)
{
    if (axis >= AXIS_COUNT || axis == AXIS_Z)
    {
        LOGE("axis_to_angle: invalid axis %d\n", axis);
        return 0;
    }
    return axis_angles[axis];
}

angle_t signed_axis_to_angle(signed_axis_t axis)
{
    angle_t angle = axis_to_angle(axis.axis);
    if (!axis.is_positive)
    {
        angle += PI;
        angle = modulo_angle(angle);
    }
    return angle;
}

void angle_to_signed_axis(angle_t angle, signed_axis_t *signed_axis)
{
    SYSTEM_ASSERT(signed_axis != NULL);
    angle_t axis_angles_signed[AXIS_COUNT_SIGNED] = {
        [AXIS_X_POS] = axis_angles[AXIS_X],
        [AXIS_X_NEG] = modulo_angle(axis_angles[AXIS_X] + PI),
        [AXIS_Y_POS] = axis_angles[AXIS_Y],
        [AXIS_Y_NEG] = modulo_angle(axis_angles[AXIS_Y] + PI),
    };

    angle_t min_diff = 2 * PI;
    enum signed_axis_enum_t best_axis = AXIS_X_POS;
    for (enum signed_axis_enum_t i = AXIS_X_POS; i < AXIS_COUNT_SIGNED; i++)
    {
        angle_t diff = fabsf(modulo_angle(angle - axis_angles_signed[i]));
        if (diff < min_diff)
        {
            min_diff = diff;
            best_axis = i;
        }
    }

    switch (best_axis)
    {
        case AXIS_X_POS:
            signed_axis->axis = AXIS_X;
            signed_axis->is_positive = true;
            break;
        case AXIS_X_NEG:
            signed_axis->axis = AXIS_X;
            signed_axis->is_positive = false;
            break;
        case AXIS_Y_POS:
            signed_axis->axis = AXIS_Y;
            signed_axis->is_positive = true;
            break;
        case AXIS_Y_NEG:
            signed_axis->axis = AXIS_Y;
            signed_axis->is_positive = false;
            break;
        default:
            LOGE("angle_to_signed_axis: invalid best_axis %d\n", best_axis);
            signed_axis->axis = AXIS_X;
            signed_axis->is_positive = true;
            break;
    }
}

axis_t angle_to_axis(angle_t angle)
{
    signed_axis_t signed_axis;
    angle_to_signed_axis(angle, &signed_axis);
    return signed_axis.axis;
}

/* ***************************************** Public callback functions definitions *************************************** */
