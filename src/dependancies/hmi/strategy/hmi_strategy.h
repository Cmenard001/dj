/**
 * @file hmi_strategy.h
 * @brief HMI strategy stub — no-op for the test build (no HMI)
 * @author Cyprien Ménard
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* Dummy type definitions */
typedef int hmi_strategy_input_id_t;
typedef int hmi_strategy_event_type_t;

/* Dummy constants */
#define HMI_STRATEGY_INPUT_ID_SWITCH_DJ_STATIC_OBSTACLES_DISPLAY  0
#define HMI_STRATEGY_EVENT_TYPE_ALL                                0
#define HMI_STRATEGY_EVENT_TYPE_ON                                 1

/* No-op callback registration */
#define hmi_strategy_register_input_event_callback(id, type, cb, arg) \
    do { (void)(id); (void)(type); (void)(cb); (void)(arg); } while (0)
