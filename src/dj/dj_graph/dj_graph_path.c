/**
 * @file dj_graph_path.C
 * @brief Dj path object
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_path.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph/dj_graph_path.h"
#include "system/assert/system_assert.h"
#include "utils/dj/dj_logs/dj_time_marker.h"
#include "utils/maths/angle/angle.h"
#include "utils/maths/kinematics/linear_speed/linear_speed.h"
#include "utils/maths/position/position.h"
#include "utils/maths/segment/segment.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

LOG_REGISTER("utils/dj/graph_path");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

MAGIC_ARRAY_DEFINITION(point_list, point_t)

/* *********************************************** Public functions declarations ***************************************** */

void dj_graph_path_init(dj_graph_path_t *path,
                        dj_viewer_status_t *init_status,
                        angle_t arrival_angle,
                        bool use_arrival_angle,
                        angle_t init_heading,
                        dj_graph_path_get_duration_callback_t get_duration_callback,
                        void *get_duration_callback_args)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(init_status != NULL);
    SYSTEM_ASSERT(get_duration_callback != NULL);
    point_list_init(&path->points);
    // Add the initial point to the path
    point_list_add(&path->points, &init_status->position);
    path->init_status = *init_status;
    path->must_be_recomputed = true;
    path->is_finished = false;
    path->get_duration_callback = get_duration_callback;
    path->get_duration_callback_args = get_duration_callback_args;
    path->init_heading = init_heading;
    path->arrival_angle = arrival_angle;
    path->use_arrival_angle = use_arrival_angle;
}

uint32_t dj_graph_path_add_point(dj_graph_path_t *path, point_t *point)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(point != NULL);
    point_list_add(&path->points, point);
    path->must_be_recomputed = true;
    return point_list_size(&path->points) - 1;
}

void dj_graph_path_get_point(const dj_graph_path_t *path, point_t *point, uint32_t index)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(point != NULL);
    point_t *p = point_list_get(&path->points, index);
    if (p == NULL)
    {
        *point = (point_t){0, 0}; // Default value if get fails
        return;
    }
    *point = *p;
}

uint32_t dj_graph_path_get_point_count(const dj_graph_path_t *path)
{
    SYSTEM_ASSERT(path != NULL);
    return point_list_size(&path->points);
}

void dj_graph_path_get_init_status(const dj_graph_path_t *path, dj_viewer_status_t *init_status)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(init_status != NULL);
    *init_status = path->init_status;
}

void dj_graph_path_set_init_status(dj_graph_path_t *path, dj_viewer_status_t *init_status)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(init_status != NULL);
    path->init_status = *init_status;
    path->must_be_recomputed = true;
}

time_ms_t dj_graph_path_get_duration(dj_graph_path_t *path)
{
    SYSTEM_ASSERT(path != NULL);
    if (!path->must_be_recomputed)
    {
        return path->duration;
    }
    dj_mark_start_time(PATH_DURATION);

    SYSTEM_ASSERT(path->get_duration_callback != NULL);
    time_ms_t duration = path->get_duration_callback(path);
    path->duration = duration;

    path->must_be_recomputed = false;

    dj_mark_end_time(PATH_DURATION);

    return duration;
}

void dj_graph_path_set_as_finished(dj_graph_path_t *path, bool is_finished)
{
    SYSTEM_ASSERT(path != NULL);
    path->is_finished = is_finished;
}

bool dj_graph_path_is_finished(const dj_graph_path_t *path)
{
    SYSTEM_ASSERT(path != NULL);
    return path->is_finished;
}

void dj_graph_path_copy(dj_graph_path_t *dest, dj_graph_path_t *src)
{
    SYSTEM_ASSERT(dest != NULL);
    SYSTEM_ASSERT(src != NULL);
    // Initialize the destination path
    dj_graph_path_init(dest,
                       &src->init_status,
                       src->arrival_angle,
                       src->use_arrival_angle,
                       src->init_heading,
                       src->get_duration_callback,
                       src->get_duration_callback_args);
    // Copy all parameters
    dest->duration = src->duration;
    dest->is_finished = src->is_finished;
    dest->must_be_recomputed = src->must_be_recomputed;
    dest->end_status = src->end_status;
    dest->init_status = src->init_status;
    // Make a copy of all points in the linked list
    // (The first point is already added in the init function)
    for (uint32_t i = 1; i < point_list_size(&src->points); i++)
    {
        point_t *point = point_list_get(&src->points, i);
        SYSTEM_ASSERT(point != NULL);
        dj_graph_path_add_point(dest, point);
    }
}

void dj_graph_path_get_end(const dj_graph_path_t *path, point_t *end)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(end != NULL);
    *end = *(point_t *)point_list_get(&path->points, point_list_size(&path->points) - 1);
    SYSTEM_ASSERT(end != NULL);
}

void dj_graph_path_get_end_status(dj_graph_path_t *path, dj_viewer_status_t *end_status)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(end_status != NULL);
    if (path->must_be_recomputed)
    {
        dj_graph_path_get_duration(path);
    }
    *end_status = path->end_status;
}

bool dj_graph_path_is_in_path(const dj_graph_path_t *path, point_t *point)
{
    SYSTEM_ASSERT(path != NULL);
    SYSTEM_ASSERT(point != NULL);
    for (uint32_t i = 0; i < point_list_size(&path->points); i++)
    {
        point_t *p = point_list_get(&path->points, i);
        SYSTEM_ASSERT(p != NULL);
        if (p->x == point->x && p->y == point->y)
        {
            return true;
        }
    }
    return false;
}

/* ******************************************* Public callback functions declarations ************************************ */
