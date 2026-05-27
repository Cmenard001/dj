/**
 * @file point_generator.c
 * @brief Generate "random" points
 * @note The points are not really random, to be the same on each run, independent of the platform.
 * @author Cyprien Ménard
 * @date 22/06/2025
 * @see point_generator.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "point_generator.h"
#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Linear Congruential Generator (LCG) parameters.
 */
#define LCG_MULTIPLIER   1103515245U
#define LCG_INCREMENT    12345U
#define LCG_MASK         0x7fffffffU
#define LCG_SEED         123456789U

/**
 * @brief Playground boundaries (configurable at runtime via point_generator_set_bounds).
 */
#define DEFAULT_MIN_X (300)
#define DEFAULT_MAX_X (1700)
#define DEFAULT_MIN_Y (300)
#define DEFAULT_MAX_Y (2700)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static uint32_t generate_random_number();

static void integer_to_coordinate(uint32_t random_number, point_t *out);

/* ************************************************** Private variables ************************************************** */

static int32_t g_min_x = DEFAULT_MIN_X;
static int32_t g_max_x = DEFAULT_MAX_X;
static int32_t g_min_y = DEFAULT_MIN_Y;
static int32_t g_max_y = DEFAULT_MAX_Y;

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Generate a random number using a linear congruential generator (LCG).
 *
 * @return uint32_t The generated random number.
 */
static uint32_t generate_random_number()
{
    // Deterministic pseudo-random generator (LCG)
    static uint32_t state = LCG_SEED; // Fixed seed for reproducibility
    state = (LCG_MULTIPLIER * state + LCG_INCREMENT) & LCG_MASK;
    return state;
}

static void integer_to_coordinate(uint32_t random_number, point_t *out)
{
    // Converts the random number to a coordinate within the current bounds
    out->x = (distance_t)((int32_t)(random_number % (uint32_t)(g_max_x - g_min_x + 1)) + g_min_x);
    out->y = (distance_t)((int32_t)(random_number % (uint32_t)(g_max_y - g_min_y + 1)) + g_min_y);
}

/* ********************************************** Public functions definitions ******************************************* */

void point_generator_set_bounds(int32_t min_x, int32_t max_x, int32_t min_y, int32_t max_y)
{
    g_min_x = min_x;
    g_max_x = max_x;
    g_min_y = min_y;
    g_max_y = max_y;
}

void generate_point(point_t *out)
{
    uint32_t random_number = generate_random_number();
    integer_to_coordinate(random_number, out);
}

/* ***************************************** Public callback functions definitions *************************************** */