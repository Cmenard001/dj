/**
 * @file dj_graph_path.C
 * @brief Dj path object
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_path.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_graph_path.h"
#include "../dj_logs/dj_logs.h"
#include "../dj_logs/dj_time_marker.h"
#include "../dj_prop_config/dj_prop_config.h"

#include <stdint.h>
#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static time32_t dj_graph_segment_get_duration(GEOMETRY_segment_t *segment,
                                              int16_t initial_speed,
                                              bool immobile_at_last_point);
static time32_t dj_graph_rotation_get_duration(int16_t angle_diff);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

MAGIC_ARRAY_SRC(point_list, GEOMETRY_point_t)

/**
 * @brief Calculate the duration of a segment
 *
 * @param segment Segment to calculate the duration
 * @param accel Acceleration of the viewer
 * @param initial_speed Initial speed of the viewer
 * @param immobile_at_last_point True if the viewer is immobile at the last point
 *
 * @return time32_t Duration of the segment
 *
 * @warning The initial speed must be coherent with the angle betwin the segment and the viewer speed
 */
static time32_t dj_graph_segment_get_duration(GEOMETRY_segment_t *segment,
                                              int16_t initial_speed,
                                              bool immobile_at_last_point)
{
    dj_control_non_null(segment, 0);
    /*
        Calculate the duration of the segment
        With a constant acceleration (a) and a initial speed (v0)

        1) If the viewer musn't be immobile at the last point :
        v = a * t + v0
        d = 0.5 * a * t^2 + v0 * t
        0 = 0.5 * a * t^2 + v0 * t - d
        delta = v0^2 + 2 * a * d
        t = (sqrt(delta) - v0) / a
        But t is in ms so :
        t = 1000 * (sqrt(delta) - v0) / a

        2) If the viewer must be immobile at the last point :
        v0 --> initial_speed
        D --> distance
        t1 --> time of acceleration
        t2 --> time of acceleration + deceleration
        aa --> acceleration
        ad --> deceleration

        We solve the following system with Matlab :
            syms t1 t2 aa ad v0 D d1 v1
            eqn1 = v0+aa*t1-ad*(t2-t1) == 0
            eqn2 = d1 == 1/2*aa*t1^2 + v0*t1
            eqn3 = v1 == aa*t1
            eqn4 = D == -1/2*ad*(t2-t1)^2 + v1*(t2-t1) + d1
            eqn6 = t1 >= 0
            eqn7 = t2 > 0
            eqn8 = aa > 0
            eqn9 = ad > 0
            eqn10 = D > 0
            eqn11 = v0 >= 0
            eqns = [eqn1, eqn2, eqn3, eqn4, eqn6, eqn7, eqn8, eqn9, eqn10, eqn11]
            S=solve(eqns, t1, t2, d1, v1)
            S.t2
            Simpl = simplify(S.t2)

        We get :
            t2 = ((2*D*aa^2*ad + aa^2*v0^2 + 2*D*aa*ad^2 + aa*ad*v0^2 + ad^2*v0^2)^(1/2) + aa*v0 - ad*v0)/(aa*ad)
        <=> t2 = (sqrt(2*D*aa^2*ad + aa^2*v0^2 + 2*D*aa*ad^2 + aa*ad*v0^2 + ad^2*v0^2) + aa*v0 - ad*v0) / (aa * ad)
        But t2 is in ms so :
            t2 = 1000 * (sqrt(2 * D * aa * ad + aa^2 * v0^2 + 2 * D * aa * ad + aa * ad * v0^2 + ad^2 * v0^2) + aa * v0 - ad * v0) / (aa * ad)
    */
    int16_t distance = GEOMETRY_distance(segment->a, segment->b);
    time32_t duration;
    if (immobile_at_last_point)
    {
        // Convertion of the parameters to s.i. units
        float aa = PROP_LINEAR_ACCELERATION / 1000.0f; // mm/s^2 --> m/s^2
        float ad = PROP_LINEAR_DECELERATION / 1000.0f; // mm/s^2 --> m/s^2
        float v0 = initial_speed / 1000.0f;            // mm/s   --> m/s
        float D = distance / 1000.0f;
        // Calculate the duration of the segment
        float time_in_s
            = (sqrt(2 * D * aa * ad + aa * aa * v0 * v0 + 2 * D * aa * ad + aa * ad * v0 * v0 + ad * ad * v0 * v0) + aa * v0
               - ad * v0)
              / (aa * ad);
        duration = (time32_t)(time_in_s * 1000);
    }
    else
    {
        int32_t delta = (int32_t)initial_speed * initial_speed + 2 * PROP_LINEAR_ACCELERATION * distance;
        duration = 1000 * (sqrt(delta) - initial_speed) / PROP_LINEAR_ACCELERATION;
    }
    if (duration == 0)
    {
        duration = 0;
    }
    return duration;
}

/**
 * @brief Function to calculate the time of a pure rotation
 *
 * @param angle_diff Angle difference between the viewer and the target
 * @return time32_t Duration of the rotation
 */
static time32_t dj_graph_rotation_get_duration(int16_t angle_diff)
{
    /*
        Calculate the duration of a pure rotation
        With a constant acceleration (a) and alpha the angle difference
        We take the absolute value of the angle difference
        w = a * t
        alpha = 0.5 * a * t^2
        // But t is in ms and a in PI4096/s^2
        So :
        alpha = 0.5 * a * t^2 / 1000^2
        0 = 0.5 * a * t^2 / 1000^2 - alpha
        delta = 2 * a * alpha / 1000^2
        t = sqrt(delta) / (a / 1000^2)
        t = sqrt(2 * a * alpha / 1000^2) / (a / 1000^2)
        t = sqrt(2 * a * alpha) / a
        But t is in ms so :
        t = 1000 * sqrt(2 * a * alpha) / a
    */
    if (angle_diff < 0)
    {
        angle_diff = -angle_diff;
    }
    time32_t duration = 1000 * sqrt(2 * PROP_ANGULAR_ACCELERATION * angle_diff) / (PROP_ANGULAR_ACCELERATION);
    return duration;
}

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_path_init(dj_graph_path_t *path, dj_viewer_status_t *init_status)
{
    dj_control_non_null(path, );
    point_list_init(&path->m_points);
    // Add the initial point to the path
    point_list_add(&path->m_points, &init_status->m_position);
    path->m_init_status = *init_status;
    path->must_be_recomputed = true;
    path->is_finished = false;
}

void dj_graph_path_deinit(dj_graph_path_t *path)
{
}

int32_t dj_graph_path_add_point(dj_graph_path_t *path, GEOMETRY_point_t *point)
{
    dj_control_non_null(path, -1);
    dj_control_non_null(point, -1);
    point_list_add(&path->m_points, point);
    path->must_be_recomputed = true;
    return point_list_size(&path->m_points) - 1;
}

void dj_graph_path_get_point(dj_graph_path_t *path, GEOMETRY_point_t *point, uint32_t index)
{
    dj_control_non_null(path, );
    dj_control_non_null(point, );
    GEOMETRY_point_t *p = point_list_get(&path->m_points, index);
    dj_control_non_null(p, );
    *point = *p;
}

uint32_t dj_graph_path_get_point_count(dj_graph_path_t *path)
{
    dj_control_non_null(path, 0);
    return point_list_size(&path->m_points);
}

void dj_graph_path_get_init_status(dj_graph_path_t *path, dj_viewer_status_t *init_status)
{
    dj_control_non_null(path, );
    dj_control_non_null(init_status, );
    *init_status = path->m_init_status;
}

void dj_graph_path_set_init_status(dj_graph_path_t *path, dj_viewer_status_t *init_status)
{
    dj_control_non_null(path, );
    dj_control_non_null(init_status, );
    path->m_init_status = *init_status;
    path->must_be_recomputed = true;
}

time32_t dj_graph_path_get_duration(dj_graph_path_t *path)
{
    dj_control_non_null(path, 0);
    if (!path->must_be_recomputed)
    {
        return path->m_duration;
    }
    dj_mark_start_time(DJ_MARK_PATH_DURATION);
    // Check if there is enough points to calculate the duration
    // Only one point : the duration is 0
    if (point_list_size(&path->m_points) < 2)
    {
        path->m_duration = 0;
        path->m_end_status = path->m_init_status;
        path->must_be_recomputed = false;
        return 0;
    }
    time32_t duration = 0;
    dj_viewer_status_t status = path->m_init_status;
    for (uint32_t i = 1; i < point_list_size(&path->m_points) - 1; i++)
    {
        bool immobile_at_last_point = false;
        GEOMETRY_point_t *initial_point = &status.m_position;
        GEOMETRY_point_t *final_point = point_list_get(&path->m_points, i);
        dj_control_non_null(final_point, 0);
        GEOMETRY_point_t *next_point = point_list_get(&path->m_points, i + 1);
        dj_control_non_null(next_point, 0);
        // Check if a pre rotation is needed after the current segment
        int16_t angle_link;
        int16_t next_angle_link;
        int16_t angle_diff;

        angle_link = GEOMETRY_viewing_angle(initial_point->x, initial_point->y, final_point->x, final_point->y);
        next_angle_link = GEOMETRY_viewing_angle(final_point->x, final_point->y, next_point->x, next_point->y);
        angle_diff = GEOMETRY_modulo_angle(next_angle_link - angle_link);

        int32_t initial_speed = sqrt(status.m_speed.x * status.m_speed.x + status.m_speed.y * status.m_speed.y);

        // Check if a pre rotation is needed after the current segment
        if (is_pre_rotation_needed(angle_diff))
        {
            // A pre rotation is needed so the robot must be immobile at the last point
            immobile_at_last_point = true;
            // Calculate the duration of the rotation of the viewer at the end of the segment
            duration += dj_graph_rotation_get_duration(angle_diff);
        }

        // Calculate the segment
        GEOMETRY_segment_t current_segment = {*initial_point, *final_point};

        // Calculate the duration of the segment
        time32_t current_duration = dj_graph_segment_get_duration(&current_segment, initial_speed, immobile_at_last_point);
        duration += current_duration;

        // Update the status
        status.m_position = *final_point;
        status.m_time += current_duration;
        /*
            Calculate the new speed
            With a constant acceleration (a) and a initial speed (v)
            v = a * t
            but t is in ms, so :
            v = a * t / 1000
            but speed is a vector, so :
            vx = a * t / 1000 * cos(angle)
            vy = a * t / 1000 * sin(angle)
        */
        if (immobile_at_last_point)
        {
            status.m_speed = (GEOMETRY_vector_t){0, 0};
        }
        else
        {
            status.m_speed = (GEOMETRY_vector_t){
                status.m_speed.x + PROP_LINEAR_ACCELERATION * current_duration * cosf(angle_link) / 1000,
                status.m_speed.y + PROP_LINEAR_ACCELERATION * current_duration * sinf(angle_link) / 1000};
        }
    }
    // Calculate the last segment
    GEOMETRY_point_t *initial_point = &status.m_position;
    GEOMETRY_point_t *final_point = point_list_get(&path->m_points, point_list_size(&path->m_points) - 1);
    dj_control_non_null(final_point, 0);
    GEOMETRY_segment_t current_segment = {*initial_point, *final_point};
    int32_t initial_speed = sqrt(status.m_speed.x * status.m_speed.x + status.m_speed.y * status.m_speed.y);
    time32_t current_duration = dj_graph_segment_get_duration(&current_segment, initial_speed, true);
    duration += current_duration;
    // Update the status
    status.m_position = *final_point;
    status.m_time += current_duration;
    // Update the speed
    status.m_speed = (GEOMETRY_vector_t){0, 0};

    // Update path parameters
    path->m_duration = duration;
    path->m_end_status = status;
    path->must_be_recomputed = false;

    dj_mark_end_time(DJ_MARK_PATH_DURATION);

    return duration;
}

void dj_graph_path_set_as_finished(dj_graph_path_t *path, bool is_finished)
{
    dj_control_non_null(path, );
    path->is_finished = is_finished;
}

bool dj_graph_path_is_finished(dj_graph_path_t *path)
{
    dj_control_non_null(path, false);
    return path->is_finished;
}

void dj_graph_path_copy(dj_graph_path_t *dest, dj_graph_path_t *src)
{
    dj_control_non_null(dest, );
    dj_control_non_null(src, );
    // Initialize the destination path
    dj_graph_path_init(dest, &src->m_init_status);
    // Copy all parameters
    dest->m_duration = src->m_duration;
    dest->is_finished = src->is_finished;
    dest->must_be_recomputed = src->must_be_recomputed;
    dest->m_end_status = src->m_end_status;
    dest->m_init_status = src->m_init_status;
    // Make a copy of all points in the linked list
    // (The first point is already added in the init function)
    for (uint32_t i = 1; i < point_list_size(&src->m_points); i++)
    {
        GEOMETRY_point_t *point = point_list_get(&src->m_points, i);
        dj_control_non_null(point, );
        dj_graph_path_add_point(dest, point);
    }
}

void dj_graph_path_get_end(dj_graph_path_t *path, GEOMETRY_point_t *end)
{
    dj_control_non_null(path, );
    dj_control_non_null(end, );
    *end = *(GEOMETRY_point_t *)point_list_get(&path->m_points, point_list_size(&path->m_points) - 1);
    dj_control_non_null(end, );
}

void dj_graph_path_get_end_status(dj_graph_path_t *path, dj_viewer_status_t *end_status)
{
    dj_control_non_null(path, );
    dj_control_non_null(end_status, );
    if (path->must_be_recomputed)
    {
        dj_graph_path_get_duration(path);
    }
    *end_status = path->m_end_status;
}

bool dj_graph_path_is_in_path(dj_graph_path_t *path, GEOMETRY_point_t *point)
{
    dj_control_non_null(path, false);
    dj_control_non_null(point, false);
    for (uint32_t i = 0; i < point_list_size(&path->m_points); i++)
    {
        GEOMETRY_point_t *p = point_list_get(&path->m_points, i);
        dj_control_non_null(p, false);
        if (p->x == point->x && p->y == point->y)
        {
            return true;
        }
    }
    return false;
}

/* ******************************************* Public callback functions declarations ************************************ */
