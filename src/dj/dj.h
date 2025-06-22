/**
 * @file dj.h
 * @brief Dj is an algorithm that allows the robot to move from point A to point B while
 * avoiding static and dynamic obstacles
 * @note This file must be overriden by the user with the correct interface
 * @author Cyprien Ménard
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
 * @warning Cette fonction doit être au démarrage du robot
 */
void dj_init();

/**
 * @brief Fonction d'initialisation de dj
 * @warning Cette fonction doit être au lancement du match (quand la couleur est connue)
 */
void dj_init_at_match_launch();

/**
 * @brief Fonction d'initialisation de dj après que la couleur ait été changée
 * @warning Cette fonction doit être appelée à chaque fois que la couleur est changée
 */
void dj_init_after_color_change();

#if DJ_ROBOT_ESEO_FRAMEWORK

/**
 * @brief Fonction de mise à jour de l'algorithme de pathfinding dj
 * @warning Cette fonction doit être dans le main
 */
void dj_process_main();

/**
 * @brief Réalise un déplacement en évitant des obstacles statiques et dynamiques en utilisant l'algorithme dj
 *        avec des paramètres personnalisés
 *
 * @pre  : la position du robot doit être à jour
 * @post : la pile asser est vidée
 *
 * @param x,y            Coordonnée du point cible
 * @param in_progress    Etat en cours
 * @param success_state  Etat à retourner si le déplacement s'est terminé correctement
 * @param fail_state     Etat à retourner si le déplacement ne s'est pas terminé correctement
 * @param speed          Vitesse de croisière du robot
 * @param way            Sens de déplacement
 * @param avoidance      Type d'évitement à faire
 * @param end_condition  Comportement du robot en fin de trajectoire
 * @param retry_without_dynamic_obstacle Si true, le robot essaiera de se déplacer sans tenir compte des obstacles dynamiques
 *        si aucun chemin n'est trouvé avec les obstacles dynamiques.
 * @return le state rentré en argument correspondant au résultat du déplacement
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
 * @brief Réalise un déplacement en évitant des obstacles statiques et dynamiques en utilisant l'algorithme dj
 *
 * @pre  : la position du robot doit être à jour
 * @post : la pile asser est vidée
 *
 * @param x,y            Coordonnée du point cible
 * @param in_progress    Etat en cours
 * @param success_state  Etat à retourner si le déplacement s'est terminé correctement
 * @param fail_state     Etat à retourner si le déplacement ne s'est pas terminé correctement
 * @param speed          Vitesse de croisière du robot
 * @param way            Sens de déplacement
 * @param avoidance      Type d'évitement à faire
 * @param end_condition  Comportement du robot en fin de trajectoire
 *
 * @return le state rentré en argument correspondant au résultat du déplacement
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
 * @brief Fonction pour activer ou désactiver un obstacle statique
 *
 * @param polygon_id ID de l'obstacle à activer ou désactiver
 * @param enable true pour activer l'obstacle, false pour le désactiver
 */
void dj_enable_static_polygon(dj_static_obstacle_id_e polygon_id, bool enable);

/**
 * @brief Fonction pour activer ou désactiver un obstacle dynamique
 *
 * @param polygon_id ID de l'obstacle à activer ou désactiver
 * @param enable true pour activer l'obstacle, false pour le désactiver
 */
void dj_enable_dynamic_polygon(dj_dynamic_obstacle_id_e polygon_id, bool enable);

/**
 * @brief Fonction pour vérifier si un obstacle statique est activé
 *
 * @param polygon_id ID de l'obstacle à vérifier
 *
 * @return true si l'obstacle est activé, false sinon
 */
bool dj_is_static_polygon_enabled(dj_static_obstacle_id_e polygon_id);

/**
 * @brief Fonction pour vérifier si un obstacle dynamique est activé
 *
 * @param polygon_id ID de l'obstacle à vérifier
 *
 * @return true si l'obstacle est activé, false sinon
 */
bool dj_is_dynamic_polygon_enabled(dj_dynamic_obstacle_id_e polygon_id);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
