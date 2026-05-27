/**
 * @file matrix_f32.h
 * @brief Lightweight float32 matrix operations
 * @author Copilot
 * @date 11/03/2026
 * @see matrix_f32.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Lightweight matrix descriptor — the caller owns the data buffer (no dynamic allocation).
 */
typedef struct
{
    uint16_t num_rows;
    uint16_t num_cols;
    float *p_data;
} matrix_f32_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Bind a matrix descriptor to an existing buffer.
 * @param mat  Pointer to the matrix to initialise
 * @param rows Number of rows
 * @param cols Number of columns
 * @param data Pointer to a float array of at least rows*cols elements
 */
void matrix_f32_init(matrix_f32_t *mat, uint16_t rows, uint16_t cols, float *data);

/**
 * @brief Matrix addition: dst = a + b (element-wise, same dimensions required).
 */
void matrix_f32_add(const matrix_f32_t *a, const matrix_f32_t *b, matrix_f32_t *dst);

/**
 * @brief Matrix multiplication: dst = a x b.
 *        a is (m x n), b is (n x p), dst must be (m x p).
 *        dst must NOT alias a or b.
 */
void matrix_f32_mult(const matrix_f32_t *a, const matrix_f32_t *b, matrix_f32_t *dst);

/**
 * @brief Matrix transpose: dst = aᵀ.
 *        If a is (m x n), dst must be (n x m).
 *        dst must NOT alias a.
 */
void matrix_f32_trans(const matrix_f32_t *a, matrix_f32_t *dst);

/* ******************************************* Public callback functions declarations ************************************ */
