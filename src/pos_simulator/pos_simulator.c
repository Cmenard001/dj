/**
 * @file pos_simulator.c
 * @brief Simulate a fictive position of robot
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see pos_simulator.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "pos_simulator.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */
static GEOMETRY_point_t g_robot_position;

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void pos_simulator_set_position(GEOMETRY_point_t position)
{
    g_robot_position = position;
}

GEOMETRY_point_t pos_simulator_get_position()
{
    return g_robot_position;
}

/* ***************************************** Public callback functions definitions *************************************** */