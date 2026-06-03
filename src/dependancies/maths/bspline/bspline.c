/**
 * @file bspline.c
 * @brief Gestion des B-Splines.
 * @author Théo Magne
 * @date 04/06/2022
 * @see bspline.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "bspline.h"
#include "system/assert/system_assert.h"
#include "utils/maths/include_arm_math.h"

/* **************************************************** Private macros *************************************************** */

#define INV_SIX ((float)0.166667f)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* control points data */
static arm_matrix_instance_f32 phi = {0};
static float phi_array[(CROSSING_POINTS_NB_MAX + 2) * (CROSSING_POINTS_NB_MAX + 2)] = {0};
static arm_matrix_instance_f32 inv_phi = {0};
static float inv_phi_array[(CROSSING_POINTS_NB_MAX + 2) * (CROSSING_POINTS_NB_MAX + 2)] = {0};
static arm_matrix_instance_f32 p = {0};
static float p_array[(CROSSING_POINTS_NB_MAX + 2) * 2] = {0};

/* curve data */
static arm_matrix_instance_f32 A = {0};
static float a_array[] = {-1, 3, -3, 1, 3, -6, 3, 0, -3, 0, 3, 0, 1, 4, 1, 0};

static arm_matrix_instance_f32 time_vector = {0};
static float time_vector_array[4] = {0, 0, 0, INV_SIX};
static arm_matrix_instance_f32 k_vector = {0};
static float k_vector_array[4] = {0};

static arm_matrix_instance_f32 x_vector = {0};
static float x_vector_array[4] = {0};
static arm_matrix_instance_f32 y_vector = {0};
static float y_vector_array[4] = {0};

static arm_matrix_instance_f32 x = {0};
static float x_array = 0;
static arm_matrix_instance_f32 y = {0};
static float y_array = 0;

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void bspline_compute_control_points(bspline_t *spline)
{
    SYSTEM_ASSERT(spline != NULL);
    /* Make sure we don t overflow our array */
    SYSTEM_ASSERT(spline->nb_points <= CROSSING_POINTS_NB_MAX);
    /* Init the matrix structure */
    uint8_t control_point_number = spline->nb_points + 2;
    arm_mat_init_f32(&phi, control_point_number, control_point_number, phi_array);

    /* Reset our array to zeros and config phi array */
    for (uint8_t i = 0; i < control_point_number * control_point_number; i++)
    {
        phi_array[i] = 0;
    }
    for (uint8_t i = 0; i < spline->nb_points; i++)
    {
        uint8_t index =
            (uint8_t)(control_point_number * (i + 1) + /* Select the line in the array */
                      i);                              /* Select the column in the array */
        phi_array[index++] = 1;
        phi_array[index++] = 4;
        phi_array[index] = 1;
    }
    phi_array[0] = -3;
    phi_array[2] = 3;
    phi_array[control_point_number * control_point_number - 3] = -3;
    phi_array[control_point_number * control_point_number - 1] = 3;

    /* Init the p vector */
    arm_mat_init_f32(&p, control_point_number, 2, p_array);

    /* Copy the point from spline->P to p with an offset of one line */
    float *p_array_second_line = &p_array[2]; /* [0] = x0, [1] = y0, [2] = x1 .. */
    for (uint8_t i = 0; i < spline->nb_points * 2; i++)
    {
        p_array_second_line[i] = spline->P[i];
    }
    /* Add the start and end tangent conditions */
    p_array[0] = spline->start_vector.x;
    p_array[1] = spline->start_vector.y;
    p_array[2 * spline->nb_points + 2] = spline->end_vector.x;
    p_array[2 * spline->nb_points + 3] = spline->end_vector.y;

    /* Compute the control points from the crossing points */
    arm_mat_init_f32(&inv_phi, control_point_number, control_point_number, inv_phi_array);
    arm_mat_inverse_f32(&phi, &inv_phi);
    arm_matrix_instance_f32 Q = {0};
    arm_mat_init_f32(&Q, control_point_number, 2, spline->Q);
    arm_mat_mult_f32(&inv_phi, &p, &Q);
    /* Multiply everything by six */
    for (uint8_t i = 0; i < control_point_number * 2; i++)
    {
        spline->Q[i] *= 6;
    }
}

bool bspline_compute_curve(const bspline_t *spline, bspline_curve_t *curve)
{
    SYSTEM_ASSERT(spline != NULL);
    SYSTEM_ASSERT(curve != NULL);
    SYSTEM_ASSERT(curve->resolution > 0.0f && curve->resolution <= 1.0f);
    SYSTEM_ASSERT(curve->nb_points_max > 0);
    curve->nb_points = 0;

    /* Init matrix once */
    static bool is_init_done = false;
    if (!is_init_done)
    {
        arm_mat_init_f32(&A, 4, 4, a_array);
        arm_mat_init_f32(&time_vector, 1, 4, time_vector_array);
        arm_mat_init_f32(&k_vector, 1, 4, k_vector_array);
        arm_mat_init_f32(&x_vector, 4, 1, x_vector_array);
        arm_mat_init_f32(&y_vector, 4, 1, y_vector_array);
        arm_mat_init_f32(&x, 1, 1, &x_array);
        arm_mat_init_f32(&y, 1, 1, &y_array);
    }

    for (uint8_t segment = 0; segment < (spline->nb_points - 1); segment++)
    {
        x_vector_array[0] = spline->Q[segment * 2];
        x_vector_array[1] = spline->Q[segment * 2 + 2];
        x_vector_array[2] = spline->Q[segment * 2 + 4];
        x_vector_array[3] = spline->Q[segment * 2 + 6];
        y_vector_array[0] = spline->Q[segment * 2 + 1];
        y_vector_array[1] = spline->Q[segment * 2 + 3];
        y_vector_array[2] = spline->Q[segment * 2 + 5];
        y_vector_array[3] = spline->Q[segment * 2 + 7];
        for (float t = 0; t < 1.0f; t += curve->resolution)
        {
            if (curve->nb_points >= curve->nb_points_max)
            {
                return false;
            }
            //time_vector_array[3] = INV_SIX; value set once at startup
            time_vector_array[2] = t * INV_SIX;
            time_vector_array[1] = time_vector_array[2] * t;
            time_vector_array[0] = time_vector_array[1] * t;
            arm_mat_mult_f32(&time_vector, &A, &k_vector);

            arm_mat_mult_f32(&k_vector, &x_vector, &x);
            arm_mat_mult_f32(&k_vector, &y_vector, &y);

            curve->points[curve->nb_points].x = x_array;
            curve->points[curve->nb_points++].y = y_array;
        }
    }

    return true;
}

/* ***************************************** Public callback functions definitions *************************************** */
