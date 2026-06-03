/**
 * @file point_generator.h
 * @brief Generate "random" points
 * @note The points are not really random, to be the same on each run, independent of the platform.
 * @author Cyprien Ménard
 * @date 22/06/2025
 * @see point_generator.c
 */

#ifndef __POINT_GENERATOR_H__
#define __POINT_GENERATOR_H__

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/point/point.h"
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Configure the bounding box used for point generation.
 * @param min_x Minimum X coordinate (mm).
 * @param max_x Maximum X coordinate (mm).
 * @param min_y Minimum Y coordinate (mm).
 * @param max_y Maximum Y coordinate (mm).
 */
void point_generator_set_bounds(int32_t min_x, int32_t max_x, int32_t min_y, int32_t max_y);

/**
 * @brief Reset the pseudo-random generator back to its initial seed.
 * @note Lets callers (e.g. unit tests) make point generation reproducible
 *       independently of how many points were previously generated.
 */
void point_generator_reset(void);

/**
 * @brief Generate a "random" point within the current bounds.
 * @param out Pointer to the point structure to fill with the generated point.
 */
void generate_point(point_t *out);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __POINT_GENERATOR_H__ */