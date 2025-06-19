/**
 * @file dj.h
 * @brief Dj is an algorithm that allows the robot to move from point A to point B while
 * avoiding static and dynamic obstacles
 * @note This file must be overriden by the user with the correct interface
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj.c
 */

#ifndef __DJ_H__
#define __DJ_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj_config.h"
#include "dj_obstacle_importer/dj_obstacle_id.h"
#if DJ_ROBOT_ESEO_FRAMEWORK
#include "propulsion/movement.h"
#endif /* DJ_ROBOT_ESEO_FRAMEWORK */

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialisation de l'algorithme de pathfinding dj
 * @warning Cette fonction doit Ãªtre au dÃ©marrage du robot
 */
void dj_init();

/**
 * @brief Fonction d'initialisation de dj
 * @warning Cette fonction doit Ãªtre au lancement du match (quand la couleur est connue)
 */
void dj_init_at_match_launch();

/**
 * @brief Fonction d'initialisation de dj aprÃ¨s que la couleur ai Ã©tÃ© changÃ©e
 * @warning Cette fonction doit Ãªtre appelÃ©e Ã  chaque fois que la couleur est changÃ©e
 */
void dj_init_after_color_change();

#if DJ_ROBOT_ESEO_FRAMEWORK

/**
 * @brief Fonction de mise Ã  jour de l'algorithme de pathfinding dj
 * @warning Cette fonction doit Ãªtre dans le main
 */
void dj_process_main();

/**
 * @brief RÃ©alise un dÃ©placement en Ã©vitant des obstacles statiques et dynamiques en utilisant l'algorithme dj
 *        avec des paramÃ¨tres personnalisÃ©s
 *
 * @pre	: la position du robot doit Ãªtre Ã  jour
 * @post   : la pile asser est vidÃ©e
 *
 * @param x,y			CoordonnÃ©e du point cible
 * @param in_progress	Etat en cours
 * @param success_state	Etat Ã  retourner si le dÃ©placement s'est terminÃ© correctement
 * @param fail_state	Etat Ã  retourner si le dÃ©placement ne s'est pas terminÃ© correctement
 * @param speed			Vitesse de croisiÃ¨re du robot
 * @param way			Sens de dÃ©placement
 * @param avoidance		Type d'evitement Ã  faire
 * @param end_condition	Comportement du robot en fin de trajectoire
 * @param retry_without_dynamic_obstacle Si true, le robot essaiera de se dÃ©placer sans tenir compte des obstacles dynamiques
 *        si aucun chemin n'est trouvÃ© avec les obstacles dynamiques.
 * @return le state rentrÃ© en argument correspondant au resultat du dÃ©placement
 */
uint8_t dj_try_going_with_params(uint16_t x,
                                 uint16_t y,
                                 uint8_t in_progress,
                                 uint8_t success_state,
                                 uint8_t fail_state,
                                 PROP_speed_e speed,
                                 way_e way,
                                 avoidance_e avoidance,
                                 STRAT_endCondition_e end_condition,
                                 bool retry_without_dynamic_obstacle);

/**
 * @brief RÃ©alise un dÃ©placement en Ã©vitant des obstacles statiques et dynamiques en utilisant l'algorithme dj
 *
 * @pre	: la position du robot doit Ãªtre Ã  jour
 * @post   : la pile asser est vidÃ©e
 *
 * @param x,y			CoordonnÃ©e du point cible
 * @param in_progress	Etat en cours
 * @param success_state	Etat Ã  retourner si le dÃ©placement s'est terminÃ© correctement
 * @param fail_state	Etat Ã  retourner si le dÃ©placement ne s'est pas terminÃ© correctement
 * @param speed			Vitesse de croisiÃ¨re du robot
 * @param way			Sens de dÃ©placement
 * @param avoidance		Type d'evitement Ã  faire
 * @param end_condition	Comportement du robot en fin de trajectoire
 *
 * @return le state rentrÃ© en argument correspondant au resultat du dÃ©placement
 */
uint8_t dj_try_going(uint16_t x,
                     uint16_t y,
                     uint8_t in_progress,
                     uint8_t success_state,
                     uint8_t fail_state,
                     PROP_speed_e speed,
                     way_e way,
                     avoidance_e avoidance,
                     STRAT_endCondition_e end_condition);

#endif /* DJ_ROBOT_ESEO_FRAMEWORK */

/**
 * @brief Fonction pour activer ou dÃ©sactiver un obstacle statique
 *
 * @param polygon_id ID de l'obstacle Ã  activer ou dÃ©sactiver
 * @param enable true pour activer l'obstacle, false pour le dÃ©sactiver
 */
void dj_enable_static_polygon(dj_static_obstacle_id_e polygon_id, bool enable);

/**
 * @brief Fonction pour activer ou dÃ©sactiver un obstacle dynamique
 *
 * @param polygon_id ID de l'obstacle Ã  activer ou dÃ©sactiver
 * @param enable true pour activer l'obstacle, false pour le dÃ©sactiver
 */
void dj_enable_dynamic_polygon(dj_dynamic_obstacle_id_e polygon_id, bool enable);

/**
 * @brief Fonction pour vÃ©rifier si un obstacle statique est activÃ©
 *
 * @param polygon_id ID de l'obstacle Ã  vÃ©rifier
 *
 * @return true si l'obstacle est activÃ©, false sinon
 */
bool dj_is_static_polygon_enabled(dj_static_obstacle_id_e polygon_id);

/**
 * @brief Fonction pour vÃ©rifier si un obstacle dynamique est activÃ©
 *
 * @param polygon_id ID de l'obstacle Ã  vÃ©rifier
 *
 * @return true si l'obstacle est activÃ©, false sinon
 */
bool dj_is_dynamic_polygon_enabled(dj_dynamic_obstacle_id_e polygon_id);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
