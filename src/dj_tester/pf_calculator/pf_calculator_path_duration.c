/**
 * @file pf_calculator_path_duration.c
 * @brief Source file for the path duration calculator module.
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see pf_calculator_path_duration.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "pf_calculator_path_duration.h"
#include "system/assert/system_assert.h"
#include "utils/maths/angle/angle.h"
#include "utils/maths/position/position.h"
#include "utils/maths/segment/segment.h"
#include <math.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Max angle difference for continuous trajectory (no stop required) [rad]
 * @note Corresponds to CPLT_TRAJ_MAX_ANGLE_DIFF_FOR_CONTINUOUS_TRAJ_RAD in the full system
 */
#define DJ_MAX_ANGLE_FOR_CONTINUOUS_TRAJ_RAD 1.2f

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static bool is_pre_rotation_needed(angle_t angle_diff);

static time_ms_t dj_graph_rotation_get_duration(angle_t angle_diff,
                                                 pf_path_duration_calculator_args_t *args);

static time_ms_t dj_graph_segment_get_duration(segment_t *segment,
                                                linear_speed_t initial_speed,
                                                bool immobile_at_last_point,
                                                pf_path_duration_calculator_args_t *args);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

static bool is_pre_rotation_needed(angle_t angle_diff)
{
    angle_t abs_angle_diff = fabsf(angle_diff);
    return abs_angle_diff >= DJ_MAX_ANGLE_FOR_CONTINUOUS_TRAJ_RAD;
}

static time_ms_t dj_graph_rotation_get_duration(angle_t angle_diff,
                                                 pf_path_duration_calculator_args_t *args)
{
    /*
        Calculate the duration of a pure rotation (symmetric accel/decel).

        1) Triangular profile (w_max not reached, alpha <= w_max^2 / a):
           t_total = 2 * sqrt(alpha / a)   =>   in ms: 2000 * sqrt(alpha / a)

        2) Trapezoidal profile (w_max reached, alpha > w_max^2 / a):
           t_total = 2 * w_max / a + alpha_cruise / w_max
    */
    if (angle_diff < 0)
    {
        angle_diff = -angle_diff;
    }

    float a      = args->angular_acceleration;
    SYSTEM_ASSERT(a > 0.0f);
    float w_max              = args->max_angular_speed;
    float alpha_triangle_max = w_max * w_max / a;

    time_ms_t duration;
    if (angle_diff <= alpha_triangle_max)
    {
        duration = (time_ms_t)(2000.0f * sqrtf(angle_diff / a));
    }
    else
    {
        float t_accel    = w_max / a;
        float alpha_cruise = angle_diff - alpha_triangle_max;
        float t_cruise   = alpha_cruise / w_max;
        duration = (time_ms_t)(1000.0f * (2.0f * t_accel + t_cruise));
    }
    return duration;
}

static time_ms_t dj_graph_segment_get_duration(segment_t *segment,
                                                linear_speed_t initial_speed,
                                                bool immobile_at_last_point,
                                                pf_path_duration_calculator_args_t *args)
{
    SYSTEM_ASSERT(segment != NULL);
    distance_t distance = point_distance(&segment->a, &segment->b);
    time_ms_t duration;

    /* Convert to SI units */
    float aa    = args->linear_acceleration / 1000.0f; /* mm/s^2 -> m/s^2 */
    float v0    = initial_speed             / 1000.0f; /* mm/s   -> m/s   */
    float v_max = args->max_linear_speed    / 1000.0f; /* mm/s   -> m/s   */
    float D     = distance                  / 1000.0f; /* mm     -> m     */

    if (immobile_at_last_point)
    {
        float ad = args->linear_deceleration / 1000.0f;
        SYSTEM_ASSERT(aa + ad > 0.0f);

        if (v0 >= v_max)
        {
            float d_decel    = v0 * v0 / (2.0f * ad);
            float d_cruise   = D - d_decel;
            float time_in_s;
            if (d_cruise > 0.0f)
            {
                time_in_s = d_cruise / v0 + v0 / ad;
            }
            else
            {
                time_in_s = v0 / ad;
            }
            duration = (time_ms_t)(time_in_s * 1000.0f);
        }
        else
        {
            float v_peak_sq = (2.0f * aa * ad * D + ad * v0 * v0) / (aa + ad);
            float v_peak    = sqrtf(v_peak_sq);

            if (v_peak <= v_max)
            {
                float time_in_s = (v_peak - v0) / aa + v_peak / ad;
                duration = (time_ms_t)(time_in_s * 1000.0f);
            }
            else
            {
                float t_accel  = (v_max - v0) / aa;
                float d_accel  = v0 * t_accel + 0.5f * aa * t_accel * t_accel;
                float t_decel  = v_max / ad;
                float d_decel  = 0.5f * v_max * t_decel;
                float d_cruise = D - d_accel - d_decel;
                float t_cruise = d_cruise / v_max;
                float time_in_s = t_accel + t_cruise + t_decel;
                duration = (time_ms_t)(time_in_s * 1000.0f);
            }
        }
    }
    else
    {
        if (v0 >= v_max)
        {
            float time_in_s = D / v_max;
            duration = (time_ms_t)(time_in_s * 1000.0f);
        }
        else
        {
            float d_accel = (v_max * v_max - v0 * v0) / (2.0f * aa);
            if (d_accel >= D)
            {
                float v_final   = sqrtf(v0 * v0 + 2.0f * aa * D);
                float time_in_s = (v_final - v0) / aa;
                duration = (time_ms_t)(time_in_s * 1000.0f);
            }
            else
            {
                float t_accel   = (v_max - v0) / aa;
                float d_cruise  = D - d_accel;
                float t_cruise  = d_cruise / v_max;
                float time_in_s = t_accel + t_cruise;
                duration = (time_ms_t)(time_in_s * 1000.0f);
            }
        }
    }
    return duration;
}

/* ********************************************** Public functions definitions ******************************************* */

time_ms_t pf_path_duration_calculator(dj_graph_path_t *path)
{
    SYSTEM_ASSERT(path != NULL);
    pf_path_duration_calculator_args_t *args =
        (pf_path_duration_calculator_args_t *)path->get_duration_callback_args;
    SYSTEM_ASSERT(args != NULL);

    if (point_list_size(&path->points) < 2)
    {
        path->duration          = 0;
        path->end_status        = path->init_status;
        path->must_be_recomputed = false;
        return 0;
    }

    time_ms_t          duration = 0;
    dj_viewer_status_t status   = path->init_status;

    /* Initial rotation: from robot heading to first segment direction */
    {
        point_t *first_point = point_list_get(&path->points, 1);
        SYSTEM_ASSERT(first_point != NULL);
        angle_t first_seg_angle;
        viewing_angle(&status.position, first_point, &first_seg_angle);
        angle_t angle_diff = modulo_angle(first_seg_angle - path->init_heading);
        if (is_pre_rotation_needed(angle_diff))
        {
            duration += dj_graph_rotation_get_duration(angle_diff, args);
            status.speed = (vector_2d_distance_t){0, 0};
        }
    }

    for (uint32_t i = 1; i < point_list_size(&path->points) - 1; i++)
    {
        bool    immobile_at_last_point = false;
        point_t *initial_point         = &status.position;
        point_t *final_point           = point_list_get(&path->points, i);
        if (final_point == NULL)
        {
            continue;
        }
        point_t *next_point = point_list_get(&path->points, i + 1);
        if (next_point == NULL)
        {
            continue;
        }

        angle_t angle_link;
        angle_t next_angle_link;
        viewing_angle(initial_point, final_point, &angle_link);
        viewing_angle(final_point, next_point, &next_angle_link);
        angle_t angle_diff = modulo_angle(next_angle_link - angle_link);

        linear_speed_t initial_speed =
            sqrtf(status.speed.x * status.speed.x + status.speed.y * status.speed.y);

        if (is_pre_rotation_needed(angle_diff))
        {
            immobile_at_last_point = true;
            duration += dj_graph_rotation_get_duration(angle_diff, args);
        }

        segment_t  current_segment  = {*initial_point, *final_point};
        time_ms_t  current_duration = dj_graph_segment_get_duration(&current_segment,
                                                                     initial_speed,
                                                                     immobile_at_last_point,
                                                                     args);
        duration += current_duration;

        status.position   = *final_point;
        status.time      += current_duration;

        if (immobile_at_last_point)
        {
            status.speed = (vector_2d_distance_t){0, 0};
        }
        else
        {
            distance_t     seg_distance = point_distance(&current_segment.a, &current_segment.b);
            linear_speed_t v_final      = sqrtf(initial_speed * initial_speed +
                                                2.0f * args->linear_acceleration * seg_distance);
            if (v_final > args->max_linear_speed)
            {
                v_final = args->max_linear_speed;
            }
            status.speed = (vector_2d_distance_t){v_final * cosf(angle_link),
                                                  v_final * sinf(angle_link)};
        }
    }

    /* Last segment: always stop */
    point_t *initial_point =
        &status.position;
    point_t *final_point =
        point_list_get(&path->points, point_list_size(&path->points) - 1);
    SYSTEM_ASSERT(final_point != NULL);
    segment_t  current_segment  = {*initial_point, *final_point};
    linear_speed_t initial_speed =
        sqrtf(status.speed.x * status.speed.x + status.speed.y * status.speed.y);
    time_ms_t current_duration =
        dj_graph_segment_get_duration(&current_segment, initial_speed, true, args);
    duration += current_duration;
    status.position   = *final_point;
    status.time      += current_duration;
    status.speed      = (vector_2d_distance_t){0, 0};

    /* Final rotation if arrival angle is imposed */
    if (path->use_arrival_angle)
    {
        angle_t last_seg_angle = 0.0f;
        viewing_angle(initial_point, final_point, &last_seg_angle);
        angle_t angle_diff = modulo_angle(path->arrival_angle - last_seg_angle);
        duration += dj_graph_rotation_get_duration(angle_diff, args);
    }

    path->duration          = duration;
    path->end_status        = status;

    return duration;
}
