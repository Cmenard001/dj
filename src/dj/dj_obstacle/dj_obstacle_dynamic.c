/**
 * @file dj_obstacle_dynamic.c
 * @brief Dynamic obstacles are obstacles that can move in a straight line with a constant acceleration
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_dynamic.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle_dynamic.h"
#include "../dj_config.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_logs/dj_logs.h"
#include "../dj_prop_config/dj_prop_config.h"
#include "dj_polygon.h"

#include <math.h>
#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Step of the brute force to find the solutions
 * @see dj_point_dynamic_get_position
 */
#define BRUTE_FORCE_STEP 0.1f // (s)

/* ************************************************ Private type definition ********************************************** */

/**
 * @brief Structure to store the solutions of the dynamic obstacle
 * @note The solutions are the positions of the center of the obstacle in space time from the viewer position
 * @see dj_point_dynamic_get_position
 */
typedef struct
{
    GEOMETRY_point_t m_solutions[4];
    uint8_t m_nb_solutions;
} dj_dynamic_point_solution_t;

/* ********************************************* Private functions declarations ****************************************** */

static bool check_solution_validity(GEOMETRY_point_t *solution,
                                    dj_viewer_status_t *viewer_status,
                                    bool rotate_from_nul_speed);
static void dj_point_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          dj_dynamic_point_solution_t *solution);
static void point_to_polygon_solution(dj_obstacle_dynamic_t *obstacle,
                                      GEOMETRY_point_t *point_solution,
                                      dj_polygon_t *polygon_solution);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Function to check if a solution is valid (if the prop would have chosen it)
 *
 * @param solution Position of the middle of the solution
 * @param viewer_status Status of the viewer
 * @param rotate_from_nul_speed true if it's an END_AT_LAST_POINT turn, false if it's an END_AT_BRAKE turn
 * @return true if the solution is valid, false otherwise
 */
static bool check_solution_validity(GEOMETRY_point_t *solution,
                                    dj_viewer_status_t *viewer_status,
                                    bool rotate_from_nul_speed)
{
    dj_control_non_null(viewer_status, false);
    dj_control_non_null(solution, false);

    // On vérifie que la solution est valide du point de vue de la prop
    // C'est à dire est-ce que la prop aurait choisi de passer par là

    // Calcul de l'angle entre la position du viewer et la solution

    int16_t viewer_angle = (int16_t)((float)atan2f(viewer_status->m_speed.y, viewer_status->m_speed.x) * 4096);
    int16_t solution_angle
        = (int16_t)((float)atan2f(solution->y - viewer_status->m_position.y, solution->x - viewer_status->m_position.x)
                    * 4096);
    int16_t angle_diff = solution_angle - viewer_angle;
    if (angle_diff < 0)
    {
        angle_diff = -angle_diff;
    }
    angle_diff = GEOMETRY_modulo_angle(angle_diff);

    if (rotate_from_nul_speed)
    {
        if (angle_diff > MIN_ANGLE_TO_DO_PRE_ROTATION)
        {
            return true;
        }
    }
    else
    {
        if (angle_diff <= MAX_ANGLE_TO_NOT_DO_PRE_ROTATION)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Function to get the position of the middle point of an dynamic obstacle in space time
 *
 * @param [in] obstacle dynamic obstacle to get the position
 * @param [in] viewer_status Status of the viewer
 * @param [out] solution Position of the middle of the obstacle
 * @return dj_dynamic_point_solution_t Position(s) of the obstacle (0 - 4 solutions)
 */
static void dj_point_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          dj_dynamic_point_solution_t *solution)
{
    /*
        Explication des maths qui vont suivre

        Ces maths peuvent être retrouvés dans le README principal de dj
        Ils seront un peu mieux mis en forme là-bas

        Rappel :
        Le but est de connaitre la position de l'obstacle dans l'espace temps depuis la position du robot.
        "Espace temps" : c'est le même principe que l'espace-temps dans l'espace mais à la vitesse du robot au lieu de la vitesse de la lumière.
        Donc le but est de connaître la ou les (ou aucune) position(s) de l'obstacle si on choisi le(s) meilleur(s) chemin(s) pour l'atteindre.
        On limite la position d'un obstacle à son centre.

        On pose :
            t = temps absolu
            tadvi = temps au quel l'obstacle a été mis à jour
            trob = temps au quel se déroule le début de l'observation

            Aadv(x, y) = accélération de l'obstacle
            Vadvi(x, y) = vitesse initiale de l'obstacle
            Padvi(x, y) = position initiale de l'obstacle

            Arob = accélération du robot
            Vrobi = vitesse initiale du robot
            Probi = position initiale du robot

        1) Evolution de la distance entre le robot et sa position initiale en fonction du temps
            Drob = Vrobi * (t - trob) + 1/2 * Arob * (t - trob)^2

        2) Evolution de la position de l'obstacle en fonction du temps
            Padvx = Padvix + Vadvix * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2
            Padvy = Padviy + Vadviy * (t - tadv) + 1/2 * Aadvy * (t - tadv)^2

        3) Evolution de la distance entre le robot et l'obstacle en fonction du temps
            Dadv = sqrt((dx)^2 + (dy)^2)
        Avec dx et dy la difference de position entre le robot et l'obstacle
        Or,
            dx = Padvx - Probix
            dy = Padvy - Probiy
        Donc,
            Dadv = sqrt((Padvx - Probix)^2 + (Padvy - Probiy)^2)
        <=> Dadv = sqrt((Padvix + Vadvix * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probix)^2 + (Padviy + Vadviy * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probiy)^2)

        4) Solutions
        Les solutions apparaissent lorsque Dadv = Drob
        Donc lorsque :
            sqrt((Padvix + Vadvix * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probix)^2 + (Padviy + Vadviy * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probiy)^2) = Vrobi * (t - trob) + 1/2 * Arob * (t - trob)^2
        <=> (Padvix + Vadvix * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probix)^2 + (Padviy + Vadviy * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probiy)^2 = (Vrobi * (t - trob) + 1/2 * Arob * (t - trob)^2)^2
        <=> 0 = (Padvix + Vadvix * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probix)^2 + (Padviy + Vadviy * (t - tadv) + 1/2 * Aadvx * (t - tadv)^2 - Probiy)^2 - (Vrobi * (t - trob) + 1/2 * Arob * (t - trob)^2)^2
        <=> 0 = (Aadvx^2/2 - Arob^2/4)*t^4 + (Aadvx*(Vadvix - Aadvx*tadv) + Aadvx*(Vadviy - Aadvx*tadv) - Arob*(Vrobi - Arob*trob))*t^3 + ((Vadvix - Aadvx*tadv)^2 + (Vadviy - Aadvx*tadv)^2 - (Vrobi - Arob*trob)^2 + Aadvx*((Aadvx*tadv^2)/2 - Vadvix*tadv + Padvix - Probix) + Aadvx*((Aadvx*tadv^2)/2 - Vadviy*tadv + Padviy - Probiy) + Arob*(Vrobi*trob - (Arob*trob^2)/2))*t^2 + (2*(Vadvix - Aadvx*tadv)*((Aadvx*tadv^2)/2 - Vadvix*tadv + Padvix - Probix) + 2*(Vadviy - Aadvx*tadv)*((Aadvx*tadv^2)/2 - Vadviy*tadv + Padviy - Probiy) + 2*(Vrobi - Arob*trob)*(Vrobi*trob - (Arob*trob^2)/2))*t + ((Aadvx*tadv^2)/2 - Vadvix*tadv + Padvix - Probix)^2 + ((Aadvx*tadv^2)/2 - Vadviy*tadv + Padviy - Probiy)^2 - (Vrobi*trob - (Arob*trob^2)/2)^2
        L'équation est un peu longue mais résumons-la :
        Elle est de la forme
            0 = a*t4 + b*t^3 + c*t^2 + d*t + e
        avec
            a = (Aadvx^2/2 - Arob^2/4)
            b = (Aadvx*(Vadvix - Aadvx*tadv) + Aadvx*(Vadviy - Aadvx*tadv) - Arob*(Vrobi - Arob*trob))
            c = ((Vadvix - Aadvx*tadv)^2 + (Vadviy - Aadvx*tadv)^2 - (Vrobi - Arob*trob)^2 + Aadvx*((Aadvx*tadv^2)/2 - Vadvix*tadv + Padvix - Probix) + Aadvx*((Aadvx*tadv^2)/2 - Vadviy*tadv + Padviy - Probiy) + Arob*(Vrobi*trob - (Arob*trob^2)/2))
            d = (2*(Vadvix - Aadvx*tadv)*((Aadvx*tadv^2)/2 - Vadvix*tadv + Padvix - Probix) + 2*(Vadviy - Aadvx*tadv)*((Aadvx*tadv^2)/2 - Vadviy*tadv + Padviy - Probiy) + 2*(Vrobi - Arob*trob)*(Vrobi*trob - (Arob*trob^2)/2))
            e = ((Aadvx*tadv^2)/2 - Vadvix*tadv + Padvix - Probix)^2 + ((Aadvx*tadv^2)/2 - Vadviy*tadv + Padviy - Probiy)^2 - (Vrobi*trob - (Arob*trob^2)/2)^2


        Cette équation est une équation du quatrième degré, donc il y a de 0 à 4 solutions.
        Les solutions générales sont complexes, donc on ne calculera pas la forme générale
        Pour rappel on cherche les racines du polynome a*t^4 + b*t^3 + c*t^2 + d*t + e
        On a l'avantage de pouvoir limiter les solutions à un intervalle de temps [0, tmax]
        En effet il n'a aucun sens de chercher une solution négative ou supérieure à tmax
        De plus, tmax est relativement petit car on ne cherche pas à prédire la position de l'obstacle dans un futur lointain (imprécision de la prédiction)
        Et de tout façon, n'importe quel robot a traversé le terrain ou changé de direction en quelques secondes seulement

        On va donc pour tout t dans [0, tmax] calculer la valeur de l'équation et regarder si elle est de signe opposé à la précédente
        Si c'est le cas, alors il y a une solution entre t et t-1

        Pour chaque solution, on va calculer la position de l'obstacle à ce temps:
            Padvx = Padvix + Vadvix * t + 1/2 * Aadvx * t^2
            Padvy = Padviy + Vadviy * t + 1/2 * Aadvx * t^2
        Avec t la solution trouvée

        Et voilà, on a trouvé aucune/la/les position(s) de l'obstacle dans l'espace temps :)
    */

    float start_time = viewer_status->m_time * 0.001f;
    float end_time = (obstacle->m_update_time + OBSTACLE_LIFETIME_MS) * 0.001f;
    // Si end_time est inférieur à start_time, alors on a dépassé le temps max
    // Il est inutile de continuer, l'obstacle est périmé
    // En effet il est inconcevable de prédire la position d'un obstacle autant dans le futur
    if (end_time < start_time)
    {
        solution->m_nb_solutions = 0;
        return;
    }

    // Conversion des variables en unités SI (s, m, m/s, m/s²)

    float tadvi = obstacle->m_update_time * 0.001f;
    float trob = viewer_status->m_time * 0.001f;

    float Aadvx = obstacle->m_acceleration.x * 0.001f;
    float Aadvy = obstacle->m_acceleration.y * 0.001f;
    float Vadviy = obstacle->m_initial_speed.y * 0.001f;
    float Vadvix = obstacle->m_initial_speed.x * 0.001f;
    float Padviy = obstacle->m_initial_position.y * 0.001f;
    float Padvix = obstacle->m_initial_position.x * 0.001f;

    float Arob = PROP_LINEAR_ACCELERATION * 0.001f;
    float Vrobi = sqrt(SQUARE(viewer_status->m_speed.x) + SQUARE(viewer_status->m_speed.y)) * 0.001f;
    float Probix = viewer_status->m_position.x * 0.001f;
    float Probiy = viewer_status->m_position.y * 0.001f;

    float l_a = (SQUARE(Aadvx) / 2 - SQUARE(Arob) / 4);
    float l_b = (Aadvx * (Vadvix - Aadvx * tadvi) + Aadvx * (Vadviy - Aadvx * tadvi) - Arob * (Vrobi - Arob * trob));
    float l_c = (SQUARE(Vadvix - Aadvx * tadvi) + SQUARE(Vadviy - Aadvx * tadvi) - SQUARE(Vrobi - Arob * trob)
                 + Aadvx * (SQUARE(Aadvx * tadvi) / 2 - Vadvix * tadvi + Padvix - Probix)
                 + Aadvx * (SQUARE(Aadvx * tadvi) / 2 - Vadviy * tadvi + Padviy - Probiy)
                 + Arob * (Vrobi * trob - SQUARE(Arob * trob) / 2));
    float l_d = (2 * (Vadvix - Aadvx * tadvi) * (SQUARE(Aadvx * tadvi) / 2 - Vadvix * tadvi + Padvix - Probix)
                 + 2 * (Vadviy - Aadvx * tadvi) * (SQUARE(Aadvx * tadvi) / 2 - Vadviy * tadvi + Padviy - Probiy)
                 + 2 * (Vrobi - Arob * trob) * (Vrobi * trob - SQUARE(Arob * trob) / 2));
    float l_e = (SQUARE(Aadvx * tadvi) / 2 - Vadvix * tadvi + Padvix - Probix)
                + (SQUARE(Aadvx * tadvi) / 2 - Vadviy * tadvi + Padviy - Probiy) - SQUARE(Vrobi * trob - Arob * trob);

    solution->m_nb_solutions = 0;
    // start_sign = true si le signe de l'équation est positif
    // On commence à t = 0, donc le signe dépend uniquement de e
    bool sign = (l_e > 0) ? true : false;
    for (float t = start_time; t < end_time; t += BRUTE_FORCE_STEP)
    {
        bool new_sign = (l_a * t * t * t * t + l_b * t * t * t + l_c * t * t + l_d * t + l_e > 0) ? true : false;
        if (new_sign != sign)
        {
            // On a trouvé une solution
            sign = new_sign;
            // Calcul de la position de l'obstacle à ce temps
            // (On prend soin de re-convertir les unités en mm)
            solution->m_solutions[solution->m_nb_solutions]
                = (GEOMETRY_point_t){.x = (Padvix + Vadvix * t + Aadvx * SQUARE(t) / 2) * 1000,
                                     .y = (Padviy + Vadviy * t + Aadvy * SQUARE(t) / 2) * 1000};
            if (solution->m_nb_solutions >= 3)
            {
                // On a trouvé toutes les solutions
                break;
            }
            solution->m_nb_solutions++;
        }
    }
}

/**
 * @brief Function to get the position of an artificial static obstacle from his shape and his position
 *
 * @param [in] obstacle Pointer on initial dynamic obstacle
 * @param [in] point_solution Solution of artificial static obstacle (as a point)
 * @param [out] polygon_solution Solution of artificial static obstacle (as a polygon)
 */
static void point_to_polygon_solution(dj_obstacle_dynamic_t *obstacle,
                                      GEOMETRY_point_t *point_solution,
                                      dj_polygon_t *polygon_solution)
{
    dj_control_non_null(obstacle, );
    dj_control_non_null(point_solution, );
    dj_control_non_null(polygon_solution, );
    polygon_solution->nb_points = obstacle->m_initial_shape.nb_points;
    for (uint8_t i = 0; i < obstacle->m_initial_shape.nb_points; i++)
    {
        polygon_solution->points[i] = (GEOMETRY_point_t){.x = point_solution->x + obstacle->m_initial_shape.points[i].x,
                                                         .y = point_solution->y + obstacle->m_initial_shape.points[i].y};
    }
}

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_dynamic_init(dj_obstacle_dynamic_t *obstacle,
                              dj_polygon_t *initial_shape,
                              GEOMETRY_point_t *initial_position,
                              GEOMETRY_vector_t *initial_speed,
                              GEOMETRY_vector_t *acceleration,
                              dj_dynamic_obstacle_id_e id,
                              bool is_enabled)
{
    dj_control_non_null(obstacle, );
    dj_control_non_null(initial_shape, );
    dj_control_non_null(initial_position, );
    dj_control_non_null(initial_speed, );
    dj_control_non_null(acceleration, );

    dj_obstacle_init((dj_obstacle_t *)obstacle, DJ_OBSTACLE_TYPE_DYNAMIC, is_enabled);
    obstacle->m_id = id;
    obstacle->m_initial_position = *initial_position;
    obstacle->m_initial_speed = *initial_speed;
    obstacle->m_acceleration = *acceleration;
    obstacle->m_update_time = 0;
    obstacle->m_initial_shape.nb_points = dj_polygon_get_nb_points(initial_shape);
    for (uint8_t i = 0; i < dj_polygon_get_nb_points(initial_shape); i++)
    {
        obstacle->m_initial_shape.points[i] = *dj_polygon_get_point(initial_shape, i);
    }
}

void dj_obstacle_dynamic_deinit(dj_obstacle_dynamic_t *obstacle)
{
    dj_control_non_null(obstacle, );
    // Call the parent deinit
    dj_obstacle_deinit((dj_obstacle_t *)obstacle);
}

void dj_obstacle_dynamic_refresh(dj_obstacle_dynamic_t *obstacle,
                                 GEOMETRY_vector_t *acceleration,
                                 GEOMETRY_vector_t *initial_speed,
                                 GEOMETRY_point_t *initial_position)
{
    dj_control_non_null(obstacle, );
    dj_control_non_null(acceleration, );
    dj_control_non_null(initial_speed, );
    dj_control_non_null(initial_position, );
    obstacle->m_acceleration = *acceleration;
    obstacle->m_initial_speed = *initial_speed;
    obstacle->m_initial_position = *initial_position;
    dj_dep_get_time(&obstacle->m_update_time);
}

void dj_obstacle_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                      dj_viewer_status_t *viewer_status,
                                      dj_dynamic_polygon_solution_t *solution)
{
    dj_control_non_null(obstacle, );
    dj_control_non_null(viewer_status, );
    dj_control_non_null(solution, );

    // Si la vitesse et l'accélération sont nulles alors la et l'unique solution est la position initiale du polygone.
    // C'est plus un workaround qu'autre chose car les maths qui sont sensées faire ça ne semble pas fonctionner correctement.
    // C'est pas très grave car à ce jour (12/05/2025), les obstacles dynamiques n'ont pas de vitesse ni d'accélération.
    if (obstacle->m_initial_speed.x == 0 && obstacle->m_initial_speed.y == 0 && obstacle->m_acceleration.x == 0
        && obstacle->m_acceleration.y == 0)
    {
        point_to_polygon_solution(obstacle, &obstacle->m_initial_position, &solution->m_solutions[0]);
        solution->m_nb_solutions = 1;
        return;
    }

    // On commence par chercher les solutions avec une vitesse nulle puis avec la vitesse du viewer
    dj_dynamic_point_solution_t found_points_from_nul_speed;
    dj_point_dynamic_get_position(obstacle, viewer_status, &found_points_from_nul_speed);
    dj_dynamic_point_solution_t found_points_from_viewer_speed;
    dj_point_dynamic_get_position(obstacle, viewer_status, &found_points_from_viewer_speed);

    // On va maintenant filtrer les solutions pour ne garder que celles qui sont valides
    // Pour les solutions avec une vitesse nulle
    solution->m_nb_solutions = 0;
    for (uint8_t i = 0; i < found_points_from_nul_speed.m_nb_solutions; i++)
    {
        if (check_solution_validity(&found_points_from_nul_speed.m_solutions[i], viewer_status, true))
        {
            point_to_polygon_solution(
                obstacle, &found_points_from_nul_speed.m_solutions[i], &solution->m_solutions[solution->m_nb_solutions]);
            solution->m_nb_solutions++;
        }
    }
    for (uint8_t i = 0; i < found_points_from_viewer_speed.m_nb_solutions; i++)
    {
        if (check_solution_validity(&found_points_from_viewer_speed.m_solutions[i], viewer_status, false))
        {
            point_to_polygon_solution(
                obstacle, &found_points_from_viewer_speed.m_solutions[i], &solution->m_solutions[solution->m_nb_solutions]);
            solution->m_nb_solutions++;
        }
    }
}

void dj_dynamic_polygon_solution_deinit(dj_dynamic_polygon_solution_t *solution)
{
}

/* ******************************************* Public callback functions declarations ************************************ */
