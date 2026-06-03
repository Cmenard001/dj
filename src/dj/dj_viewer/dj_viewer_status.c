/**
 * @file dj_viewer_status.h
 * @brief A viewer status is an state of the robot during the dj solver algorithm
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_viewer_status.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include <stdlib.h>

LOG_REGISTER("utils/dj/viewer_status");
/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void viewer_status_init(dj_viewer_status_t *status,
                        const point_t *position,
                        const vector_2d_distance_t *speed,
                        time_ms_t time)
{
    SYSTEM_ASSERT(status != NULL);
    SYSTEM_ASSERT(position != NULL);
    SYSTEM_ASSERT(speed != NULL);
    viewer_status_update(status, position, speed, time);
}

void viewer_status_update(dj_viewer_status_t *status,
                          const point_t *position,
                          const vector_2d_distance_t *speed,
                          time_ms_t time)
{
    SYSTEM_ASSERT(status != NULL);
    SYSTEM_ASSERT(position != NULL);
    SYSTEM_ASSERT(speed != NULL);
    status->position = *position;
    status->speed = *speed;
    status->time = time;
}

/* ******************************************* Public callback functions declarations ************************************ */
