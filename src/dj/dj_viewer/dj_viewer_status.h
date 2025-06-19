/**
 * @file dj_viewer_status.h
 * @brief A viewer status is an state of the robot during the dj solver algorithm
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_viewer_status.c
 */

#ifndef __VIEWER_STATUS_H__
#define __VIEWER_STATUS_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"

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
    GEOMETRY_point_t m_position;
    /**
     * @brief Speed of the viewer
     */
    GEOMETRY_vector_t m_speed;
    /**
     * @brief Time at which the viewer is observing
     */
    time32_t m_time;
} dj_viewer_status_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a viewer status
 *
 * @param status Pointer to the viewer status to initialize
 * @param position position of the viewer
 * @param speed speed of the viewer
 * @param acceleration acceleration of the viewer
 * @param time time at which the viewer is observing
 */
void viewer_status_init(dj_viewer_status_t *status, GEOMETRY_point_t *position, GEOMETRY_vector_t *speed, time32_t time);

/**
 * @brief Function to deinitialize a viewer status
 *
 * @param status Pointer to the viewer status to deinitialize
 */
void viewer_status_deinit(dj_viewer_status_t *status);

/**
 * @brief Function to update a viewer status
 *
 * @param status Pointer to the viewer status to update
 * @param position position of the viewer
 * @param speed speed of the viewer
 * @param acceleration acceleration of the viewer
 * @param time time at which the viewer is observing
 */
void viewer_status_update(dj_viewer_status_t *status, GEOMETRY_point_t *position, GEOMETRY_vector_t *speed, time32_t time);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
