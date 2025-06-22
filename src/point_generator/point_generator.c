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

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Linear Congruential Generator (LCG) parameters.
 */
#define LCG_MULTIPLIER   1103515245U
#define LCG_INCREMENT    12345U
#define LCG_MASK         0x7fffffffU
#define LCG_SEED         123456789U

/**
 * @brief Playground boundaries.
 */
#define PLAYGROUND_MIN_X (300)
#define PLAYGROUND_MAX_X (1700)
#define PLAYGROUND_MIN_Y (300)
#define PLAYGROUND_MAX_Y (2700)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static uint32_t generate_random_number();

static void integer_to_coordinate(uint32_t random_number, GEOMETRY_point_t *out);

/* ************************************************** Private variables ************************************************** */

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

static void integer_to_coordinate(uint32_t random_number, GEOMETRY_point_t *out)
{
    // Converts the random number to a coordinate in the range [PLAYGROUND_MIN_X, PLAYGROUND_MAX_X]
    out->x = (random_number % (PLAYGROUND_MAX_X - PLAYGROUND_MIN_X + 1)) + PLAYGROUND_MIN_X;
    // Converts the random number to a coordinate in the range [PLAYGROUND_MIN_Y, PLAYGROUND_MAX_Y]
    out->y = (random_number % (PLAYGROUND_MAX_Y - PLAYGROUND_MIN_Y + 1)) + PLAYGROUND_MIN_Y;
}

/* ********************************************** Public functions definitions ******************************************* */

void generate_point(GEOMETRY_point_t *out)
{
    uint32_t random_number = generate_random_number();
    integer_to_coordinate(random_number, out);
}

/* ***************************************** Public callback functions definitions *************************************** */