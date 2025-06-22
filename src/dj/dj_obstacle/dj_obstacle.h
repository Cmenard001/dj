/**
 * @file dj_obstacle.h
 * @brief Generic obstacle of dj
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle.c
 */

#ifndef __DJ_OBSTACLE_H__
#define __DJ_OBSTACLE_H__

/* ******************************************************* Includes ****************************************************** */

#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Common attributes of all obstacles
 * @note Use this macro in the obstacle structures to define the common attributes
 * @warning Put this macro at the beginning of the obstacle structures for correct casting
 */
#define DJ_OBSTACLE_MOTHER_ATTRIBUTES                                                                                       \
    dj_obstacle_type_t m_type;                                                                                              \
    bool m_is_enabled;

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type of obstacles
 */
typedef enum
{
    DJ_OBSTACLE_TYPE_STATIC,
    DJ_OBSTACLE_TYPE_DYNAMIC,
    DJ_OBSTACLE_TYPE_COUNT
} dj_obstacle_type_t;

/**
 * @brief Generic obstacle structure
 * @warning Do not instanciate this structure directly, use the specific obstacle structures
 * @note This structure is used to store the common attributes of all obstacles
 * @note So, you can cast a specific obstacle to this structure to access the common attributes
 */
typedef struct
{
    DJ_OBSTACLE_MOTHER_ATTRIBUTES
} dj_obstacle_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize an generic obstacle
 *
 * @param obstacle Pointer to the obstacle to initialize
 * @param type Type of the obstacle
 * @param id ID of the obstacle (must be unique)
 */
void dj_obstacle_init(dj_obstacle_t *obstacle, dj_obstacle_type_t type, bool is_enabled);

/**
 * @brief Function to deinitialize an generic obstacle
 *
 * @param obstacle Pointer to the obstacle to deinitialize
 */
void dj_obstacle_deinit(dj_obstacle_t *obstacle);

/**
 * @brief Function to get the type of an obstacle
 *
 */
dj_obstacle_type_t dj_obstacle_get_type(dj_obstacle_t *obstacle);

/**
 * @brief Function to enable or disable an obstacle
 *
 */
void dj_obstacle_enable(dj_obstacle_t *obstacle, bool enable);

/**
 * @brief Function to check if an obstacle is enabled
 *
 */
bool dj_obstacle_is_enabled(dj_obstacle_t *obstacle);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
