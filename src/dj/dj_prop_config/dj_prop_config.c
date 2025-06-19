/**
 * @file dj_prop_config.h
 * @brief Configuration de dj avec les coefficients de propulsion
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_prop_config.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_prop_config.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

bool is_pre_rotation_needed(int16_t angle_diff)
{
    if (angle_diff > MIN_ANGLE_PRE_ROTATION_IMMOBILE_RAD || angle_diff < -MIN_ANGLE_PRE_ROTATION_IMMOBILE_RAD)
    {
        return true;
    }
    return false;
}

/* ******************************************* Public callback functions declarations ************************************ */
