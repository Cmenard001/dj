/**
 * @file moving_average.h
 * @brief Gestion de filtre à moyenne glissante
 * @author Théo 2021
 * @date 2021
 * @see moving_average.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "utils/time/time.h"
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Période maximale de la moyenne glissante
 */
#define MOVING_AVERAGE_PERIOD_MAX (30)

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief définition de la structure moyenne glissante
 */
typedef struct
{
    float sum;
    float buffer[MOVING_AVERAGE_PERIOD_MAX];
    float inv_period;
    time_ms_t period;
    uint16_t index;
    float average;
} moving_average_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialise la structure de moyenne glissante
 * @param moving_average la structure de moyenne glissante à initialiser
 * @param period la période de la moyenne glissante (en nombre d'échantillons)
 */
void moving_average_init(moving_average_t *moving_average, time_ms_t period);

/**
 * @brief Met à jour la moyenne glissante avec une nouvelle valeur
 * @param moving_average la structure de moyenne glissante à mettre à jour
 * @param new_value la nouvelle valeur à ajouter à la moyenne glissante
 * @return la nouvelle valeur moyenne glissante
 */
float moving_average_update(moving_average_t *moving_average, float new_value);

/* ******************************************* Public callback functions declarations ************************************ */
