/**
 * @file dj_graph_path.h
 * @brief Dj path object
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_path.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include "utils/lists/magic_array.h"
#include "utils/maths/angle/angle.h"
#include "utils/time/time.h"
#include <stdbool.h>
#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

MAGIC_ARRAY_TYPEDEF(point_list, point_t, 100);

/**
 * @brief Forward declaration of dj_graph_path_t
 */
typedef struct dj_graph_path_s dj_graph_path_t;

/**
 * @brief Callback function to calculate the deplacement time of a path
 * @param path Path object
 * @return time_ms_t Duration of the path
 */
typedef time_ms_t (*dj_graph_path_get_duration_callback_t)(dj_graph_path_t *path);

/**
 * @brief Path object
 */
struct dj_graph_path_s
{
    /**
     * @brief List of points in the path
     * @warning The first point is the initial status of the path
     */
    point_list_t points;
    /**
     * @brief Initial status of the path
     */
    dj_viewer_status_t init_status;
    /**
     * @brief Duration of the path
     * @warning This value is not always up to date, please use dj_graph_path_get_duration to get the duration
     */
    time_ms_t duration;
    /**
     * @brief Status at the end of the path
     * @warning This value is not always up to date, please use dj_graph_path_get_end_status to get the status
     */
    dj_viewer_status_t end_status;
    /**
     * @brief Flag to know if the duration and the end status must be recomputed
     */
    bool must_be_recomputed;
    /**
     * @brief Flag to know if the path is finished
     */
    bool is_finished;
    /**
     * @brief Callback function to calculate the duration of the path
     */
    dj_graph_path_get_duration_callback_t get_duration_callback;
    /**
     * @brief Additional arguments for the duration callback function
     */
    void *get_duration_callback_args;
    /**
     * @brief Initial heading of the robot [rad] - set by dj_launcher after path init
     * @note Used to compute the cost of the first rotation before the first segment
     */
    angle_t init_heading;
    /**
     * @brief Desired arrival angle [rad] - set by dj_launcher after path init
     */
    angle_t arrival_angle;
    /**
     * @brief If true, a final rotation to arrival_angle is required
     */
    bool use_arrival_angle;
};

/* *********************************************** Public functions declarations ***************************************** */

MAGIC_ARRAY_DECLARATION(point_list, point_t);

/**
 * @brief Initialize a path object
 * @param path Path object to initialize
 * @param init_status Initial status of the path
 * @param arrival_angle Desired arrival angle [rad]
 * @param use_arrival_angle If true, a final rotation to arrival_angle is required
 * @param init_heading Initial heading of the robot [rad]
 * @param get_duration_callback Callback function to calculate the duration of the path
 * @param get_duration_callback_args Additional arguments for the duration callback function
 */
void dj_graph_path_init(dj_graph_path_t *path,
                        dj_viewer_status_t *init_status,
                        angle_t arrival_angle,
                        bool use_arrival_angle,
                        angle_t init_heading,
                        dj_graph_path_get_duration_callback_t get_duration_callback,
                        void *get_duration_callback_args);

/**
 * @brief Add a point to the path
 * @warning The first point added to the path is the initial status of the path
 *
 * @param path Path object
 * @param point Point to add
 * @return uint32_t The index of the point in the path or -1 if the link is invalid
 */
uint32_t dj_graph_path_add_point(dj_graph_path_t *path, point_t *point);

/**
 * @brief Get the link at the specified index
 * @param [in] path Path object
 * @param [in] index Index of the point to get
 * @param [out] point Point to get
 */
void dj_graph_path_get_point(const dj_graph_path_t *path, point_t *point, uint32_t index);

/**
 * @brief Get the number of links in the path
 * @param path Path object
 * @return uint32_t The number of points in the path
 */
uint32_t dj_graph_path_get_point_count(const dj_graph_path_t *path);

/**
 * @brief Get the initial status of the path
 * @param [in] path Path object
 * @param [out] init_status Initial status of the path
 */
void dj_graph_path_get_init_status(const dj_graph_path_t *path, dj_viewer_status_t *init_status);

/**
 * @brief Set the initial status of the path
 * @param path Path object
 * @param init_status Initial status of the path
 */
void dj_graph_path_set_init_status(dj_graph_path_t *path, dj_viewer_status_t *init_status);

/**
 * @brief Get the total duration of the path
 * @param path Path object
 * @return time_ms_t The total duration of the path
 */
time_ms_t dj_graph_path_get_duration(dj_graph_path_t *path);

/**
 * @brief Set the path as a finished or not
 * @param path Path object
 * @param is_solution True if the path is finished, false otherwise
 */
void dj_graph_path_set_as_finished(dj_graph_path_t *path, bool is_finished);

/**
 * @brief Check if the path is finished
 * @param path Path object
 * @return bool True if the path is finished, false otherwise
 */
bool dj_graph_path_is_finished(const dj_graph_path_t *path);

/**
 * @brief Copy a path object
 * @param dest Destination path object
 * @param src Source path object
 *
 * @warning The destination path object musn't be initialized
 */
void dj_graph_path_copy(dj_graph_path_t *dest, dj_graph_path_t *src);

/**
 * @brief Get the position of the end of the path
 * @param [in] path Path object
 * @param [out] end End of the path
 */
void dj_graph_path_get_end(const dj_graph_path_t *path, point_t *end);

/**
 * @brief Get the status at the end of the path
 * @param [in] path Path object
 * @param [out] end_status Status at the end of the path
 */
void dj_graph_path_get_end_status(dj_graph_path_t *path, dj_viewer_status_t *end_status);

/**
 * @brief Check if a point is in the path
 * @param [in] path Path object
 * @param [in] point Point to check
 * @return bool True if the point is in the path, false otherwise
 */
bool dj_graph_path_is_in_path(const dj_graph_path_t *path, point_t *point);

/* ******************************************* Public callback functions declarations ************************************ */
