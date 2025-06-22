/**
 * @file dj_obstacle.c
 * @brief Generic obstacle of dj
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle.h"
#include "../dj_logs/dj_logs.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_init(dj_obstacle_t *obstacle, dj_obstacle_type_t type, bool is_enabled)
{
    dj_control_non_null(obstacle, ) obstacle->m_type = type;
    obstacle->m_is_enabled = is_enabled;
}

void dj_obstacle_deinit(dj_obstacle_t *obstacle)
{
    // Nothing to do
}

dj_obstacle_type_t dj_obstacle_get_type(dj_obstacle_t *obstacle)
{
    dj_control_non_null(obstacle, DJ_OBSTACLE_TYPE_COUNT) return obstacle->m_type;
}

void dj_obstacle_enable(dj_obstacle_t *obstacle, bool enable)
{
    dj_control_non_null(obstacle, ) obstacle->m_is_enabled = enable;
}

bool dj_obstacle_is_enabled(dj_obstacle_t *obstacle)
{
    dj_control_non_null(obstacle, false) return obstacle->m_is_enabled;
}

/* ******************************************* Public callback functions declarations ************************************ */
