/**
 * @file dj_dependencies.c
 * @brief Useful file to centralize dependency includes and simplify the codebase
 * @note This file must be overriden by the user with the correct dependencies
 * @author Cyprien MÃ©nard
 * @date 25/03/2025
 * @see dj_dependencies.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_dependencies.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_dep_get_robot_position(GEOMETRY_point_t *robot_position)
{
    // Fictive values
    robot_position->x = 0;
    robot_position->y = 0;
}

void dj_dep_get_robot_speed(GEOMETRY_vector_t *robot_speed)
{
    // Fictive values
    robot_speed->x = 0;
    robot_speed->y = 0;
}

void dj_dep_get_robot_acceleration(GEOMETRY_vector_t *robot_acceleration)
{
    // Fictive values
    robot_acceleration->x = 0;
    robot_acceleration->y = 0;
}

void dj_dep_get_time(time32_t *time)
{
    // Fictive value
    *time = 0;
}

/* ***************************************** Public callback functions definitions *************************************** */
