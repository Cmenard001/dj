/**
 * @file dj_viewer_status.h
 * @brief A viewer status is an state of the robot during the dj solver algorithm
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_viewer_status.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_viewer_status.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_logs/dj_logs.h"

#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void viewer_status_init(dj_viewer_status_t *status, GEOMETRY_point_t *position, GEOMETRY_vector_t *speed, time32_t time)
{
    dj_control_non_null(status, );
    dj_control_non_null(position, );
    dj_control_non_null(speed, );
    viewer_status_update(status, position, speed, time);
}

void viewer_status_deinit(dj_viewer_status_t *status)
{
    // Nothing to do
}

void viewer_status_update(dj_viewer_status_t *status, GEOMETRY_point_t *position, GEOMETRY_vector_t *speed, time32_t time)
{
    dj_control_non_null(status, );
    dj_control_non_null(position, );
    dj_control_non_null(speed, );
    status->m_position = *position;
    status->m_speed = *speed;
    status->m_time = time;
}

/* ******************************************* Public callback functions declarations ************************************ */
