/**
 * @file dj_viewer_status.h
 * @brief A viewer status is an state of the robot during the dj solver algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_viewer_status.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/point/point.h"
#include "utils/maths/vector/vector.h"
#include "utils/time/time.h"
#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief A viewer status is an state of the robot during the dijkstra algorithm
 *
 */
typedef struct
{
    /**
     * @brief Position of the viewer
     */
    point_t position;
    /**
     * @brief Speed of the viewer
     */
    vector_2d_distance_t speed;
    /**
     * @brief Time at which the viewer is observing
     */
    time_ms_t time;
} dj_viewer_status_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a viewer status
 * @param status Pointer to the viewer status to initialize
 * @param position position of the viewer
 * @param speed speed of the viewer
 * @param acceleration acceleration of the viewer
 * @param time time at which the viewer is observing
 */
void viewer_status_init(dj_viewer_status_t *status,
                        const point_t *position,
                        const vector_2d_distance_t *speed,
                        time_ms_t time);

/**
 * @brief Function to update a viewer status
 * @param status Pointer to the viewer status to update
 * @param position position of the viewer
 * @param speed speed of the viewer
 * @param acceleration acceleration of the viewer
 * @param time time at which the viewer is observing
 */
void viewer_status_update(dj_viewer_status_t *status,
                          const point_t *position,
                          const vector_2d_distance_t *speed,
                          time_ms_t time);

/* ******************************************* Public callback functions declarations ************************************ */
