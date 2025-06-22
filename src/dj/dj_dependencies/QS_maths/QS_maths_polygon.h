#ifndef QS_MATHS_POLYGON_H
#define QS_MATHS_POLYGON_H

#include "../QS_macro.h"
#include "../QS_types.h"
#include "QS_maths_point.h"
#include "QS_maths_segment.h"

/**
 * @brief Macro to create a polygon
 *
 * @param ... All points of the polygon in order (GEOMETRY_point_t)
 */
#define GEOMETRY_POLYGON_CREATE(...)                                                                                        \
    (GEOMETRY_polygon_t)                                                                                                    \
    {                                                                                                                       \
        .points = (GEOMETRY_point_t[]){__VA_ARGS__},                                                                        \
        .nb_points = sizeof((GEOMETRY_point_t[]){__VA_ARGS__}) / sizeof(GEOMETRY_point_t)                                   \
    }

/**
 * @brief Create an empty polygon with
 */
#define GEOMETRY_POLYGON_CREATE_N_SUMMIT(_n, _polygon_name)                                                                 \
    GEOMETRY_point_t _polygon_name##weird_not_used[_n];                                                                     \
    GEOMETRY_polygon_t _polygon_name = {.points = _polygon_name##weird_not_used, .nb_points = _n}

typedef struct
{
    int16_t x1;
    int16_t x2;
    int16_t y1;
    int16_t y2;
} GEOMETRY_rectangle_t;

/**
 * @brief Structure representing a polygon
 * @details A polygon is defined by a number of points and an array of points
 */
typedef struct
{
    uint16_t nb_points;
    GEOMETRY_point_t *points;
} GEOMETRY_polygon_t;

/**
 * @brief Fonction qui permet de savoir si un point est dans un carré
 *
 * @param x1, y1 Les coordonnées du premier point du carré
 * @param x2, y2 Les coordonnées du second point du carré
 * @param current Le point à tester
 * @return true Si le point est dans le carré
 * @return false Si le point n'est pas dans le carré
 */
bool GEOMETRY_is_in_square(int16_t x1, int16_t x2, int16_t y1, int16_t y2, GEOMETRY_point_t current);

/**
 * @brief Fonction qui permet de savoir si un point est dans un rectangle
 *
 * @param rect Le rectangle
 * @param current Le point à tester
 * @return true Si le point est dans le rectangle
 * @return false Si le point n'est pas dans le rectangle
 */
bool GEOMETRY_is_in_rectangle(GEOMETRY_rectangle_t rect, GEOMETRY_point_t current);

/**
 * @brief GEOMETRY_is_in_quadri
 * @pre Les points doivent étre donné pour que le quadrilatére soit connexe (chaques points voit tout les points)
 * @param points	Le tableau de point du quadrilatére (doit étre dans l'ordre)
 * @param tested_point Le point à tester
 * @return si le point est dans le quadrilatére
 */
bool GEOMETRY_is_in_quadri(GEOMETRY_point_t points[4], GEOMETRY_point_t tested_point);

/**
 * @brief GEOMETRY_is_in_polygon
 * @param polygon Les points du polygone
 * @param nb_summits Le nombre de sommets
 * @param tested_point Le point que l'on veut tester si il est dans le polygone
 * @param out_point Un point dont on est sur qu'il est en dehors du polygone (par exemple en dehors du terrain
 * @return
 */
bool GEOMETRY_is_in_polygon(GEOMETRY_point_t polygon[],
                            uint8_t nb_summits,
                            GEOMETRY_point_t tested_point,
                            GEOMETRY_point_t out_point,
                            uint8_t *stock_intersections);

/**
 * @brief Test if a point is strictly in a polygon
 * @note If the point is on the edge of the polygon, it is considered as outside the polygon
 * @param polygon Les points du polygone
 * @param nb_summits Le nombre de sommets
 * @param tested_point Le point que l'on veut tester si il est dans le polygone
 * @param out_point Un point dont on est sur qu'il est en dehors du polygone (par exemple en dehors du terrain
 * @return
 */
bool GEOMETRY_is_strictly_in_polygon(GEOMETRY_point_t polygon[],
                                     uint8_t nb_summits,
                                     GEOMETRY_point_t tested_point,
                                     GEOMETRY_point_t out_point,
                                     uint8_t *stock_intersections);

/**
 * @brief Function to know if a segment intersects a polygon
 * @note If the segment is on the edge of the polygon, it is considered as intersecting the polygon
 *
 * @param polygon Polygon to test
 * @param segment Segment to test
 * @return true If the segment intersects the polygon
 * @return false If the segment does not intersect the polygon
 */
bool GEOMETRY_polygon_intersects_segment(GEOMETRY_polygon_t *polygon, GEOMETRY_segment_t segment);

/**
 * @brief Function to know if a segment strictly intersects a polygon
 * @note If the segment is on the edge of the polygon, it is not considered as intersecting the polygon
 * @warning If the segment is very close to a summit but inside the polygon,
    it can be not detected as intersecting the polygon due to approximation
 *
 * @param polygon Polygon to test
 * @param segment Segment to test
 * @return true If the segment intersects the polygon
 * @return false If the segment does not intersect the polygon
 */
bool GEOMETRY_polygon_strictly_intersects_segment(GEOMETRY_polygon_t *polygon, GEOMETRY_segment_t segment);

/**
 * @brief Create a polygon
 *
 * @param nb_points number of points in the polygon
 * @return GEOMETRY_polygon_t* pointer to the polygon created
 *
 * @note The polygon must be destroyed with GEOMETRY_polygon_destroy
 * @note Polygon points are not initialized
 */
GEOMETRY_polygon_t *GEOMETRY_polygon_create(uint16_t nb_points);

/**
 * @brief Destroy a polygon
 *
 * @param polygon pointer to the polygon to destroy
 */
void GEOMETRY_polygon_destroy(GEOMETRY_polygon_t *polygon);

/**
 * @brief Function to check if a point is a node of a polygon
 *
 * @param polygon The polygon to check
 * @param point The point to check
 *
 * @return The index of the point in the polygon if it is a node
 * @return -1 if the point is not a node of the polygon
 */
int16_t GEOMETRY_polygon_contains_point(GEOMETRY_polygon_t *polygon, GEOMETRY_point_t point);

/**
 * @brief Function to project a point on a polygon
 *
 * @details The projeted point will be the closest projection of the point on each edge of the polygon
 *
 * @param polygon The polygon to project the point on
 * @param point The point to project
 *
 * @return The projected point on the polygon
 */
GEOMETRY_point_t GEOMETRY_polygon_project_point(GEOMETRY_polygon_t *polygon, GEOMETRY_point_t point);

/**
 * @brief Set the points of a polygon in trigonometric order
 *
 * @details This function will reverse the order of the points if the polygon is in hourly order
 *
 * @param polygon : the polygon to set in trigonometric order
 */
void GEOMETRY_polygon_set_in_trigonometric_order(GEOMETRY_polygon_t *polygon);

/**
 * @brief Fusion two polygons
 *
 * @details This function will create a new polygon that is the fusion of the two polygons
 *
 * @param polygon1 : the first polygon
 * @param polygon2 : the second polygon
 * @param out_polygon : the polygon that will be the fusion of the two polygons
 * @param nb_points_max_in_out_polygon : the maximum number of points in the output polygon
 * @note if the output polygon is not big enough, the function will return false
 *
 * @return true if the fusion was successful, false otherwise
 *
 * @warning Enough points in the output polygon must be allocated.
 * @warning The input polygons can be modified if they are not in trigonometric order
 * @warning Do not use the same polygon for an input and output
 * @note The output polygon will be in trigonometric order
 */
bool GEOMETRY_polygon_fusion(GEOMETRY_polygon_t *polygon1,
                             GEOMETRY_polygon_t *polygon2,
                             GEOMETRY_polygon_t *out_polygon,
                             uint8_t nb_points_max_in_out_polygon);

/**
 * @brief Function to find the closest point of a polygon to a point
 * @details The function will can return a submit or a point on the edge of the polygon
 *
 * @param polygon The polygon to search in
 * @param point The point to search for
 * @return GEOMETRY_point_t The closest point of the polygon to the point
 */
GEOMETRY_point_t GEOMETRY_polygon_find_closest_point(GEOMETRY_polygon_t *polygon, GEOMETRY_point_t point);

/**
 * @brief Create a regular polygon at the specifed [position]
 *
 * @param polygon the polygon instance to use. Make sure to create the point array
 * @param nb_summit number of summit for the regular polygon
 * @param rayon the "radius" for the regular polygon (from center to summit)
 * @param position the center position for the regular polygon
 */
void GEOMETRY_polygon_create_regular(GEOMETRY_polygon_t *polygon,
                                     uint32_t nb_summit,
                                     uint16_t rayon,
                                     GEOMETRY_point_t *position);

#endif
