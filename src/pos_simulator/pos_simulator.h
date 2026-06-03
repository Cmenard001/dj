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

#include "utils/maths/position/position.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Set the full position of the robot in the simulator
 * @param position The position to set (point + angle)
 */
void pos_simulator_set_position(position_t position);

/**
 * @brief Set the position of the robot using a point (angle defaults to 0.0)
 * @param p The point to set as robot position
 */
void pos_simulator_set_point(point_t p);

/**
 * @brief Get the current position of the robot in the simulator
 * @return The current position of the robot
 */
position_t pos_simulator_get_position();

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __POS_SIMULATOR_H__ */