/**
 * @file pf_calculator_path_duration.h
 * @brief Header file for path duration calculator
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see pf_calculator_path_duration.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "propulsion/prop_types/prop_end_condition.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/maths/kinematics/angular_acceleration/angular_acceleration.h"
#include "utils/maths/kinematics/angular_speed/angular_speed.h"
#include "utils/maths/kinematics/linear_acceleration/linear_acceleration.h"
#include "utils/maths/kinematics/linear_speed/linear_speed.h"
#include "utils/time/time.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Argument structure for the path duration calculator
 */
typedef struct
{
    linear_speed_t max_linear_speed;             /**< Maximum linear speed [mm/s] */
    angular_speed_t max_angular_speed;           /**< Maximum angular speed [rad/s] */
    linear_acceleration_t linear_acceleration;   /**< Linear acceleration [mm/s^2] */
    linear_acceleration_t linear_deceleration;   /**< Linear deceleration [mm/s^2] */
    angular_acceleration_t angular_acceleration; /**< Angular acceleration [rad/s^2] */
    prop_end_condition_t end_condition;          /**< End condition between path points */
} pf_path_duration_calculator_args_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Calculates the duration of a path
 * @param path Path object
 * @return time_ms_t The total duration of the path
 */
time_ms_t pf_path_duration_calculator(dj_graph_path_t *path);
