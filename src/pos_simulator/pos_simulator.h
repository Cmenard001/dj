/**
 * @file pos_simulator.h
 * @brief Simulate a fictive position of robot
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see pos_simulator.c
 */

#ifndef __POS_SIMULATOR_H__
#define __POS_SIMULATOR_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj/dj_dependencies/QS_maths/QS_maths.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Set the position of the robot in the simulator
 * @param position The position to set
 */
void pos_simulator_set_position(GEOMETRY_point_t position);

/**
 * @brief Get the current position of the robot in the simulator
 * @return The current position of the robot
 */
GEOMETRY_point_t pos_simulator_get_position();

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __POS_SIMULATOR_H__ */