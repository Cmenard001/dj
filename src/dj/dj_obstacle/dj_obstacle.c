/**
 * @file dj_obstacle.c
 * @brief Generic obstacle of dj
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_obstacle.h"
#include "system/assert/system_assert.h"
#include <stddef.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_init(dj_obstacle_t *obstacle, dj_obstacle_type_t type, bool is_enabled)
{
    SYSTEM_ASSERT(obstacle != NULL);
    obstacle->type = type;
    obstacle->is_enabled = is_enabled;
}

dj_obstacle_type_t dj_obstacle_get_type(dj_obstacle_t *obstacle)
{
    SYSTEM_ASSERT(obstacle != NULL);
    return obstacle->type;
}

void dj_obstacle_enable(dj_obstacle_t *obstacle, bool enable)
{
    SYSTEM_ASSERT(obstacle != NULL);
    obstacle->is_enabled = enable;
}

bool dj_obstacle_is_enabled(dj_obstacle_t *obstacle)
{
    SYSTEM_ASSERT(obstacle != NULL);
    return obstacle->is_enabled;
}

/* ******************************************* Public callback functions declarations ************************************ */
