/**
 * @file dj_prop_config.h
 * @brief Configuration de dj avec les coefficients de propulsion
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_prop_config.c
 *
 * @todo Lier les macros de dj_prop_config.h avec celles de prop.h pour éviter la redondance
 */

#ifndef __DJ_PROP_CONFIG_H__
#define __DJ_PROP_CONFIG_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Angle minimum pour que la prop choississe de tourner en rotation pure
 * @todo Lier cette macro avec celle en prop pour éviter la redondance
 */
#define MIN_ANGLE_PRE_ROTATION_IMMOBILE_RAD (PI4096 / 5)

/**
 * @brief Lorsqu'on résoud la position d'un obstacle dynamique dans l'espace temps
 * on doit tolérer une certaine marge d'erreur à cause de l'approximation d'un polygon à son point central
 */
#define MIN_ANGLE_PRE_ROTATION_TOLERANCE_RAD PI4096 / 10

/**
 * @brief Angles minimum et maximum pour que la prop choississe de tourner en rotation pure ou non
 */
#define MIN_ANGLE_TO_DO_PRE_ROTATION (MIN_ANGLE_PRE_ROTATION_IMMOBILE_RAD - MIN_ANGLE_PRE_ROTATION_TOLERANCE_RAD)
#define MAX_ANGLE_TO_NOT_DO_PRE_ROTATION (MIN_ANGLE_PRE_ROTATION_IMMOBILE_RAD + MIN_ANGLE_PRE_ROTATION_TOLERANCE_RAD)

/**
 * @brief Accélération angulaire maximale de la propulsion
 * @note Unité : PI4096/s^2
 * @note En prop, l'accélération angulaire est en float(rad/s^2), en strat elle est en entier (PI4096/s^2)
 * @todo Lier cette macro avec celle en prop pour éviter la redondance
 */
#define PROP_ANGULAR_ACCELERATION ((int32_t)(4 * 4096))

/**
 * @brief Accélération linéaire maximale de la propulsion
 * @note Unité : mm/s^2
 * @todo Lier cette macro avec celle en prop pour éviter la redondance
 */
#define PROP_LINEAR_ACCELERATION ((int32_t)(400))

/**
 * @brief Décélération linéaire maximale de la propulsion
 * @note Unité : mm/s^2
 * @todo Lier cette macro avec celle en prop pour éviter la redondance
 */
#define PROP_LINEAR_DECELERATION ((int32_t)(400))

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Vérifie si une rotation préalable est nécessaire pour atteindre la cible
 *
 * @param angle_diff Différence d'angle entre la trajectoire actuelle et le point cible
 * @return true Si une rotation préalable est nécessaire
 * @return false Sinon
 */
bool is_pre_rotation_needed(int16_t angle_diff);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
