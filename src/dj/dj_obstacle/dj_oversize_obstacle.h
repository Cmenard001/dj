/**
 * @file dj_oversize_obstacle.h
 * @brief Tool to oversize an obstacle
 * Why do we need to oversize an obstacle?
 * User define an obstacle which in all the area of robot mustn't go through.
 * But when we make displacement, we consider the center of the robot.
 * So, we need to oversize the obstacle by half of the robot's width.
 * @author Cyprien Ménard
 * @date 04/02/2025
 * @see dj_oversize_obstacle.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_polygon.h"
/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Default mode to use if you don't know what to use
 */
#define DJ_DEFAULT_OVERSIZE_MODE DJ_OBSTACLE_OVERSIZE_MODE_1_POINT

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Oversize mode
 * @warning Values must match number of points except for END_AT_BRAKE and NO_OVERSIZE
 */
typedef enum
{
    DJ_OBSTACLE_OVERSIZE_MODE_1_POINT = 1, /**< 1 point by corner */
    DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,    /**< 2 points by corner */
    DJ_OBSTACLE_OVERSIZE_MODE_3_POINTS,    /**< 3 points by corner */
    DJ_OBSTACLE_OVERSIZE_MODE_4_POINTS,    /**< 4 points by corner */
    DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE, /**< The angle between the two points will be small enough
                                                  to force the propulsion to use END_AT_BRAKE rotations */
    DJ_OBSTACLE_OVERSIZE_MODE_NO_OVERSIZE   /**< No oversize */
} dj_obstacle_oversize_mode_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Oversize an obstacle
 * @param[in,out] polygon : the polygon to be oversized (modified in place)
 * @param[in] mode : oversize mode
 * @param[in] oversize_distance : distance to take into account in millimeters
 */
void dj_oversize_obstacle(dj_polygon_t *polygon,
                          dj_obstacle_oversize_mode_t mode,
                          distance_t oversize_distance);

/* ******************************************* Public callback functions declarations ************************************ */
