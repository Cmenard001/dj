#include "QS_maths_vector.h"

#include <math.h>

float GEOMETRY_dot_product(vector_2d_t *v, vector_2d_t *u)
{
    return v->x * u->x + v->y * u->y;
}

int GEOMETRY_vector_resize_absolute(point_2d_t *end, point_2d_t *start, point_2d_t *new_end, float norme)
{
    float delta_y = end->y - start->y;
    float delta_x = end->x - start->x;
    if ((delta_x == 0) && (delta_y == 0))
    {
        return -1;
    }
    float angle = atan2f(delta_y, delta_x);
    new_end->x = start->x + cosf(angle) * norme;
    new_end->y = start->y + sinf(angle) * norme;
    return 0;
}

int GEOMETRY_vector_resize_relative(point_2d_t *end, point_2d_t *start, point_2d_t *new_end, int16_t offset)
{
    float delta_y = end->y - start->y;
    float delta_x = end->x - start->x;
    if ((delta_x == 0) && (delta_y == 0))
    {
        return -1;
    }
    float angle = atan2f(delta_y, delta_x);
    float norme = sqrtf(delta_x * delta_x + delta_y * delta_y) + offset;
    new_end->x = start->x + cosf(angle) * norme;
    new_end->y = start->y + sinf(angle) * norme;
    return 0;
}