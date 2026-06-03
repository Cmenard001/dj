/**
 * @file state_space_model.c
 * @brief Module pour utiliser des modèles à espace d'état
 * @author Cyprien Ménard
 * @date 14/10/2025
 * @see state_space_model.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "state_space_model.h"
#include "system/assert/system_assert.h"
#include <string.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void state_space_model_init(state_space_model_t *ss_model,
                            float *F_array,
                            float *B_array,
                            float *H_array,
                            uint8_t state_vector_size,
                            uint8_t input_vector_size,
                            uint8_t output_vector_size)
{
    SYSTEM_ASSERT(ss_model != NULL);
    SYSTEM_ASSERT(F_array != NULL);
    SYSTEM_ASSERT(B_array != NULL);
    SYSTEM_ASSERT(H_array != NULL);
    /* Vérification des dimensions */
    SYSTEM_ASSERT(state_vector_size <= STATE_VECTOR_MAX_SIZE);
    SYSTEM_ASSERT(input_vector_size <= INPUT_MAX_SIZE);
    SYSTEM_ASSERT(output_vector_size <= OUTPUT_MAX_SIZE);
    ss_model->x_size = state_vector_size;  // state vector size
    ss_model->u_size = input_vector_size;  // input size
    ss_model->z_size = output_vector_size; // output size

    /* Copie des tableaux passées en paramètre pour avoir les valeurs initiales des matrices */
    memcpy(ss_model->F_array, F_array, sizeof(float) * ss_model->x_size * ss_model->x_size);
    memcpy(ss_model->B_array, B_array, sizeof(float) * ss_model->x_size * ss_model->u_size);
    memcpy(ss_model->H_array, H_array, sizeof(float) * ss_model->z_size * ss_model->x_size);

    /* Initialisation des matrices */
    matrix_f32_init(&ss_model->F, ss_model->x_size, ss_model->x_size, ss_model->F_array);
    matrix_f32_init(&ss_model->F_trans,
                    ss_model->x_size,
                    ss_model->x_size,
                    ss_model->F_trans_array);
    matrix_f32_trans(&ss_model->F, &ss_model->F_trans);
    matrix_f32_init(&ss_model->B, ss_model->x_size, ss_model->u_size, ss_model->B_array);
    matrix_f32_init(&ss_model->H, ss_model->z_size, ss_model->x_size, ss_model->H_array);

    if (ss_model->z_size > 0)
    {
        /* Calcule et initialisation de la transposé de la matrice d'observation */
        matrix_f32_init(&ss_model->H_trans,
                        ss_model->x_size,
                        ss_model->z_size,
                        ss_model->H_trans_array);
        matrix_f32_trans(&ss_model->H, &ss_model->H_trans);
        /* Initialisation du vecteur de mesure */
        matrix_f32_init(&ss_model->z, ss_model->z_size, 1, ss_model->z_array);
    }
    /* Initialisation des vecteurs d'état et de référence */
    matrix_f32_init(&ss_model->x, ss_model->x_size, 1, ss_model->x_array);
    matrix_f32_init(&ss_model->x_tmp, ss_model->x_size, 1, ss_model->x_tmp_array);
    matrix_f32_init(&ss_model->x_ref, ss_model->x_size, 1, ss_model->x_ref_array);

    /* Initialisation du vecteur d'entré */
    matrix_f32_init(&ss_model->u, ss_model->u_size, 1, ss_model->u_array);
}

void state_space_model_set_x0(state_space_model_t *ss_model, float *x_values)
{
    SYSTEM_ASSERT(ss_model != NULL);
    SYSTEM_ASSERT(x_values != NULL);
    for (uint8_t x = 0; x < ss_model->x_size; x++)
    {
        ss_model->x_array[x] = x_values[x];
    }
}

void state_space_model_step(state_space_model_t *ss_model)
{
    SYSTEM_ASSERT(ss_model != NULL);
    //Predict next state
    matrix_f32_mult(&ss_model->B, &ss_model->u, &ss_model->x_tmp);
    matrix_f32_mult(&ss_model->F, &ss_model->x, &ss_model->x);
    matrix_f32_add(&ss_model->x_tmp, &ss_model->x, &ss_model->x);
}

/* ***************************************** Public callback functions definitions *************************************** */
