/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : QS_maths.h
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions mathématiques utiles
 *  Auteur : Arnaud
 *  Version 20130518
 */

/** ----------------  Defines possibles  --------------------
 *	FAST_COS_SIN				: Calcul rapide des cos et sin à l'aide d'un GRAND tableau de valeur
 */
#ifndef QS_MATHS_H
#define QS_MATHS_H

#include <math.h>

#include "../QS_macro.h"
#include "../QS_types.h"
#include "QS_maths_angle.h"
#include "QS_maths_circle.h"
#include "QS_maths_cluster.h"
#include "QS_maths_point.h"
#include "QS_maths_polygon.h"
#include "QS_maths_position.h"
#include "QS_maths_segment.h"
#include "QS_maths_vector.h"

// IMPORTANT IL FAUT UTILISER UN TABLEAU DE TYPE SINT32, sinon ça va merder violement !

// L'utilisation d'un filtre à temps futur ou passé introduit un déphasage (avance pour temps futur, retard pour temps passé)
// Pour contrer ce déphasage il est conseillé d'utiliser les deux filtres à la fois.
// ex :
//		filter_future_time(values, 30, (float[]){1/2., 1/2.}, 2);
//		filter_past_time(values, 30, (float[]){1/2., 1/2.}, 2);
//
//	Subtilité pour dérivé votre tableau de donnée faite :
//		filter_future_time(values, 30, (float[]){1, -1}, 2);

/** Filtre à temps future
 * @param values	: tableau de donnée é traiter
 * @param nb_value	: nombre de valeur é traiter dans le tableau
 * @param factor	: tableau des facteurs
 * @param nb_factor	: nombre de facteurs
 *
 * @brief exemple : filtre moyenneur sur deux valeurs :
 *						filter_future_time(values, 30, (float[]){1/2., 1/2.}, 2);
 *						résultat pour la premiére valeur :	values[0] = 1/2 * values[0] + 1/2 values[1];
 *						effet de bord : values[29] = values[29];
 */
void filter_future_time(int32_t values[], uint16_t nb_value, float factor[], uint8_t nb_factor);

/** Filtre à temps passé
 * @param values	: tableau de donnée à traiter
 * @param nb_value	: nombre de valeur à traiter dans le tableau
 * @param factor	: tableau des facteurs
 * @param nb_factor	: nombre de facteurs
 *
 * @brief exemple : filtre moyenneur sur deux valeurs :
 *						filter_past_time(values, 30, (float[]){1/2., 1/2.}, 2);
 *						résultat pour la premiére valeur :	values[29] = 1/2 * values[29] + 1/2 values[28];
 *						effet de bord : values[0] = values[0];
 */
void filter_past_time(int32_t values[], uint16_t nb_value, float factor[], uint8_t nb_factor);

/** Recherche de la valeur minimale dans un tableau
 * @param values	: tableau de donnée à traiter
 * @param nbValue	: nombre de valeur à traiter dans le tableau
 * @return indice de la valeur minimale
 */
uint16_t searchMin(int32_t values[], uint16_t nbValue);

/** Recherche de la valeur maximale dans un tableau
 * @param values	: tableau de donnée à traiter
 * @param nbValue	: nombre de valeur à traiter dans le tableau
 * @return indice de la valeur maximale
 */
uint16_t searchMax(int32_t values[], uint16_t nbValue);

/** Calcul de la factorielle
 * @param nb	: nombre dont on veut calculer la factorielle
 * @return factorielle de nb
 */
uint64_t factorielle(uint32_t nb);

/** Calcul de la régression linéaire
 * @param angle		: angle de la droite
 * @param offset	: ordonnée à l'origine
 * @param quality	: qualité de la régression
 * @param points	: tableau des points
 * @param n			: nombre de points
 * @param axis		: axe sur lequel on veut faire la régression
 * @return true si la régression est possible, false sinon
 */
bool GEOMETRY_linear_regression(
    int16_t *angle, int16_t *offset, int8_t *quality, GEOMETRY_point_t *points, int32_t n, axis_e axis);

/**
 * @brief Calcul de l'angle absolue quand on regarde un point depuis un autre
 *
 * @param start_x, start_y Point regardant
 * @param destination_x, destination_y Point regardé
 * @return int16_t Angle calculé
 */
int16_t GEOMETRY_viewing_angle(int16_t start_x, int16_t start_y, int16_t destination_x, int16_t destination_y);

/**
 * @brief Calcul le point pour une distance dist à partir d'un point donné et dans un angle donné.
 * @param start_point point de référence
 * @param angle angle absolu sur le terrain
 * @param dist distance entre la référence et le point calculé
 * @param point point calculé
 */
void GEOMETRY_calculate_point_from_distance_and_angle(GEOMETRY_point_t start_point,
                                                      int16_t angle,
                                                      int16_t dist,
                                                      GEOMETRY_point_t *point);

/**
 * @brief Calcul de la distance que l'on a à parcourir entre la position actuelle et la position finale ( en mm )
 * et calcul de l'angle
 *
 * @param start_x, start_y Position actuelle
 * @param destination_x, destination_y Position finale
 * @param angle_de_vue Angle de vue
 * @return int32_t Distance à parcourir
 */
int32_t GEOMETRY_viewing_algebric_distance(
    int32_t start_x, int32_t start_y, int32_t destination_x, int32_t destination_y, int32_t angle_de_vue);

int32_t GEOMETRY_viewing_algebric_distance_mm16(
    int32_t start_x, int32_t start_y, int32_t destination_x, int32_t destination_y, int32_t angle_de_vue);

/**
 * @brief Fonction qui permet d'obtenir une position de consigne du robot de manière a prendre une bouée dans un de ses
 * compartiments
 * @param *x_consigne : consigne en x que le robot devra suivre
 * @param *y_consigne : consigne en y que le robot devra suivre
 * @param buoy_x : position en x de la bouée cible
 * @param buoy_y : position en y de la bouée cible
 * @param x_emplacement : position en x, relativement au centre du robot, du compartiment dans lequel on souhaite récupérer
 * la bouée EX : X_LOCATION_FRONT_RIGHT
 * @param x_emplacement : position en y, relativement au centre du robot, du compartiment dans lequel on souhaite récupérer
 * la bouée EX : Y_LOCATION_FRONT_SIDE
 * @param push_element_over : distance en mm sur laquelle le robot doit pousser la bouée (permet de s'assurer qu'elle est
 * bien au fond de l'emplacement avant de refermer un bras)
 */
void GEOMETRY_where_to_go_to_take(int32_t *x_consigne,
                                  int32_t *y_consigne,
                                  int32_t buoy_x,
                                  int32_t buoy_y,
                                  int32_t x_emplacement,
                                  int32_t y_emplacement,
                                  int32_t push_element_over,
                                  int32_t *x_act_event,
                                  int32_t *y_act_event,
                                  int32_t dist_act_event);

float GEOMETRY_atof(char *s);

#endif /* ndef QS_MATHS_H */
