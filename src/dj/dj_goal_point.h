/**
 * @file dj_goal_point.h
 * @brief DJ goal point type definition
 * @author Cyprien Ménard
 * @date 01/2025
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/angle/angle.h"
#include "utils/maths/point/point.h"
#include <stdbool.h>

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief A destination point for the DJ pathfinding engine, with optional arrival angle
 */
typedef struct
{
    point_t point;         /**< Destination position */
    angle_t arrival_angle; /**< Desired arrival angle [rad] */
    bool use_angle;        /**< If true, perform final rotation to arrival_angle */
} dj_goal_point_t;
