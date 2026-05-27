/**
 * @file pf_calculator_dynamic_obstacles.c
 * @brief Source file for the dynamic obstacles calculator module.
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see pf_calculator_dynamic_obstacles.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "pf_calculator_dynamic_obstacles.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include <math.h>
#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Step of the brute force to find the solutions [s]
 */
#define BRUTE_FORCE_STEP     0.1f

/**
 * @brief Max angle to trigger a pre-rotation stop [rad]
 * @note Corresponds to CPLT_TRAJ_MAX_ANGLE_DIFF_FOR_CONTINUOUS_TRAJ_RAD - 0.5 in the full system
 */
#define MIN_ANGLE_TO_DO_PRE_ROTATION     (DJ_MAX_ANGLE_FOR_CONTINUOUS_TRAJ_RAD - 0.5f)

/**
 * @brief Max angle below which no pre-rotation is needed [rad]
 * @note Corresponds to CPLT_TRAJ_MAX_ANGLE_DIFF_FOR_CONTINUOUS_TRAJ_RAD + 0.5 in the full system
 */
#define MAX_ANGLE_TO_NOT_DO_PRE_ROTATION (DJ_MAX_ANGLE_FOR_CONTINUOUS_TRAJ_RAD + 0.5f)

/**
 * @brief Max angle difference for continuous trajectory (no stop required) [rad]
 */
#define DJ_MAX_ANGLE_FOR_CONTINUOUS_TRAJ_RAD 1.2f

/**
 * @brief Lifetime of an obstacle [ms]
 */
#define OBSTACLE_LIFETIME_MS 5000

/* ************************************************ Private type definition ********************************************** */

/**
 * @brief Up to 4 center-point solutions for a dynamic obstacle in space-time
 */
typedef struct
{
    point_t solutions[4];
    uint8_t nb_solutions;
} dj_dynamic_point_solution_t;

/* ********************************************* Private functions declarations ****************************************** */

static bool check_solution_validity(point_t *solution,
                                    dj_viewer_status_t *viewer_status,
                                    bool rotate_from_nul_speed);

static void dj_point_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          dj_dynamic_point_solution_t *solution,
                                          pf_obstacle_dynamic_get_position_args_t *args);

static void point_to_polygon_solution(dj_obstacle_dynamic_t *obstacle,
                                      point_t *point_solution,
                                      dj_polygon_t *polygon_solution);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

static bool check_solution_validity(point_t *solution,
                                    dj_viewer_status_t *viewer_status,
                                    bool rotate_from_nul_speed)
{
    SYSTEM_ASSERT(viewer_status != NULL);
    SYSTEM_ASSERT(solution != NULL);

    angle_t viewer_angle   = atan2f(viewer_status->speed.y, viewer_status->speed.x);
    angle_t solution_angle = atan2f(solution->y - viewer_status->position.y,
                                    solution->x - viewer_status->position.x);
    angle_t angle_diff = solution_angle - viewer_angle;
    if (angle_diff < 0)
    {
        angle_diff = -angle_diff;
    }
    /* Fold into [0, pi] */
    while (angle_diff > 3.14159265f)
    {
        angle_diff -= 3.14159265f;
    }

    if (rotate_from_nul_speed)
    {
        return (angle_diff > MIN_ANGLE_TO_DO_PRE_ROTATION);
    }
    else
    {
        return (angle_diff <= MAX_ANGLE_TO_NOT_DO_PRE_ROTATION);
    }
}

static void dj_point_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          dj_dynamic_point_solution_t *solution,
                                          pf_obstacle_dynamic_get_position_args_t *args)
{
    float start_time = (float)viewer_status->time * 0.001f;
    float end_time   = (float)(obstacle->update_time + OBSTACLE_LIFETIME_MS) * 0.001f;
    if (end_time < start_time)
    {
        solution->nb_solutions = 0;
        return;
    }

    /* Convert all quantities to SI units */
    float tadvi  = (float)obstacle->update_time * 0.001f;
    float trob   = (float)viewer_status->time   * 0.001f;

    float Aadvx  = obstacle->acceleration.x   * 0.001f;
    float Aadvy  = obstacle->acceleration.y   * 0.001f;
    float Vadvix = obstacle->initial_speed.x  * 0.001f;
    float Vadviy = obstacle->initial_speed.y  * 0.001f;
    float Padvix = obstacle->initial_position.x * 0.001f;
    float Padviy = obstacle->initial_position.y * 0.001f;

    float Arob   = args->prop_linear_acceleration * 0.001f;
    float Vrobi  = sqrtf(SQUARE(viewer_status->speed.x) + SQUARE(viewer_status->speed.y)) * 0.001f;
    float Probix = viewer_status->position.x * 0.001f;
    float Probiy = viewer_status->position.y * 0.001f;

    /* Polynomial coefficients a*t^4 + b*t^3 + c*t^2 + d*t + e = 0 */
    float l_a = (SQUARE(Aadvx) / 2.0f - SQUARE(Arob) / 4.0f);
    float l_b = (Aadvx * (Vadvix - Aadvx * tadvi) + Aadvx * (Vadviy - Aadvx * tadvi) -
                 Arob * (Vrobi - Arob * trob));
    float l_c = (SQUARE(Vadvix - Aadvx * tadvi) + SQUARE(Vadviy - Aadvx * tadvi) -
                 SQUARE(Vrobi - Arob * trob) +
                 Aadvx * (Aadvx * SQUARE(tadvi) / 2.0f - Vadvix * tadvi + Padvix - Probix) +
                 Aadvx * (Aadvx * SQUARE(tadvi) / 2.0f - Vadviy * tadvi + Padviy - Probiy) +
                 Arob * (Vrobi * trob - Arob * SQUARE(trob) / 2.0f));
    float l_d = (2.0f * (Vadvix - Aadvx * tadvi) *
                     (Aadvx * SQUARE(tadvi) / 2.0f - Vadvix * tadvi + Padvix - Probix) +
                 2.0f * (Vadviy - Aadvx * tadvi) *
                     (Aadvx * SQUARE(tadvi) / 2.0f - Vadviy * tadvi + Padviy - Probiy) +
                 2.0f * (Vrobi - Arob * trob) * (Vrobi * trob - Arob * SQUARE(trob) / 2.0f));
    float l_e = SQUARE(Aadvx * SQUARE(tadvi) / 2.0f - Vadvix * tadvi + Padvix - Probix) +
                SQUARE(Aadvx * SQUARE(tadvi) / 2.0f - Vadviy * tadvi + Padviy - Probiy) -
                SQUARE(Vrobi * trob - Arob * SQUARE(trob) / 2.0f);

    solution->nb_solutions = 0;

    bool sign      = false;
    bool first_loop = true;
    for (float t = start_time; t < end_time; t += BRUTE_FORCE_STEP)
    {
        bool new_sign = ((l_a * t * t * t * t + l_b * t * t * t + l_c * t * t + l_d * t + l_e) > 0);
        if (first_loop)
        {
            sign       = new_sign;
            first_loop = false;
        }
        else if (new_sign != sign)
        {
            sign = new_sign;
            float dt_adv = t - tadvi;
            solution->solutions[solution->nb_solutions] =
                (point_t){.x = (Padvix + Vadvix * dt_adv + Aadvx * SQUARE(dt_adv) / 2.0f) * 1000.0f,
                          .y = (Padviy + Vadviy * dt_adv + Aadvy * SQUARE(dt_adv) / 2.0f) * 1000.0f};
            if (solution->nb_solutions >= 3)
            {
                break;
            }
            solution->nb_solutions++;
        }
    }
}

static void point_to_polygon_solution(dj_obstacle_dynamic_t *obstacle,
                                      point_t *point_solution,
                                      dj_polygon_t *polygon_solution)
{
    SYSTEM_ASSERT(obstacle != NULL);
    SYSTEM_ASSERT(point_solution != NULL);
    SYSTEM_ASSERT(polygon_solution != NULL);
    polygon_solution->nb_points = obstacle->initial_shape.nb_points;
    for (uint8_t i = 0; i < obstacle->initial_shape.nb_points; i++)
    {
        polygon_solution->points[i] =
            (point_t){.x = point_solution->x + obstacle->initial_shape.points[i].x,
                      .y = point_solution->y + obstacle->initial_shape.points[i].y};
    }
}

/* ********************************************** Public functions definitions ******************************************* */

void pf_obstacle_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                      dj_viewer_status_t *viewer_status,
                                      dj_dynamic_polygon_solution_t *solution)
{
    SYSTEM_ASSERT(obstacle != NULL);
    SYSTEM_ASSERT(viewer_status != NULL);
    SYSTEM_ASSERT(solution != NULL);
    pf_obstacle_dynamic_get_position_args_t *args =
        (pf_obstacle_dynamic_get_position_args_t *)obstacle->get_position_args;
    SYSTEM_ASSERT(args != NULL);

    /* Optimisation : static obstacle (no speed/acceleration) -> single solution at initial position */
    if (obstacle->initial_speed.x == 0 && obstacle->initial_speed.y == 0 &&
        obstacle->acceleration.x  == 0 && obstacle->acceleration.y  == 0)
    {
        point_to_polygon_solution(obstacle, &obstacle->initial_position, &solution->solutions[0]);
        solution->nb_solutions = 1;
        return;
    }

    /* Find solutions from null speed then from viewer speed */
    dj_dynamic_point_solution_t found_from_nul_speed;
    dj_viewer_status_t viewer_nul = *viewer_status;
    viewer_nul.speed = (vector_2d_distance_t){.x = 0, .y = 0};
    dj_point_dynamic_get_position(obstacle, &viewer_nul, &found_from_nul_speed, args);

    dj_dynamic_point_solution_t found_from_viewer_speed;
    dj_point_dynamic_get_position(obstacle, viewer_status, &found_from_viewer_speed, args);

    solution->nb_solutions = 0;
    for (uint8_t i = 0; i < found_from_nul_speed.nb_solutions; i++)
    {
        if (check_solution_validity(&found_from_nul_speed.solutions[i], viewer_status, true))
        {
            point_to_polygon_solution(obstacle,
                                      &found_from_nul_speed.solutions[i],
                                      &solution->solutions[solution->nb_solutions]);
            solution->nb_solutions++;
        }
    }
    for (uint8_t i = 0; i < found_from_viewer_speed.nb_solutions; i++)
    {
        if (check_solution_validity(&found_from_viewer_speed.solutions[i], viewer_status, false))
        {
            point_to_polygon_solution(obstacle,
                                      &found_from_viewer_speed.solutions[i],
                                      &solution->solutions[solution->nb_solutions]);
            solution->nb_solutions++;
        }
    }
}
