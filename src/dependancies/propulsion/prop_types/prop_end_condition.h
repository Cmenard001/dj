/**
 * @file prop_end_condition.h
 * @brief Propulsion end condition type
 * @author Cyprien Ménard
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/**
 * @brief End condition for a propulsion trajectory segment
 */
typedef enum
{
    PROP_END_AT_POINT = 0, /**< Robot stops at every intermediate point */
    PROP_END_AT_BRAKE = 1, /**< Robot brakes smoothly (continuous trajectory) */
} prop_end_condition_t;
