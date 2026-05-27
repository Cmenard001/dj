/**
 * @file matrix_f32.c
 * @brief Lightweight float32 matrix operations
 * @author Copilot
 * @date 11/03/2026
 * @see matrix_f32.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "matrix_f32.h"
#include "system/assert/system_assert.h"
#include <stddef.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void matrix_f32_init(matrix_f32_t *mat, uint16_t rows, uint16_t cols, float *data)
{
    SYSTEM_ASSERT(mat != NULL);
    SYSTEM_ASSERT(data != NULL);
    mat->num_rows = rows;
    mat->num_cols = cols;
    mat->p_data = data;
}

void matrix_f32_add(const matrix_f32_t *a, const matrix_f32_t *b, matrix_f32_t *dst)
{
    SYSTEM_ASSERT(a != NULL && b != NULL && dst != NULL);
    SYSTEM_ASSERT(a->num_rows == b->num_rows && a->num_cols == b->num_cols);
    SYSTEM_ASSERT(a->num_rows == dst->num_rows && a->num_cols == dst->num_cols);

    uint32_t n = (uint32_t)a->num_rows * a->num_cols;
    for (uint32_t i = 0; i < n; i++)
    {
        dst->p_data[i] = a->p_data[i] + b->p_data[i];
    }
}

void matrix_f32_mult(const matrix_f32_t *a, const matrix_f32_t *b, matrix_f32_t *dst)
{
    SYSTEM_ASSERT(a != NULL && b != NULL && dst != NULL);
    SYSTEM_ASSERT(a->num_cols == b->num_rows);
    SYSTEM_ASSERT(dst->num_rows == a->num_rows && dst->num_cols == b->num_cols);

    uint16_t m = a->num_rows;
    uint16_t n = a->num_cols;
    uint16_t p = b->num_cols;

    for (uint16_t i = 0; i < m; i++)
    {
        for (uint16_t j = 0; j < p; j++)
        {
            float sum = 0.0f;
            for (uint16_t k = 0; k < n; k++)
            {
                sum += a->p_data[(uint32_t)i * n + k] * b->p_data[(uint32_t)k * p + j];
            }
            dst->p_data[(uint32_t)i * p + j] = sum;
        }
    }
}

void matrix_f32_trans(const matrix_f32_t *a, matrix_f32_t *dst)
{
    SYSTEM_ASSERT(a != NULL && dst != NULL);
    SYSTEM_ASSERT(dst->num_rows == a->num_cols && dst->num_cols == a->num_rows);

    for (uint16_t i = 0; i < a->num_rows; i++)
    {
        for (uint16_t j = 0; j < a->num_cols; j++)
        {
            dst->p_data[(uint32_t)j * a->num_rows + i] = a->p_data[(uint32_t)i * a->num_cols + j];
        }
    }
}

/* ***************************************** Public callback functions definitions *************************************** */
