#include "QS_maths_segment.h"
#include "QS_maths_point.h"
#include "QS_maths_vector.h"

#include <math.h>
#include <stdint.h>

bool GEOMETRY_segments_get_intersection(GEOMETRY_segment_t s1, GEOMETRY_segment_t s2, GEOMETRY_point_t *intersection)
{
    /* On calcule les vecteur orthogonaux à s1 et s2 */

    GEOMETRY_vector_t u1 = {-(s1.b.y - s1.a.y), (s1.b.x - s1.a.x)};

    GEOMETRY_vector_t u2 = {-(s2.b.y - s2.a.y), (s2.b.x - s2.a.x)};

    /* on projète s1 et s2 sur u1 */

    int32_t proj_s1_a = ((int32_t)s1.a.x) * ((int32_t)u1.x) + ((int32_t)s1.a.y) * ((int32_t)u1.y);
    int32_t proj_s1_b = ((int32_t)s1.b.x) * ((int32_t)u1.x) + ((int32_t)s1.b.y) * ((int32_t)u1.y);
    int32_t proj_s2_a = ((int32_t)s2.a.x) * ((int32_t)u1.x) + ((int32_t)s2.a.y) * ((int32_t)u1.y);
    int32_t proj_s2_b = ((int32_t)s2.b.x) * ((int32_t)u1.x) + ((int32_t)s2.b.y) * ((int32_t)u1.y);

    /*
     * Si les projetés ne se chevauchent pas, les segments ne
     * se croisent pas
     */

    if (MIN(proj_s1_a, proj_s1_b) > MAX(proj_s2_a, proj_s2_b) || MAX(proj_s1_a, proj_s1_b) < MIN(proj_s2_a, proj_s2_b))
    {
        return false;
    }

    /* sinon on projète s1 et s2 sur u2 et on recommence*/

    proj_s1_a = ((int32_t)s1.a.x) * ((int32_t)u2.x) + ((int32_t)s1.a.y) * ((int32_t)u2.y);
    proj_s1_b = ((int32_t)s1.b.x) * ((int32_t)u2.x) + ((int32_t)s1.b.y) * ((int32_t)u2.y);
    proj_s2_a = ((int32_t)s2.a.x) * ((int32_t)u2.x) + ((int32_t)s2.a.y) * ((int32_t)u2.y);
    proj_s2_b = ((int32_t)s2.b.x) * ((int32_t)u2.x) + ((int32_t)s2.b.y) * ((int32_t)u2.y);

    /*
     * Si les projetés ne se chevauchent pas, les segments ne
     * se croisent pas
     */

    if (MIN(proj_s1_a, proj_s1_b) > MAX(proj_s2_a, proj_s2_b) || MAX(proj_s1_a, proj_s1_b) < MIN(proj_s2_a, proj_s2_b))
    {
        return false;
    }

    // Rajout d'une protection car deux segements sur une même ligne ne se coupent pas forcément
    if ((s1.a.x < MIN(s2.a.x, s2.b.x) && s1.b.x < MIN(s2.a.x, s2.b.x))
        || (s1.a.x > MAX(s2.a.x, s2.b.x) && s1.b.x > MAX(s2.a.x, s2.b.x))
        || (s1.a.y < MIN(s2.a.y, s2.b.y) && s1.b.y < MIN(s2.a.y, s2.b.y))
        || (s1.a.y > MAX(s2.a.y, s2.b.y) && s1.b.y > MAX(s2.a.y, s2.b.y)))
    {
        return false;
    }

    if (intersection == NULL)
    {
        return true; // On ne veut pas l'intersection
    }

    // Calcul de l'intersection
    int32_t det = (s1.b.x - s1.a.x) * (s2.b.y - s2.a.y) - (s1.b.y - s1.a.y) * (s2.b.x - s2.a.x);
    if (det == 0)
    {
        // Les segments sont parallèles ou colinéaires
        if (GEOMETRY_segments_colinear(s1, s2))
        {
            // Vérification si les segments colinéaires se chevauchent
            if (MAX(s1.a.x, s1.b.x) < MIN(s2.a.x, s2.b.x) || MAX(s2.a.x, s2.b.x) < MIN(s1.a.x, s1.b.x)
                || MAX(s1.a.y, s1.b.y) < MIN(s2.a.y, s2.b.y) || MAX(s2.a.y, s2.b.y) < MIN(s1.a.y, s1.b.y))
            {
                return false; // Pas de chevauchement
            }

            if (intersection != NULL)
            {
                // Retourne un point d'intersection arbitraire dans le chevauchement
                intersection->x = MAX(MIN(s1.a.x, s1.b.x), MIN(s2.a.x, s2.b.x));
                intersection->y = MAX(MIN(s1.a.y, s1.b.y), MIN(s2.a.y, s2.b.y));
            }
            return true;
        }
        return false; // Les segments sont parallèles mais non colinéaires
    }

    float t = ((s2.a.x - s1.a.x) * (s2.b.y - s2.a.y) - (s2.a.y - s1.a.y) * (s2.b.x - s2.a.x)) / (float)det;
    float u = ((s2.a.x - s1.a.x) * (s1.b.y - s1.a.y) - (s2.a.y - s1.a.y) * (s1.b.x - s1.a.x)) / (float)det;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        intersection->x = s1.a.x + t * (s1.b.x - s1.a.x);
        intersection->y = s1.a.y + t * (s1.b.y - s1.a.y);
        return true;
    }

    return false;
}

bool GEOMETRY_segments_intersects(GEOMETRY_segment_t s1, GEOMETRY_segment_t s2)
{
    return GEOMETRY_segments_get_intersection(s1, s2, NULL);
}

bool GEOMETRY_point_on_segment(GEOMETRY_segment_t seg, GEOMETRY_point_t tested_point)
{
    // On fait le produit vectoriel entre les vecteurs formés par les points du segment et le point
    // testé. Si le produit vectoriel est nul, le point est sur la droite formée par le segment.
    GEOMETRY_vector_t v1 = {seg.b.x - seg.a.x, seg.b.y - seg.a.y};
    GEOMETRY_vector_t v2 = {tested_point.x - seg.a.x, tested_point.y - seg.a.y};
    int32_t cross_product = v1.x * v2.y - v1.y * v2.x;
    if (cross_product != 0)
    {
        return false;
    }

    // Cas particulier où le segment est vertical
    if (seg.a.x == seg.b.x)
    {
        return (tested_point.x == seg.a.x)                   // Le point est sur la droite
               && (tested_point.y >= MIN(seg.a.y, seg.b.y))  // Le point est entre les deux extrémités du segment
               && (tested_point.y <= MAX(seg.a.y, seg.b.y)); // Le point est entre les deux extrémités du segment
    }

    // Cas particulier où le segment est horizontal
    if (seg.a.y == seg.b.y)
    {
        return (tested_point.y == seg.a.y)                   // Le point est sur la droite
               && (tested_point.x >= MIN(seg.a.x, seg.b.x))  // Le point est entre les deux extrémités du segment
               && (tested_point.x <= MAX(seg.a.x, seg.b.x)); // Le point est entre les deux extrémités du segment
    }

    // On vérifie ensuite que le point est bien entre les deux extrémités du segment.
    // Pour ça on vérifie que les coordonnées du point sont entre les coordonnées des extrémités du segment.
    return (tested_point.x >= MIN(seg.a.x, seg.b.x)) && (tested_point.x <= MAX(seg.a.x, seg.b.x))
           && (tested_point.y >= MIN(seg.a.y, seg.b.y)) && (tested_point.y <= MAX(seg.a.y, seg.b.y));
}

bool GEOMETRY_segments_parallel(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2)
{
    // Cas des segments verticaux
    if (seg1.a.x == seg1.b.x && seg2.a.x == seg2.b.x)
    {
        return true;
    }

    // Cas des segments horizontaux
    if (seg1.a.y == seg1.b.y && seg2.a.y == seg2.b.y)
    {
        return true;
    }

    // Vérification des pentes sans division pour éviter la division par zéro
    int64_t deltaX1 = seg1.b.x - seg1.a.x;
    int64_t deltaY1 = seg1.b.y - seg1.a.y;
    int64_t deltaX2 = seg2.b.x - seg2.a.x;
    int64_t deltaY2 = seg2.b.y - seg2.a.y;

    // Deux segments sont parallèles si deltaY1 * deltaX2 == deltaY2 * deltaX1
    if (deltaY1 * deltaX2 == deltaY2 * deltaX1)
    {
        return true;
    }

    return false;
}
bool GEOMETRY_segments_get_strictly_intersection(GEOMETRY_segment_t s1,
                                                 GEOMETRY_segment_t s2,
                                                 GEOMETRY_point_t *intersection)
{
    /* On vérifie d'abord si une des extrémités d'un segment est sur l'autre segment */
    if (GEOMETRY_point_on_segment(s1, s2.a) || GEOMETRY_point_on_segment(s1, s2.b) || GEOMETRY_point_on_segment(s2, s1.a)
        || GEOMETRY_point_on_segment(s2, s1.b))
    {
        return false;
    }

    /* On vérifie si les segments se croisent */
    return GEOMETRY_segments_get_intersection(s1, s2, intersection);
}

bool GEOMETRY_segments_strictly_intersects(GEOMETRY_segment_t s1, GEOMETRY_segment_t s2)
{
    return GEOMETRY_segments_get_strictly_intersection(s1, s2, NULL);
}

GEOMETRY_point_t GEOMETRY_segment_middle(GEOMETRY_segment_t s)
{
    return (GEOMETRY_point_t){(s.a.x + s.b.x) / 2, (s.a.y + s.b.y) / 2};
}

GEOMETRY_point_t GEOMETRY_proj_on_line(GEOMETRY_segment_t seg, GEOMETRY_point_t pointToProj)
{
    GEOMETRY_point_t proj;
    float a1, b1, a2, b2;

    // On traite le cas général d'abord
    if (seg.a.x != seg.b.x && seg.a.y != seg.b.y)
    {
        // Calcul de l'équation de la droite du segment
        a1 = (float)(seg.a.y - seg.b.y) / (float)(seg.a.x - seg.b.x);
        b1 = seg.a.y - a1 * seg.a.x;
        // Calcul de l'équation de la droite orthogonale passant par le point à projeter
        a2 = -1.0f / a1;
        b2 = pointToProj.y - a2 * pointToProj.x;

        proj.x = (b2 - b1) / (a1 - a2);
        proj.y = a1 * proj.x + b1;
    }
    else if (seg.a.x != seg.b.x)
    { // seg.a.y == seg.b.y Gestion du cas particulier où l'équation de la droite du segment est
        // du type y = c
        proj.x = pointToProj.x;
        proj.y = seg.a.y;
    }
    else
    { // seg.a.x == seg.b.x Gestion du cas particulier où l'équation de la droite du segment est du type x = c
        proj.x = seg.a.x;
        proj.y = pointToProj.y;
    }

    return proj;
}

GEOMETRY_point_t GEOMETRY_proj_strictly_behind_line(GEOMETRY_segment_t seg, GEOMETRY_point_t pointToProj)
{
    // Utilisation de la fonction factorisée pour obtenir la projection
    GEOMETRY_point_t proj = GEOMETRY_proj_on_line(seg, pointToProj);

    // Vérification que le point projeté est strictement derrière le segment
    GEOMETRY_vector_t seg_vector = {seg.b.x - seg.a.x, seg.b.y - seg.a.y};
    GEOMETRY_vector_t proj_vector = {proj.x - seg.a.x, proj.y - seg.a.y};

    if ((seg_vector.x * proj_vector.x + seg_vector.y * proj_vector.y) > 0)
    {
        // Si le point projeté est devant ou sur le segment, on le déplace strictement derrière
        proj.x = seg.a.x - proj_vector.x;
        proj.y = seg.a.y - proj_vector.y;
    }

    return proj;
}

bool GEOMETRY_segments_colinear(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2)
{
    // Vérification si les segments sont parallèles
    if (!GEOMETRY_segments_parallel(seg1, seg2))
    {
        return false;
    }

    // Vérification si un point de seg2 est sur la droite formée par seg1
    GEOMETRY_vector_t v1 = {seg1.b.x - seg1.a.x, seg1.b.y - seg1.a.y};
    GEOMETRY_vector_t v2 = {seg2.a.x - seg1.a.x, seg2.a.y - seg1.a.y};
    int32_t cross_product = v1.x * v2.y - v1.y * v2.x;

    return (cross_product == 0);
}
