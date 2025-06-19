/**
 * @file dj_obstacle_id.c
 * @brief Gestion des identifiants des obstacles
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_obstacle_id.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle_id.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/**
 * @brief Index for attributions of static obstacles
 * @note The index is the next available id
 */
static dj_static_obstacle_id_e dj_static_obstacle_id = STATIC_OBSTACLE_COUNT;

/**
 * @brief Index for attributions of dynamic obstacles
 * @note The index is the next available id
 */
static dj_dynamic_obstacle_id_e dj_dynamic_obstacle_id = DYNAMIC_OBSTACLE_COUNT;

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

dj_static_obstacle_id_e dj_obstacle_id_generate_static_obstacle_id()
{
    return dj_static_obstacle_id++;
}

dj_dynamic_obstacle_id_e dj_obstacle_id_generate_dynamic_obstacle_id()
{
    return dj_dynamic_obstacle_id++;
}

dj_static_obstacle_id_e dj_obstacle_id_get_static_obstacle_count()
{
    return dj_static_obstacle_id;
}

dj_dynamic_obstacle_id_e dj_obstacle_id_get_dynamic_obstacle_count()
{
    return dj_dynamic_obstacle_id;
}

/* ******************************************* Public callback functions declarations ************************************ */
