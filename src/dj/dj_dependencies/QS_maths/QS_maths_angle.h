#ifndef QS_MATHS_ANGLE_H
#define QS_MATHS_ANGLE_H

#include "../QS_macro.h"
#include "../QS_types.h"

#define PI4096_ANGLE_TYPE int16_t

#define PI16384 51472
#define HALF_PI16384 25736
#define QUATER_PI16384 12868
#define THREE_HALF_PI16384 77208
#define TWO_PI16384 102944
#define PI_22 (13176795) /*Valeur de PI<<22 */
#define TWO_PI22 (26353589)
#define PI_28 (843314856) /*Valeur de PI<<28 */
#define TWO_PI28 (1686629713)

#define TWO_PI4096 (25736)
#define HALF_PI4096 (6434)
#define THREE_HALF_PI4096 (19302)

#define RAD_TO_DEG(x) ((x)*180 / PI4096)
#define DEG_TO_RAD(x) ((x)*PI4096 / 180.0)

/**
 * @brief Renvoi le cosinus et le sinus de l'angle teta.
 *
 * @warning ATTENTION, aucune vérification n'est faite sur l'angle demandé.
 *
 * @param teta L'angle en PI4096
 * @param cos [out] Le cosinus de l'angle
 * @param sin [out] Le sinus de l'angle
 */
void COS_SIN_4096_get(int16_t teta, int16_t *cos, int16_t *sin);
void COS_SIN_16384_get(int32_t teta, int16_t *cos, int16_t *sin);

/**
 * @brief Renvoi le cosinus de l'angle teta en float
 *
 * @param angle L'angle en PI4096
 * @return float Le cosinus de l'angle
 */
float cos4096(int16_t angle);

/**
 * @brief Renvoi le sinus de l'angle teta en float
 *
 * @param angle L'angle en PI4096
 * @return float Le sinus de l'angle
 */
float sin4096(int16_t angle);

/**
 * @brief Renvoi la tangente de l'angle teta en float
 *
 * @param angle L'angle en PI4096
 * @return float La tangente de l'angle
 */
float tan4096(int angle);

/**
 * @brief Renvoi l'angle en PI4096 de la tangente
 *
 * @param tangent La tangente
 * @return int16_t L'angle en PI4096
 */
int16_t atan4096(float tangent);

/**
 * @brief Renvoi l'angle en PI4096 de la tangente
 *
 * @param y L'ordonnée
 * @param x L'abscisse
 * @return int16_t L'angle en PI4096
 */
int16_t atan2_4096(float y, float x);

/**
 * @brief Fonction de conversion d'un angle en PI4096 en degré
 *
 * @param angle L'angle en PI4096
 * @return int16_t L'angle en degrés
 */
int16_t rad2deg(int16_t angle);

/**
 * @brief Fonction de modulo pour les angles en PI4096
 * @note Renvoi un angle entre -PI4096 et PI4096
 *
 * @param angle Angle à moduler
 * @return int16_t Angle modulé
 */
int16_t GEOMETRY_modulo_angle(int16_t angle);

/**
 * @brief Fonction de modulo pour les angles en PI22
 *
 * @param angle L'angle à moduler
 * @return int32_t L'angle modulé
 */
int32_t GEOMETRY_modulo_angle_22(int32_t angle);

/**
 * @brief Fonction de modulo pour les angles en float
 *
 * @param rad L'angle en radian
 * @return float L'angle modulé
 */
float GEOMETRY_modulo_angle_float(float rad);

#endif
