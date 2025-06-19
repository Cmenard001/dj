/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : QS_maths.h
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions mathématiques utiles
 *  Auteur : Arnaud
 *  Version 20130518
 */

#include "QS_maths.h"
#include <stdio.h>


uint64_t factorielle(uint32_t nb)
{
    if (nb == 0)
    {
        return 1;
    }
    return nb * factorielle(nb - 1);
}

int16_t GEOMETRY_viewing_angle(int16_t start_x, int16_t start_y, int16_t destination_x, int16_t destination_y)
{
    return (int16_t)((float)atan2f(destination_y - start_y, destination_x - start_x) * 4096);
}

void GEOMETRY_calculate_point_from_distance_and_angle(GEOMETRY_point_t start_point,
                                                      int16_t angle,
                                                      int16_t dist,
                                                      GEOMETRY_point_t *point)
{
    int16_t cos, sin;
    COS_SIN_4096_get(angle, &cos, &sin);
    point->x = (((int32_t)cos) * dist) / 4096 + start_point.x;
    point->y = (((int32_t)sin) * dist) / 4096 + start_point.y;
}

/* calcul de la distance que l'on a à parcourir entre la position actuelle et la position finale ( en mm ) OK
et calcul de l'angle */
int32_t GEOMETRY_viewing_algebric_distance(
    int32_t start_x, int32_t start_y, int32_t destination_x, int32_t destination_y, int32_t angle_de_vue)
{
    int32_t deltaX, deltaY;
    deltaX = absolute(destination_x - start_x);
    deltaY = absolute(destination_y - start_y);
    // debug_printf("deltaX %ld, deltaY %ld sqrt %ld", deltaX, deltaY, (int32_t)sqrt( deltaX*deltaX + deltaY*deltaY ));

    if (absolute(angle_de_vue) > PI4096 / 2)
    {
        return -(int32_t)sqrt(deltaX * deltaX + deltaY * deltaY);
    }
    else
    {
        return (int32_t)sqrt(deltaX * deltaX + deltaY * deltaY);
    }
    // en mm
}

/* calcul de la distance que l'on a à parcourir entre la position actuelle et la position finale ( en mm.4096 ) OK
et calcul de l'angle, valeur d'entrée en [mm.16] et [rad.4096]*/
int32_t GEOMETRY_viewing_algebric_distance_mm16(
    int32_t start_x, int32_t start_y, int32_t destination_x, int32_t destination_y, int32_t angle_de_vue)
{
    int32_t deltaX, deltaY;
    deltaX = absolute(destination_x - start_x);
    deltaY = absolute(destination_y - start_y);

    if (absolute(angle_de_vue) > PI4096 / 2)
    {
        return -(float)sqrt(deltaX * deltaX + deltaY * deltaY) * 256;
    }
    else
    {
        return (float)sqrt(deltaX * deltaX + deltaY * deltaY) * 256;
    }
    // en mm
}

/*
 * Cette fonction cherche la régression linéaire par la méthode des moindres carrés (i.e. courbe de tendance du nuage de
 * point obtenu) ressource utile : http://serge.mehl.free.fr/anx/meth_carr.html axis axe où les points doivent avoir environ
 * la même valeur (dans le cas d'un scan latéral, c'est l'axe perpendiculaire à la trajectoire)
 */
bool GEOMETRY_linear_regression(
    int16_t *angle, int16_t *offset, int8_t *quality, GEOMETRY_point_t *points, int32_t n, axis_e axis)
{
    float a = 0;
    float b = 0;
    int32_t x;
    int32_t y;
    int32_t ymoy;
    int32_t xmoy;
    int32_t sum_xy = 0;
    int32_t sum_x = 0;
    int32_t sum_y = 0;
    int32_t sum_xx = 0;
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
    denum = (n)*sum_xx - (sum_x * sum_x);
    num = n * sum_xy - sum_x * sum_y;

    ret = true;
    if (denum)
    {
        a = num / denum;
    }
    else
    {
        ret = false; // mission impossible, cela n'arrivera jamais...
    }
    if (n)
    {
        ymoy = sum_y / n;
        xmoy = sum_x / n;
        b = ymoy - a * xmoy;

        /*	int32_t sum, sx2, sy2;
            sum = 0;
            sx2 = 0;
            sy2 = 0;

            for(i = 0; i<n; i++)
            {
                sum += (n*points[i].x - sum_x)*(n*points[i].y - sum_y);
                sx2 += (n*points[i].x - sum_x)*(n*points[i].x - sum_x);
                sy2 += (n*points[i].y - sum_y)*(n*points[i].y - sum_y);
            }
            sx2 /= (n*n*n);
            sy2 /= (n*n*n);
            float sx, sy;
            sx = sqrt(sx2);
            sy = sqrt(sy2);
            float dquality;
            float cov;
            cov = sum / (n*n*n);	//covariance xy
            if(sx!=0. && sy!=0.)
            {
                dquality = 100*cov/(sx*sy);	//pour avoir un pourcentage signé
                *quality = (int8_t)dquality;	//qualité de la régression : https://math.unice.fr/~diener/MAB07/MCO.pdf
                printf("quality %d\n",*quality);
            }*/

        float zi;
        float sum;
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
        float q = sum / n; // moyenne des écarts à la courbe de tendance
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

    *angle = (int32_t)(atan(a) * 4096);
    *offset = (int16_t)b;

    return ret;
}

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
#if I_AM_CARTE_STRAT
void GEOMETRY_where_to_go_to_take(int32_t *x_consigne,
                                  int32_t *y_consigne,
                                  int32_t buoy_x,
                                  int32_t buoy_y,
                                  int32_t x_emplacement,
                                  int32_t y_emplacement,
                                  int32_t push_element_over,
                                  int32_t *x_act_event,
                                  int32_t *y_act_event,
                                  int32_t dist_act_event)
{
    // Distance entre la bouées qu'on vise et le centre du robot
    float x_distance_squared = (float)powf((global.pos.x - buoy_x), 2);
    float y_distance_squared = (float)powf((global.pos.y - buoy_y), 2);
    float distance_from_buoy = sqrtf(x_distance_squared + y_distance_squared);

    // Angle à adopter par le robot pour viser la bouée
    float theta = asinf((float)x_emplacement / distance_from_buoy);
    theta += atan2f((float)(buoy_y - global.pos.y), (float)(buoy_x - global.pos.x));

    // Distance parcourue par le robot
    int32_t distance_to_travel = sqrtf((float)powf(distance_from_buoy, 2) - (float)powf(x_emplacement, 2));
    distance_to_travel -= absolute(y_emplacement);
    float x = (cosf(theta) * (float)(distance_to_travel + push_element_over)) + global.pos.x;
    float y = (sinf(theta) * (float)(distance_to_travel + push_element_over)) + global.pos.y;
    *x_consigne = (int32_t)x;
    *y_consigne = (int32_t)y;

    if (x_act_event != NULL && y_act_event != NULL)
    {
        *x_act_event = (int32_t)((cosf(theta) * (float)(distance_to_travel + dist_act_event)) + global.pos.x);
        *y_act_event = (int32_t)((sinf(theta) * (float)(distance_to_travel + dist_act_event)) + global.pos.y);
    }
}
#endif

void filter_future_time(int32_t values[], uint16_t nb_value, float factor[], uint8_t nb_factor)
{
    uint16_t i;
    uint8_t y;
    float new_value;

    if (nb_factor < 1)
    {
        return;
    }

    for (i = 0; i < nb_value; i++)
    {
        new_value = factor[0] * values[i];

        for (y = 1; y < nb_factor; y++)
        {
            if (i + y < nb_value)
            {
                new_value += factor[y] * values[i + y];
            }
        }

        values[i] = (int32_t)(new_value);
    }
}

void filter_past_time(int32_t values[], uint16_t nb_value, float factor[], uint8_t nb_factor)
{
    int16_t i;
    uint8_t y;
    float new_value;

    if (nb_factor < 1)
    {
        return;
    }

    for (i = nb_value - 1; i >= 0; i--)
    {
        new_value = factor[0] * values[i];

        for (y = 1; y < nb_factor; y++)
        {
            if (i - y >= 0)
            {
                new_value += factor[y] * values[i - y];
            }
        }

        values[i] = (int32_t)(new_value);
    }
}

uint16_t searchMin(int32_t values[], uint16_t nbValue)
{
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

uint16_t searchMax(int32_t values[], uint16_t nbValue)
{
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

float GEOMETRY_atof(char *s)
{
    float a = 0.0;
    int e = 0;
    int c;
    bool neg = false;
    if (s[0] == '-')
    {
        neg = true;
        s++;
    }
    while ((c = *s++) != '\0' && c >= '0' && c <= '9')
    {
        a = a * 10.0 + (c - '0');
    }
    if (c == '.')
    {
        while ((c = *s++) != '\0' && c >= '0' && c <= '9')
        {
            a = a * 10.0 + (c - '0');
            e = e - 1;
        }
    }
    if (c == 'e' || c == 'E')
    {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+')
        {
            c = *s++;
        }
        else if (c == '-')
        {
            c = *s++;
            sign = -1;
        }
        while (c >= '0' && c <= '9')
        {
            i = i * 10 + (c - '0');
            c = *s++;
        }
        e += i * sign;
    }
    while (e > 0)
    {
        a *= 10.0;
        e--;
    }
    while (e < 0)
    {
        a *= 0.1;
        e++;
    }
    return (neg ? -a : a);
}
