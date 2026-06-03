/**
 * @file dj_obstacle_id.h
 * @brief Gestion des identifiants des obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_id.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Id pour les solutions statiques des obstacles dynamiques
 * @see dj_dynamic_obstacle_id_t
 */
#define STATIC_OBSTACLE_UNKNOWN_ID ((dj_static_obstacle_id_t)(-1))

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Enumération des identifiants des obstacles statiques
 * @note Depuis le portage vers Flexibot, les obstacles statiques sont des entiers
 * quelconques (int32_t) et plus une énumération car ils ne sont plus spécifiques
 * au robot.
 */
typedef int32_t dj_static_obstacle_id_t;

/**
 * @brief Enumération des identifiants des obstacles dynamiques
 * @note Depuis le portage vers Flexibot, les obstacles dynamiques sont des entiers
 * quelconques (int32_t) et plus une énumération car ils ne sont plus spécifiques
 * au robot.
 */
typedef int32_t dj_dynamic_obstacle_id_t;

/**
 * @brief Structure représentant un générateur d'identifiants d'obstacles
 */
typedef struct
{
    /**
     * @brief Index for attributions of static obstacles
     * @note The index is the next available id
     */
    dj_static_obstacle_id_t dj_static_obstacle_id;

    /**
     * @brief Index for attributions of dynamic obstacles
     * @note The index is the next available id
     */
    dj_dynamic_obstacle_id_t dj_dynamic_obstacle_id;
} dj_obstacle_id_generator_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialisation du générateur d'identifiants d'obstacles
 */
void dj_obstacle_id_generator_init(dj_obstacle_id_generator_t *generator);

/**
 * @brief Fonction de génération d'un identifiant d'obstacle statique unique
 * @warning L'identifiant généré est unique et ne pourra pas être relaché.
 * Faites attention à ne pas en générer trop.
 * @param generator Générateur d'identifiants d'obstacles
 * @return dj_static_obstacle_id_t Identifiant d'obstacle statique unique généré
 */
dj_static_obstacle_id_t dj_obstacle_id_generate_static_obstacle_id(
    dj_obstacle_id_generator_t *generator);

/**
 * @brief Fonction de génération d'un identifiant d'obstacle dynamique unique
 * @warning L'identifiant généré est unique et ne pourra pas être relaché.
 * Faites attention à ne pas en générer trop.
 * @param generator Générateur d'identifiants d'obstacles
 * @return dj_dynamic_obstacle_id_t Identifiant d'obstacle dynamique unique généré
 */
dj_dynamic_obstacle_id_t dj_obstacle_id_generate_dynamic_obstacle_id(
    dj_obstacle_id_generator_t *generator);

/**
 * @brief Fonction de récupération du nombre d'obstacles statiques importés statiquement et dynamiquement
 * @param generator Générateur d'identifiants d'obstacles
 * @return dj_static_obstacle_id_t Nombre d'obstacles statiques importés
 */
dj_static_obstacle_id_t dj_obstacle_id_get_static_obstacle_count(
    const dj_obstacle_id_generator_t *generator);

/**
 * @brief Fonction de récupération du nombre d'obstacles dynamiques importés statiquement et dynamiquement
 * @param generator Générateur d'identifiants d'obstacles
 * @return dj_dynamic_obstacle_id_t Nombre d'obstacles dynamiques importés
 */
dj_dynamic_obstacle_id_t dj_obstacle_id_get_dynamic_obstacle_count(
    const dj_obstacle_id_generator_t *generator);

/* ******************************************* Public callback functions declarations ************************************ */
