/**
 * @file maths.c
 * @brief Regroupement de toutes les fonctions mathématiques utiles
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see maths.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "maths.h"
#include "angle/angle.h"
#include "axis/axis.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include <stdint.h>
#include <stdio.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

uint64_t factorielle(uint32_t nb)
{
    if (nb == 0)
    {
        return 1;
    }
    return nb * factorielle(nb - 1);
}

/*
 * Cette fonction cherche la régression linéaire par la méthode des moindres carrés (i.e. courbe de tendance du nuage de
 * point obtenu) ressource utile : http://serge.mehl.free.fr/anx/meth_carr.html axis axe où les points doivent avoir environ
 * la même valeur (dans le cas d'un scan latéral, c'est l'axe perpendiculaire à la trajectoire)
 */
bool linear_regression(angle_t *angle,
                       distance_t *offset,
                       int8_t *quality,
                       const point_t *points,
                       int32_t n,
                       axis_t axis)
{
    SYSTEM_ASSERT(angle != NULL);
    SYSTEM_ASSERT(offset != NULL);
    SYSTEM_ASSERT(quality != NULL);
    SYSTEM_ASSERT(points != NULL || n == 0);
    SYSTEM_ASSERT(n >= 0);
    float a = 0;
    float b = 0;
    distance_t x;
    distance_t y;
    float ymoy;
    float xmoy;
    float sum_xy = 0;
    float sum_x = 0;
    float sum_y = 0;
    float sum_xx = 0;
    float denum;
    float num;
    int32_t i;
    bool ret;

    for (i = 0; i < n; i++)
    {
#ifdef VERBOSE_MODE
        printf("p%4d;%4d\n", points[i].x, points[i].y);
#endif
        x = points[i].x;
        y = points[i].y;
        sum_xy += x * y;
        sum_x += x;
        sum_y += y;
        sum_xx += x * x;
    }
    denum = ((float)n * sum_xx - (sum_x * sum_x));
    num = (float)n * sum_xy - sum_x * sum_y;

    ret = true;
    if (fabsf(denum) > 0.0001f)
    {
        a = (float)num / (float)denum;
    }
    else
    {
        ret = false; // mission impossible, cela n'arrivera jamais...
    }
    if (n)
    {
        ymoy = sum_y / (float)n;
        xmoy = sum_x / (float)n;
        b = (float)ymoy - a * (float)xmoy;

        float sum, sx2, sy2;
        sum = 0;
        sx2 = 0;
        sy2 = 0;

        for (i = 0; i < n; i++)
        {
            sum += ((float)n * points[i].x - sum_x) * ((float)n * points[i].y - sum_y);
            sx2 += ((float)n * points[i].x - sum_x) * ((float)n * points[i].x - sum_x);
            sy2 += ((float)n * points[i].y - sum_y) * ((float)n * points[i].y - sum_y);
        }
        sx2 /= ((float)n * (float)n * (float)n);
        sy2 /= ((float)n * (float)n * (float)n);
        float sx, sy;
        sx = sqrtf(sx2);
        sy = sqrtf(sy2);
        float dquality;
        float cov;
        cov = sum / ((float)n * (float)n * (float)n); //covariance xy
        if (sx != 0.f && sy != 0.f)
        {
            dquality = 100 * cov / (sx * sy); //pour avoir un pourcentage signé
            *quality = (int8_t)
                dquality; //qualité de la régression : https://math.unice.fr/~diener/MAB07/MCO.pdf
            printf("quality %d\n", *quality);
        }

        float zi;
        sum = 0;
        if (axis == AXIS_X)
        {
            for (i = 0; i < n; i++)
            {
                zi = a * points[i].x + b;
                sum += absolute(zi - points[i].y); // somme des écarts à la courbe de tendance
            }
        }
        else
        {
            float c, d;
            c = 1 / a;
            d = -b / a;
            for (i = 0; i < n; i++)
            {
                zi = c * points[i].y + d;
                sum += absolute(zi - points[i].x);
            }
        }
        float q = sum / (float)n; // moyenne des écarts à la courbe de tendance
        if (q > 127)
        {
            *quality = 127;
        }
        else if (q < -128)
        {
            *quality = -128;
        }
        else
        {
            *quality = (int8_t)q;
        }
    }
    else
    {
        *quality = 0;
    }

    *angle = atanf(a);
    *offset = b;

    return ret;
}

void filter_future_time(int32_t values[],
                        uint16_t nb_value,
                        const float factor[],
                        uint8_t nb_factor)
{
    SYSTEM_ASSERT(values != NULL || nb_value == 0);
    SYSTEM_ASSERT(factor != NULL || nb_factor == 0);
    uint16_t i;
    uint8_t y;
    float new_value;

    if (nb_factor < 1)
    {
        return;
    }

    for (i = 0; i < nb_value; i++)
    {
        new_value = factor[0] * (float)values[i];

        for (y = 1; y < nb_factor; y++)
        {
            if (i + y < nb_value)
            {
                new_value += factor[y] * (float)values[i + y];
            }
        }

        values[i] = (int32_t)(new_value);
    }
}

void filter_past_time(int32_t values[], uint16_t nb_value, const float factor[], uint8_t nb_factor)
{
    SYSTEM_ASSERT(values != NULL || nb_value == 0);
    SYSTEM_ASSERT(factor != NULL || nb_factor == 0);
    int16_t i;
    uint8_t y;
    float new_value;

    if (nb_factor < 1)
    {
        return;
    }

    for (i = (int16_t)(nb_value - 1); i >= 0; i--)
    {
        new_value = factor[0] * (float)values[i];

        for (y = 1; y < nb_factor; y++)
        {
            if (i - y >= 0)
            {
                new_value += factor[y] * (float)values[i - y];
            }
        }

        values[i] = (int32_t)(new_value);
    }
}

uint16_t searchMin(const int32_t values[], uint16_t nbValue)
{
    SYSTEM_ASSERT(values != NULL);
    SYSTEM_ASSERT(nbValue > 0);
    uint8_t i, indiceMin = 0;
    for (i = 0; i < nbValue; i++)
    {
        if (values[i] < values[indiceMin])
        {
            indiceMin = i;
        }
    }
    return indiceMin;
}

uint16_t searchMax(const int32_t values[], uint16_t nbValue)
{
    SYSTEM_ASSERT(values != NULL);
    SYSTEM_ASSERT(nbValue > 0);
    uint8_t i, indiceMax = 0;
    for (i = 0; i < nbValue; i++)
    {
        if (values[i] < values[indiceMax])
        {
            indiceMax = i;
        }
    }
    return indiceMax;
}

/* ***************************************** Public callback functions definitions *************************************** */
