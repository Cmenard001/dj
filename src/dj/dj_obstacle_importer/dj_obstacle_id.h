/**
 * @file dj_obstacle_id.h
 * @brief Gestion des identifiants des obstacles
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_obstacle_id.c
 */

#ifndef __DJ_OBSTACLE_ID_H__
#define __DJ_OBSTACLE_ID_H__

/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Id pour les solutions statiques des obstacles dynamiques
 * @see dj_dynamic_obstacle_id_e
 */
#define STATIC_OBSTACLE_UNKNOWN_ID ((dj_static_obstacle_id_e)(-1))

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief EnumÃ©ration des identifiants des polygones
 */
typedef enum
{
    // Ajoute ici les polygones statiques :
    //	Exemple :
    //	STATIC_OBSTACLE_EXEMPLE,

    /* Start/end area + building area */
    STATIC_OBSTACLE_START_END_AREA_ADV,
    STATIC_OBSTACLE_BUILDING_AREA_OUR_1,
    STATIC_OBSTACLE_BUILDING_AREA_OUR_2,
    STATIC_OBSTACLE_BUILDING_AREA_OUR_3,
    STATIC_OBSTACLE_BUILDING_AREA_OUR_4,
    STATIC_OBSTACLE_BUILDING_AREA_ADV_1,
    STATIC_OBSTACLE_BUILDING_AREA_ADV_2,
    STATIC_OBSTACLE_BUILDING_AREA_ADV_3,
    STATIC_OBSTACLE_BUILDING_AREA_ADV_4,

    /* Columns storage */
    STATIC_OBSTACLE_COLUMN_1,
    STATIC_OBSTACLE_COLUMN_2,
    STATIC_OBSTACLE_COLUMN_3,
    STATIC_OBSTACLE_COLUMN_4,
    STATIC_OBSTACLE_COLUMN_5,
    STATIC_OBSTACLE_COLUMN_6,
    STATIC_OBSTACLE_COLUMN_7,
    STATIC_OBSTACLE_COLUMN_8,
    STATIC_OBSTACLE_COLUMN_9,
    STATIC_OBSTACLE_COLUMN_10,

    /* PAMI start area */
    STATIC_OBSTACLE_PAMI_START_AREA_OUR,
    STATIC_OBSTACLE_PAMI_START_AREA_ADV,

    /* Platform */
    STATIC_OBSTACLE_PLATFORM,

    /* PAMI trajectories areas */
    STATIC_OBSTACLE_PAMI_START_TRAJECTORY_AREA,
    STATIC_OBSTACLE_PAMI_END_TRAJECTORY_AREA,

    /**
     * @brief Nombre d'obstacles statiques importÃ©s statiquement
     * @warning Dans la plupart des cas, ne pas utiliser cette valeur
     * car il peut y avoir des obstacles statiques importÃ©s dynamiquement
     * @see TODO: Mettre le lien vers la documentation de l'import dynamique
     */
    STATIC_OBSTACLE_COUNT,
} dj_static_obstacle_id_e;

/**
 * @brief EnumÃ©ration des identifiants des obstacles dynamiques
 */
typedef enum
{
    // Ajoute ici les obstacles dynamiques :
    //	Exemple :
    //	DYNAMIC_OBSTACLE_EXEMPLE,
    // DYNAMIC_OBSTACLE_ADV_ROBOT,

    /**
     * @brief Nombre d'obstacles dynamiques importÃ©s statiquement
     * @warning Dans la plupart des cas, ne pas utiliser cette valeur
     * car il peut y avoir des obstacles dynamiques importÃ©s dynamiquement
     * @see TODO: Mettre le lien vers la documentation de l'import dynamique
     */
    DYNAMIC_OBSTACLE_COUNT
} dj_dynamic_obstacle_id_e;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Fonction de gÃ©nÃ©ration d'un identifiant d'obstacle statique unique
 * @warning L'identifiant gÃ©nÃ©rÃ© est unique et ne pourra pas Ãªtre relachÃ©.
 * Faites attention Ã  ne pas en gÃ©nÃ©rer trop.
 *
 * @return dj_static_obstacle_id_e Identifiant d'obstacle statique unique gÃ©nÃ©rÃ©
 */
dj_static_obstacle_id_e dj_obstacle_id_generate_static_obstacle_id();

/**
 * @brief Fonction de gÃ©nÃ©ration d'un identifiant d'obstacle dynamique unique
 * @warning L'identifiant gÃ©nÃ©rÃ© est unique et ne pourra pas Ãªtre relachÃ©.
 * Faites attention Ã  ne pas en gÃ©nÃ©rer trop.
 *
 * @return dj_dynamic_obstacle_id_e Identifiant d'obstacle dynamique unique gÃ©nÃ©rÃ©
 */
dj_dynamic_obstacle_id_e dj_obstacle_id_generate_dynamic_obstacle_id();

/**
 * @brief Fonction de rÃ©cupÃ©ration du nombre d'obstacles statiques importÃ©s statiquement et dynamiquement
 *
 * @return dj_static_obstacle_id_e Nombre d'obstacles statiques importÃ©s
 */
dj_static_obstacle_id_e dj_obstacle_id_get_static_obstacle_count();

/**
 * @brief Fonction de rÃ©cupÃ©ration du nombre d'obstacles dynamiques importÃ©s statiquement et dynamiquement
 *
 * @return dj_dynamic_obstacle_id_e Nombre d'obstacles dynamiques importÃ©s
 */
dj_dynamic_obstacle_id_e dj_obstacle_id_get_dynamic_obstacle_count();

/* ******************************************* Public callback functions declarations ************************************ */

#endif
