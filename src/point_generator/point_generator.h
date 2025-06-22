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

#include "dj/dj_dependencies/dj_dependencies.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Generate a "random" point.
 * @param out Pointer to the point structure to fill with the generated point.
 */
void generate_point(GEOMETRY_point_t *out);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __POINT_GENERATOR_H__ */