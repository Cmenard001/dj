/**
 * @file dj_test_suite.h
 * @brief A test suite groups a set of DJ pathfinding tests with a shared obstacle configuration
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see dj_test_suite.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#ifndef __DJ_TEST_SUITE_H__
#define __DJ_TEST_SUITE_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj_tester/dj_test_set/dj_test_set.h"
#include "utils/dj/dj_obstacle/dj_oversize_obstacle.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/maths/distance/distance.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum number of obstacles in a single test suite
 */
#define DJ_TEST_SUITE_MAX_OBSTACLES (10)

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Configuration of a single static obstacle to import into the engine for a suite
 */
typedef struct
{
    dj_polygon_t polygon;                     /**< Shape of the obstacle */
    bool is_enabled;                          /**< Whether the obstacle is active */
    dj_obstacle_oversize_mode_t oversize_mode; /**< How to oversize the obstacle */
    distance_t margin;                        /**< Margin applied around the obstacle [mm] */
    distance_t smooth_extraction_radius;      /**< Smooth extraction radius [mm] */
} dj_test_obstacle_t;

/**
 * @brief Configuration of a test suite (shared obstacle layout + playground bounds + test count)
 */
typedef struct dj_test_suite_config_s
{
    const char *name;            /**< Human-readable name displayed in the report */
    uint32_t    nb_tests;        /**< Number of random pathfinding tests to run */
    int32_t     playground_min_x; /**< Minimum X boundary of the playground [mm] */
    int32_t     playground_max_x; /**< Maximum X boundary of the playground [mm] */
    int32_t     playground_min_y; /**< Minimum Y boundary of the playground [mm] */
    int32_t     playground_max_y; /**< Maximum Y boundary of the playground [mm] */
    dj_test_obstacle_t obstacles[DJ_TEST_SUITE_MAX_OBSTACLES]; /**< Static obstacles for this suite */
    uint8_t     nb_obstacles;    /**< Number of obstacles in the obstacles array */
} dj_test_suite_config_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Run a test suite: set up the engine with the suite obstacles, generate tests, and run them.
 * @param suite Pointer to the suite configuration (read-only).
 * @param test_set Pointer to the test set to populate and run.
 */
void dj_test_suite_run(const dj_test_suite_config_t *suite, dj_test_set_t *test_set);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TEST_SUITE_H__ */
