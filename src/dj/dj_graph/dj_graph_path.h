/**
 * @file dj_graph_path.h
 * @brief Dj path object
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_path.c
 */

#ifndef __DJ_GRAPH_PATH_H__
#define __DJ_GRAPH_PATH_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_viewer/dj_viewer_status.h"

#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

MAGIC_ARRAY_TYPEDEF(point_list, GEOMETRY_point_t, 20);

/**
 * @brief Path object
 */
typedef struct
{
    /**
     * @brief List of points in the path
     * @warning The first point is the initial status of the path
     */
    point_list_t m_points;
    /**
     * @brief Initial status of the path
     */
    dj_viewer_status_t m_init_status;
    /**
     * @brief Duration of the path
     * @warning This value is not always up to date, please use dj_graph_path_get_duration to get the duration
     */
    time32_t m_duration;
    /**
     * @brief Status at the end of the path
     * @warning This value is not always up to date, please use dj_graph_path_get_end_status to get the status
     */
    dj_viewer_status_t m_end_status;
    /**
     * @brief Flag to know if the duration and the end status must be recomputed
     */
    bool must_be_recomputed;
    /**
     * @brief Flag to know if the path is finished
     */
    bool is_finished;
} dj_graph_path_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize a path object
 *
 * @param path Path object to initialize
 */
void dj_graph_path_init(dj_graph_path_t *path, dj_viewer_status_t *init_status);

/**
 * @brief Deinitialize a path object
 *
 * @param path Path object to deinitialize
 */
void dj_graph_path_deinit(dj_graph_path_t *path);

/**
 * @brief Add a point to the path
 * @warning The first point added to the path is the initial status of the path
 *
 * @param path Path object
 * @param point Point to add
 * @return uint32_t The index of the point in the path or -1 if the link is invalid
 */
int32_t dj_graph_path_add_point(dj_graph_path_t *path, GEOMETRY_point_t *point);

/**
 * @brief Get the link at the specified index
 *
 * @param [in] path Path object
 * @param [in] index Index of the point to get
 * @param [out] point Point to get
 */
void dj_graph_path_get_point(dj_graph_path_t *path, GEOMETRY_point_t *point, uint32_t index);

/**
 * @brief Get the number of links in the path
 *
 * @param path Path object
 * @return uint32_t The number of points in the path
 */
uint32_t dj_graph_path_get_point_count(dj_graph_path_t *path);

/**
 * @brief Get the initial status of the path
 *
 * @param [in] path Path object
 * @param [out] init_status Initial status of the path
 */
void dj_graph_path_get_init_status(dj_graph_path_t *path, dj_viewer_status_t *init_status);

/**
 * @brief Set the initial status of the path
 *
 * @param path Path object
 * @param init_status Initial status of the path
 */
void dj_graph_path_set_init_status(dj_graph_path_t *path, dj_viewer_status_t *init_status);

/**
 * @brief Get the total duration of the path
 *
 * @param path Path object
 * @return time32_t The total duration of the path
 */
time32_t dj_graph_path_get_duration(dj_graph_path_t *path);

/**
 * @brief Set the path as a finished or not
 *
 * @param path Path object
 * @param is_solution True if the path is finished, false otherwise
 */
void dj_graph_path_set_as_finished(dj_graph_path_t *path, bool is_finished);

/**
 * @brief Check if the path is finished
 *
 * @param path Path object
 * @return bool True if the path is finished, false otherwise
 */
bool dj_graph_path_is_finished(dj_graph_path_t *path);

/**
 * @brief Copy a path object
 *
 * @param dest Destination path object
 * @param src Source path object
 *
 * @warning The destination path object musn't be initialized
 */
void dj_graph_path_copy(dj_graph_path_t *dest, dj_graph_path_t *src);

/**
 * @brief Get the position of the end of the path
 *
 * @param [in] path Path object
 * @param [out] end End of the path
 */
void dj_graph_path_get_end(dj_graph_path_t *path, GEOMETRY_point_t *end);

/**
 * @brief Get the status at the end of the path
 *
 * @param [in] path Path object
 * @param [out] end_status Status at the end of the path
 */
void dj_graph_path_get_end_status(dj_graph_path_t *path, dj_viewer_status_t *end_status);

/**
 * @brief Check if a point is in the path
 *
 * @param [in] path Path object
 * @param [in] point Point to check
 * @return bool True if the point is in the path, false otherwise
 */
bool dj_graph_path_is_in_path(dj_graph_path_t *path, GEOMETRY_point_t *point);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
