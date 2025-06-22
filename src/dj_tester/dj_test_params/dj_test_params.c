/**
 * @file dj_test_params.c
 * @brief Parameters of a dj test
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test_params.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_test_params.h"
#include "dj/dj_launcher/dj_launcher.h"
#include "pos_simulator/pos_simulator.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_test_params_init(dj_test_params_t *params, GEOMETRY_point_t start_point, GEOMETRY_point_t destination_point)
{
    params->m_start_point = start_point;
    params->m_destination_point = destination_point;
}

bool dj_test_params_start_test(const dj_test_params_t *params)
{
    // Set the robot position to the start point
    pos_simulator_set_position(params->m_start_point);
    // Generate the path to the destination point
    dj_graph_path_t path;
    return dj_genarate_path_with_param(&path, params->m_destination_point, true);
}

/* ***************************************** Public callback functions definitions *************************************** */