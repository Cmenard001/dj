/**
 * @file pf_calculator_dynamic_obstacles.h
 * @brief Generic header for the dynamic obstacles calculator module
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see pf_calculator_dynamic_obstacles.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "utils/dj/dj_obstacle/dj_obstacle_dynamic.h"
#include "utils/maths/kinematics/linear_acceleration/linear_acceleration.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Argument structure for the dynamic obstacle position calculation
 */
typedef struct
{
    linear_acceleration_t prop_linear_acceleration;
} pf_obstacle_dynamic_get_position_args_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Get the position of a dynamic obstacle in space time
 * @param obstacle Dynamic obstacle object
 * @param viewer_status Viewer status object
 * @param solution Dynamic polygon solution object
 */
void pf_obstacle_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                      dj_viewer_status_t *viewer_status,
                                      dj_dynamic_polygon_solution_t *solution);
