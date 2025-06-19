#ifndef QS_MATHS_VECTOR_H
#define QS_MATHS_VECTOR_H

#include "../QS_macro.h"
#include "../QS_types.h"
#include "QS_maths_point.h"


typedef struct
{
    int16_t x;
    int16_t y;
} GEOMETRY_vector_t;

float GEOMETRY_dot_product(vector_2d_t *v, vector_2d_t *u);

/**
 * @brief Change la norme d'un vecteur défini par son point d'arrivé et de départ
 * @param end point de fin du vecteur
 * @param start point de départ du vecteur
 * @param new_end point calculé pour la nouvelle "fin" du vecteur
 * @param norme la nouvelle norme du vecteur
 * @return -1 Si il y a une erreur dans les maths (point superposer par exemple) 0 sinon
 */
int GEOMETRY_vector_resize_absolute(point_2d_t *end, point_2d_t *start, point_2d_t *new_end, float norme);

/**
 * @brief Change la norme d'un vecteur défini par son point d'arrivé et de départ en prenant un compte un ofsset
 * @param end point de fin du vecteur
 * @param start point de départ du vecteur
 * @param new_end point calculé pour la nouvelle "fin" du vecteur
 * @param offset l'offset à appliquer sur la norme du vecteur
 * @return -1 Si il y a une erreur dans les maths (point superposer par exemple) 0 sinon
 */
int GEOMETRY_vector_resize_relative(point_2d_t *end, point_2d_t *start, point_2d_t *new_end, int16_t offset);

#endif
