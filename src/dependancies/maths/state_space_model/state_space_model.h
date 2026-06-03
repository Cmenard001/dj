/**
 * @file state_space_model.h
 * @brief Module pour utiliser des modèles à espace d'état
 * @author Théo Magne
 * @date 2021
 * @see state_space_model.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/matrix/matrix_f32.h"
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

#define STATE_VECTOR_MAX_SIZE 4 //n
#define INPUT_MAX_SIZE        4 //m
#define OUTPUT_MAX_SIZE       4 //l

/**
 * @brief définition d une structure abstaite pour le modèle d'espace d'état
 * x_size n
 * u_size m
 * z_size l
 * F system's dynamic matrix
 * B control matrix
 * H Observation matrix
 */
#define ABSTRACT_STATE_SPACE_MODEL()                                                               \
    uint8_t x_size;                                                                                \
    uint8_t u_size;                                                                                \
    uint8_t z_size;                                                                                \
                                                                                                   \
    matrix_f32_t F;                                                                                \
    float F_array[STATE_VECTOR_MAX_SIZE * STATE_VECTOR_MAX_SIZE];                                  \
    matrix_f32_t F_trans;                                                                          \
    float F_trans_array[STATE_VECTOR_MAX_SIZE * STATE_VECTOR_MAX_SIZE];                            \
                                                                                                   \
    matrix_f32_t B;                                                                                \
    float B_array[STATE_VECTOR_MAX_SIZE * INPUT_MAX_SIZE];                                         \
                                                                                                   \
    matrix_f32_t H;                                                                                \
    float H_array[STATE_VECTOR_MAX_SIZE * OUTPUT_MAX_SIZE];                                        \
    matrix_f32_t H_trans;                                                                          \
    float H_trans_array[STATE_VECTOR_MAX_SIZE * OUTPUT_MAX_SIZE];                                  \
                                                                                                   \
    matrix_f32_t x_tmp;                                                                            \
    float x_tmp_array[STATE_VECTOR_MAX_SIZE];                                                      \
                                                                                                   \
    matrix_f32_t x;                                                                                \
    float x_array[STATE_VECTOR_MAX_SIZE];                                                          \
    matrix_f32_t x_ref;                                                                            \
    float x_ref_array[STATE_VECTOR_MAX_SIZE];                                                      \
    matrix_f32_t u;                                                                                \
    float u_array[INPUT_MAX_SIZE];                                                                 \
    matrix_f32_t z;                                                                                \
    float z_array[OUTPUT_MAX_SIZE];

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief définition de la structure à utiliser pour le modèle
 */
typedef struct
{
    ABSTRACT_STATE_SPACE_MODEL();
} state_space_model_t;

/* *********************************************** Public functions declarations ***************************************** */
/**
 * @brief initialisation du modèle avec les matrices données
 * @param ss_model pointeur sur la structure à utiliser
 * @param F_array le tableau d'initialisation de la matrice F (dynamique du système)
 * @param B_array le tableau d'initialisation de la matrice B (matrice d'input)
 * @param H_array le tableau d'initialisation de la matrice H (matrice d'observation)
 * @param state_vector_size la taille du vecteur d'état
 * @param input_vector_size la taille du vecteur d'entrée
 * @param output_vector_size la taille du vecteur de sortie
 */
void state_space_model_init(state_space_model_t *ss_model,
                            float *F_array,
                            float *B_array,
                            float *H_array,
                            uint8_t state_vector_size,
                            uint8_t input_vector_size,
                            uint8_t output_vector_size);

/**
 * @brief accesseur sur le vecteur d'état pour initialiser l'état du système au t0
 * @param ss_model pointeur sur la structure d'espace d'état à utiliser
 * @param x_values valeur à utiliser pour mettre à jour le vecteur
 */
void state_space_model_set_x0(state_space_model_t *ss_model, float *x_values);

/**
 * @brief effectue une mise à jour du système d'état (un pas)
 * @param ss_model pointeur sur la structure d'état à utiliser
 */
void state_space_model_step(state_space_model_t *ss_model);

/* ******************************************* Public callback functions declarations ************************************ */
