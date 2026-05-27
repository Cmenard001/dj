/**
 * @file segment.c
 * @brief Segment utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see segment.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "segment.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include "utils/maths/distance/distance.h"
#include "utils/maths/point/point.h"
#include "utils/maths/vector/vector.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

bool segments_get_intersection(const segment_t *s1, const segment_t *s2, point_t *intersection)
{
    SYSTEM_ASSERT(s1 != NULL);
    SYSTEM_ASSERT(s2 != NULL);
    /* On calcule les vecteur orthogonaux à s1 et s2 */

    vector_2d_distance_t u1 = {.x = -(s1->b.y - s1->a.y), .y = (s1->b.x - s1->a.x)};
    vector_2d_distance_t u2 = {.x = -(s2->b.y - s2->a.y), .y = (s2->b.x - s2->a.x)};

    /* on projète s1 et s2 sur u1 */

    distance_t proj_s1_a = s1->a.x * u1.x + s1->a.y * u1.y;
    distance_t proj_s1_b = s1->b.x * u1.x + s1->b.y * u1.y;
    distance_t proj_s2_a = s2->a.x * u1.x + s2->a.y * u1.y;
    distance_t proj_s2_b = s2->b.x * u1.x + s2->b.y * u1.y;

    /*
     * Si les projetés ne se chevauchent pas, les segments ne
     * se croisent pas
     */

    if (MIN(proj_s1_a, proj_s1_b) > MAX(proj_s2_a, proj_s2_b) ||
        MAX(proj_s1_a, proj_s1_b) < MIN(proj_s2_a, proj_s2_b))
    {
        return false;
    }

    /* sinon on projète s1 et s2 sur u2 et on recommence*/

    proj_s1_a = s1->a.x * u2.x + s1->a.y * u2.y;
    proj_s1_b = s1->b.x * u2.x + s1->b.y * u2.y;
    proj_s2_a = s2->a.x * u2.x + s2->a.y * u2.y;
    proj_s2_b = s2->b.x * u2.x + s2->b.y * u2.y;

    /*
     * Si les projetés ne se chevauchent pas, les segments ne
     * se croisent pas
     */

    if (MIN(proj_s1_a, proj_s1_b) > MAX(proj_s2_a, proj_s2_b) ||
        MAX(proj_s1_a, proj_s1_b) < MIN(proj_s2_a, proj_s2_b))
    {
        return false;
    }

    // Rajout d'une protection car deux segements sur une même ligne ne se coupent pas forcément
    if ((s1->a.x < MIN(s2->a.x, s2->b.x) && s1->b.x < MIN(s2->a.x, s2->b.x)) ||
        (s1->a.x > MAX(s2->a.x, s2->b.x) && s1->b.x > MAX(s2->a.x, s2->b.x)) ||
        (s1->a.y < MIN(s2->a.y, s2->b.y) && s1->b.y < MIN(s2->a.y, s2->b.y)) ||
        (s1->a.y > MAX(s2->a.y, s2->b.y) && s1->b.y > MAX(s2->a.y, s2->b.y)))
    {
        return false;
    }

    if (intersection == NULL)
    {
        return true; // On ne veut pas l'intersection
    }

    // Calcul de l'intersection
    distance_t det =
        (s1->b.x - s1->a.x) * (s2->b.y - s2->a.y) - (s1->b.y - s1->a.y) * (s2->b.x - s2->a.x);
    if (distance_null(det))
    {
        // Les segments sont parallèles ou colinéaires
        if (segments_colinear(s1, s2))
        {
            // Vérification si les segments colinéaires se chevauchent
            if (MAX(s1->a.x, s1->b.x) < MIN(s2->a.x, s2->b.x) ||
                MAX(s2->a.x, s2->b.x) < MIN(s1->a.x, s1->b.x) ||
                MAX(s1->a.y, s1->b.y) < MIN(s2->a.y, s2->b.y) ||
                MAX(s2->a.y, s2->b.y) < MIN(s1->a.y, s1->b.y))
            {
                return false; // Pas de chevauchement
            }

            if (intersection != NULL)
            {
                // Retourne un point d'intersection arbitraire dans le chevauchement
                intersection->x = MAX(MIN(s1->a.x, s1->b.x), MIN(s2->a.x, s2->b.x));
                intersection->y = MAX(MIN(s1->a.y, s1->b.y), MIN(s2->a.y, s2->b.y));
            }
            return true;
        }
        return false; // Les segments sont parallèles mais non colinéaires
    }

    float t = ((float)(s2->a.x - s1->a.x) * (float)(s2->b.y - s2->a.y) -
               (float)(s2->a.y - s1->a.y) * (float)(s2->b.x - s2->a.x)) /
              (float)det;
    float u = ((float)(s2->a.x - s1->a.x) * (float)(s1->b.y - s1->a.y) -
               (float)(s2->a.y - s1->a.y) * (float)(s1->b.x - s1->a.x)) /
              (float)det;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        intersection->x = s1->a.x + t * (s1->b.x - s1->a.x);
        intersection->y = s1->a.y + t * (s1->b.y - s1->a.y);
        return true;
    }

    return false;
}

bool segments_intersects(const segment_t *s1, const segment_t *s2)
{
    SYSTEM_ASSERT(s1 != NULL);
    SYSTEM_ASSERT(s2 != NULL);
    return segments_get_intersection(s1, s2, NULL);
}

bool point_on_segment(const segment_t *seg, const point_t *tested_point)
{
    SYSTEM_ASSERT(seg != NULL);
    SYSTEM_ASSERT(tested_point != NULL);
    // On fait le produit vectoriel entre les vecteurs formés par les points du segment et le point
    // testé. Si le produit vectoriel est nul, le point est sur la droite formée par le segment.
    vector_2d_distance_t v1 = {.x = seg->b.x - seg->a.x, .y = seg->b.y - seg->a.y};
    vector_2d_distance_t v2 = {.x = tested_point->x - seg->a.x, .y = tested_point->y - seg->a.y};
    distance_t cross_product = v1.x * v2.y - v1.y * v2.x;
    distance_t segment_length_sq = v1.x * v1.x + v1.y * v1.y;
    distance_t normalized_distance = fabsf(cross_product) / sqrtf(segment_length_sq);
    if (!distance_null(normalized_distance))
    {
        return false;
    }

    // On vérifie ensuite que le point est bien entre les deux extrémités du segment.
    // Pour ça on vérifie que les coordonnées du point sont entre les coordonnées des extrémités du segment.
    return (tested_point->x >= MIN(seg->a.x, seg->b.x)) &&
           (tested_point->x <= MAX(seg->a.x, seg->b.x)) &&
           (tested_point->y >= MIN(seg->a.y, seg->b.y)) &&
           (tested_point->y <= MAX(seg->a.y, seg->b.y));
}

bool segments_parallel(const segment_t *seg1, const segment_t *seg2)
{
    SYSTEM_ASSERT(seg1 != NULL);
    SYSTEM_ASSERT(seg2 != NULL);
    // Cas des segments verticaux
    if (distances_equal(seg1->a.x, seg1->b.x) && distances_equal(seg2->a.x, seg2->b.x))
    {
        return true;
    }

    // Cas des segments horizontaux
    if (distances_equal(seg1->a.y, seg1->b.y) && distances_equal(seg2->a.y, seg2->b.y))
    {
        return true;
    }

    // Vérification des pentes sans division pour éviter la division par zéro
    distance_t deltaX1 = seg1->b.x - seg1->a.x;
    distance_t deltaY1 = seg1->b.y - seg1->a.y;
    distance_t deltaX2 = seg2->b.x - seg2->a.x;
    distance_t deltaY2 = seg2->b.y - seg2->a.y;

    // Deux segments sont parallèles si deltaY1 * deltaX2 == deltaY2 * deltaX1
    // On utilise une tolérance pour les erreurs de calcul flottant
    if (fabsf(deltaY1 * deltaX2 - deltaY2 * deltaX1) < 1e-3f)
    {
        return true;
    }

    return false;
}

bool segments_get_strictly_intersection(const segment_t *s1,
                                        const segment_t *s2,
                                        point_t *intersection)
{
    SYSTEM_ASSERT(s1 != NULL);
    SYSTEM_ASSERT(s2 != NULL);
    /* On vérifie d'abord si une des extrémités d'un segment est sur l'autre segment */
    if (point_on_segment(s1, &s2->a) || point_on_segment(s1, &s2->b) ||
        point_on_segment(s2, &s1->a) || point_on_segment(s2, &s1->b))
    {
        return false;
    }

    /* On vérifie si les segments se croisent */
    return segments_get_intersection(s1, s2, intersection);
}

bool segments_strictly_intersects(const segment_t *s1, const segment_t *s2)
{
    SYSTEM_ASSERT(s1 != NULL);
    SYSTEM_ASSERT(s2 != NULL);
    return segments_get_strictly_intersection(s1, s2, NULL);
}

void segment_middle(const segment_t *s, point_t *middle)
{
    SYSTEM_ASSERT(s != NULL);
    SYSTEM_ASSERT(middle != NULL);
    middle->x = (s->a.x + s->b.x) / 2;
    middle->y = (s->a.y + s->b.y) / 2;
}

void proj_on_line(const segment_t *seg, const point_t *pointToProj, point_t *proj)
{
    SYSTEM_ASSERT(seg != NULL);
    SYSTEM_ASSERT(pointToProj != NULL);
    SYSTEM_ASSERT(proj != NULL);
    distance_t a1, b1, a2, b2;

    // On traite le cas général d'abord
    if (!distances_equal(seg->a.x, seg->b.x) && !distances_equal(seg->a.y, seg->b.y))
    {
        // Calcul de l'équation de la droite du segment
        a1 = (seg->a.y - seg->b.y) / (seg->a.x - seg->b.x);
        b1 = seg->a.y - a1 * seg->a.x;
        // Calcul de l'équation de la droite orthogonale passant par le point à projeter
        a2 = -1.0f / a1;
        b2 = pointToProj->y - a2 * pointToProj->x;

        proj->x = (b2 - b1) / (a1 - a2);
        proj->y = a1 * proj->x + b1;
    }
    else if (!distances_equal(seg->a.x, seg->b.x))
    { // seg->a.y == seg->b.y Gestion du cas particulier où l'équation de la droite du segment est
        // du type y = c
        proj->x = pointToProj->x;
        proj->y = seg->a.y;
    }
    else
    { // seg->a.x == seg->b.x Gestion du cas particulier où l'équation de la droite du segment est du type x = c
        proj->x = seg->a.x;
        proj->y = pointToProj->y;
    }
}

void proj_strictly_behind_line(const segment_t *seg, const point_t *pointToProj, point_t *proj)
{
    SYSTEM_ASSERT(seg != NULL);
    SYSTEM_ASSERT(pointToProj != NULL);
    SYSTEM_ASSERT(proj != NULL);
    // Utilisation de la fonction factorisée pour obtenir la projection
    point_t temp_proj;
    proj_on_line(seg, pointToProj, &temp_proj);

    // Vérification que le point projeté est strictement derrière le segment
    vector_2d_distance_t seg_vector = {.x = seg->b.x - seg->a.x, .y = seg->b.y - seg->a.y};
    vector_2d_distance_t proj_vector = {.x = temp_proj.x - seg->a.x, .y = temp_proj.y - seg->a.y};

    if ((seg_vector.x * proj_vector.x + seg_vector.y * proj_vector.y) > 0)
    {
        // Si le point projeté est devant ou sur le segment, on le déplace strictement derrière
        proj->x = seg->a.x - proj_vector.x;
        proj->y = seg->a.y - proj_vector.y;
    }
    else
    {
        *proj = temp_proj;
    }
}

bool segments_colinear(const segment_t *seg1, const segment_t *seg2)
{
    SYSTEM_ASSERT(seg1 != NULL);
    SYSTEM_ASSERT(seg2 != NULL);
    // Vérification si les segments sont parallèles
    if (!segments_parallel(seg1, seg2))
    {
        return false;
    }

    // Vérification si un point de seg2 est sur la droite formée par seg1
    vector_2d_distance_t v1 = {seg1->b.x - seg1->a.x, seg1->b.y - seg1->a.y};
    vector_2d_distance_t v2 = {seg2->a.x - seg1->a.x, seg2->a.y - seg1->a.y};
    float cross_product = v1.x * v2.y - v1.y * v2.x;

    return (distance_null(cross_product));
}

/* ***************************************** Public callback functions definitions *************************************** */
