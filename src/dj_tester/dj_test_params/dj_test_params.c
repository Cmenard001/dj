/**
 * @file dj_test_params.c
 * @brief Parameters of a dj test — engine initialisation and test execution
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see dj_test_params.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_test_params.h"
#include "dj_tester/dj_test_suite/dj_test_suite.h"
#include "dj_tester/pf_calculator/pf_calculator_dynamic_obstacles.h"
#include "dj_tester/pf_calculator/pf_calculator_path_duration.h"
#include "pos_simulator/pos_simulator.h"
#include "utils/dj/dj_engine.h"
#include "utils/dj/dj_goal_point.h"
#include "utils/dj/dj_graph/dj_graph_path.h"
#include "utils/dj/dj_solver/dj_solver_common/dj_solver_common.h"

/* **************************************************** Private macros *************************************************** */

/** Robot radius used for all tests [mm] */
#define TEST_ROBOT_RADIUS_MM (150.0f)

/** Maximum linear speed [mm/s] */
#define TEST_MAX_LINEAR_SPEED        (800.0f)
/** Linear acceleration [mm/s^2] */
#define TEST_LINEAR_ACCELERATION     (1500.0f)
/** Linear deceleration [mm/s^2] */
#define TEST_LINEAR_DECELERATION     (1500.0f)
/** Maximum angular speed [rad/s] */
#define TEST_MAX_ANGULAR_SPEED       (3.14f)
/** Angular acceleration [rad/s^2] */
#define TEST_ANGULAR_ACCELERATION    (10.0f)
/** Angular deceleration [rad/s^2] */
#define TEST_ANGULAR_DECELERATION    (10.0f)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static void args_generator_cb(dj_engine_calculator_args_t *args_out,
                               dj_engine_propulsion_properties_t *args_in);

/* ************************************************** Private variables ************************************************** */

static dj_engine_t                             g_test_engine;
static pf_path_duration_calculator_args_t      g_duration_args;
static pf_obstacle_dynamic_get_position_args_t g_dyn_obs_args;

/* ********************************************** Private functions definitions ****************************************** */

static void args_generator_cb(dj_engine_calculator_args_t *args_out,
                               dj_engine_propulsion_properties_t *args_in)
{
    g_duration_args.max_linear_speed      = args_in->max_linear_speed;
    g_duration_args.linear_acceleration   = args_in->linear_acceleration;
    g_duration_args.linear_deceleration   = args_in->linear_deceleration;
    g_duration_args.max_angular_speed     = args_in->max_angular_speed;
    g_duration_args.angular_acceleration  = args_in->angular_acceleration;
    g_duration_args.end_condition         = args_in->end_condition;

    g_dyn_obs_args.prop_linear_acceleration = args_in->linear_acceleration;

    args_out->path_duration_calculator_args             = &g_duration_args;
    args_out->dynamic_obstacle_position_calculator_args = &g_dyn_obs_args;
}

/* ********************************************** Public functions definitions ******************************************* */

void dj_test_params_engine_init(void)
{
    dj_engine_init(&g_test_engine,
                   TEST_ROBOT_RADIUS_MM,
                   NULL,
                   pf_path_duration_calculator,
                   pf_obstacle_dynamic_get_position,
                   args_generator_cb);
}

void dj_test_params_engine_setup_for_suite(const dj_test_suite_config_t *suite)
{
    dj_test_params_engine_init();

    for (uint8_t i = 0; i < suite->nb_obstacles; i++)
    {
        const dj_test_obstacle_t *obs    = &suite->obstacles[i];
        dj_polygon_t              poly_copy = obs->polygon;
        dj_engine_import_static_obstacle(&g_test_engine,
                                         &poly_copy,
                                         obs->is_enabled,
                                         obs->oversize_mode,
                                         obs->margin,
                                         obs->smooth_extraction_radius);
    }
}

void dj_test_params_init(dj_test_params_t *params, point_t start_point, point_t destination_point)
{
    params->m_start_point       = start_point;
    params->m_destination_point = destination_point;
}

bool dj_test_params_start_test(const dj_test_params_t *params)
{
    pos_simulator_set_point(params->m_start_point);

    position_t start_pos = pos_simulator_get_position();
    dj_engine_propulsion_properties_t props;
    dj_engine_init_propulsion_properties(&props,
                                          TEST_MAX_LINEAR_SPEED,
                                          TEST_LINEAR_ACCELERATION,
                                          TEST_LINEAR_DECELERATION,
                                          TEST_MAX_ANGULAR_SPEED,
                                          TEST_ANGULAR_ACCELERATION,
                                          TEST_ANGULAR_DECELERATION,
                                          0.0f,
                                          PROP_END_AT_BRAKE,
                                          &start_pos);

    dj_goal_point_t goal;
    goal.point         = params->m_destination_point;
    goal.use_angle     = false;
    goal.arrival_angle = 0.0f;

    dj_graph_path_t path;
    bool success = false;
    dj_engine_generate_path(&g_test_engine,
                             &props,
                             &goal,
                             1,
                             &path,
                             &success,
                             DJ_SOLVER_DIJKSTRA,
                             false);
    return success;
}

/* ***************************************** Public callback functions definitions *************************************** */