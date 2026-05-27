/**
 * @file moving_average.c
 * @brief Gestion de filtre à moyenne glissante
 * @author Théo 2021
 * @date 2021
 * @see moving_average.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "moving_average.h"
#include "system/assert/system_assert.h"
#include "utils/time/time.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */
/**
 * @brief initialisation d'une sctructure pour gérer les moyennes glissantes
 *
 * @param moving_average pointeur sur la structure à initialiser
 * @param period nombre d'échantillons sur laquelle il faut réaliser la moyenne glissante
 */
void moving_average_init(moving_average_t *moving_average, time_ms_t period)
{
    SYSTEM_ASSERT(period <= MOVING_AVERAGE_PERIOD_MAX);
    moving_average->period = period;
    moving_average->inv_period = 1.0f / (float)period;
}

/**
 * @brief Mise à jour de la moyenne glissante en utilisant la nouvelle valeur
 *
 * @param moving_average pointeur sur la structure de filtre glissant à utiliser
 * @param new_value nouvelle valeur à utiliser pour mettre à jour le filtre
 * @return float
 */
float moving_average_update(moving_average_t *moving_average, float new_value)
{
    moving_average->sum -= moving_average->buffer[moving_average->index];
    moving_average->buffer[moving_average->index] = new_value;
    moving_average->sum += new_value;
    moving_average->average = moving_average->sum * moving_average->inv_period;
    moving_average->index++;
    moving_average->index
        = (uint16_t)((uint32_t)moving_average->index % (uint32_t)moving_average->period);
    return moving_average->average;
}

/* ***************************************** Public callback functions definitions *************************************** */
