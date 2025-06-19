#include "QS_maths_point.h"

#include <math.h>

inline bool GEOMETRY_point_compare(GEOMETRY_point_t a, GEOMETRY_point_t b)
{
    return a.x == b.x && a.y == b.y;
}

uint16_t GEOMETRY_distance(GEOMETRY_point_t a, GEOMETRY_point_t b)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

uint32_t GEOMETRY_distance_square(GEOMETRY_point_t a, GEOMETRY_point_t b)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return (dx * dx + dy * dy);
}

uint32_t GEOMETRY_pythagore(uint32_t d1, uint32_t d2)
{
    return (uint32_t)sqrt(d1 * d1 + d2 * d2);
}

uint16_t GEOMETRY_manhattan_distance(GEOMETRY_point_t a, GEOMETRY_point_t b)
{
    return (a.x > b.x ? a.x - b.x : b.x - a.x) + (a.y > b.y ? a.y - b.y : b.y - a.y);
}

/**
 * @brief calcule la distance entre deux points compatible
 *        avec toute structure héritée de point_2d_t
 * @param a premier point
 * @param b second point
 * @return float distance entre les deux points
 */
float GEOMETRY_distance_f(point_2d_t *a, point_2d_t *b)
{
    float delta_x = a->x - b->x;
    float delta_y = a->y - b->y;
    return sqrtf((delta_x * delta_x) + (delta_y * delta_y));
}

int32_t GEOMETRY_cross_product(GEOMETRY_point_t a, GEOMETRY_point_t b, GEOMETRY_point_t c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
