/**
 * @file dj_obstacle_id.c
 * @brief Gestion des identifiants des obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_id.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle_importer/dj_obstacle_id.h"
#include "system/assert/system_assert.h"
#include <stddef.h>
/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_id_generator_init(dj_obstacle_id_generator_t *generator)
{
    SYSTEM_ASSERT(generator != NULL);
    generator->dj_static_obstacle_id = 0;
    generator->dj_dynamic_obstacle_id = 0;
}

dj_static_obstacle_id_t dj_obstacle_id_generate_static_obstacle_id(
    dj_obstacle_id_generator_t *generator)
{
    SYSTEM_ASSERT(generator != NULL);
    return generator->dj_static_obstacle_id++;
}

dj_dynamic_obstacle_id_t dj_obstacle_id_generate_dynamic_obstacle_id(
    dj_obstacle_id_generator_t *generator)
{
    SYSTEM_ASSERT(generator != NULL);
    return generator->dj_dynamic_obstacle_id++;
}

dj_static_obstacle_id_t dj_obstacle_id_get_static_obstacle_count(
    const dj_obstacle_id_generator_t *generator)
{
    SYSTEM_ASSERT(generator != NULL);
    return generator->dj_static_obstacle_id;
}

dj_dynamic_obstacle_id_t dj_obstacle_id_get_dynamic_obstacle_count(
    const dj_obstacle_id_generator_t *generator)
{
    SYSTEM_ASSERT(generator != NULL);
    return generator->dj_dynamic_obstacle_id;
}

/* ******************************************* Public callback functions declarations ************************************ */
