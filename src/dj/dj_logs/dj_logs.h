/**
 * @file dj_logs.h
 * @brief Manage the logs of dj
 * @author Cyprien Ménard
 * @date 12/2024
 */

#ifndef __DJ_LOGS_H__
#define __DJ_LOGS_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_config.h"
#include "../dj_dependencies/dj_dependencies.h"


#include <stddef.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Macro to print debug logs
 * @note This macro will print the logs only if DJ_ENABLE_DEBUG_LOGS is enabled
 */
#if DJ_ENABLE_DEBUG_LOGS
#define dj_debug_printf(...) dj_dep_debug_printf(__VA_ARGS__)
#else
#define dj_debug_printf(...) (void)0
#endif

#define DJ_LOGS_PREFIX "DJ: "

/**
 * @brief Print an error log
 * @param ... The log to print
 */
#define dj_error_printf(...) dj_dep_error_printf(DJ_LOGS_PREFIX __VA_ARGS__)

#define dj_info_printf(...) dj_dep_info_printf(DJ_LOGS_PREFIX __VA_ARGS__)

#define dj_control_non_null(pointer, return_value)                                                                          \
    if (pointer == NULL)                                                                                                    \
    {                                                                                                                       \
        dj_error_printf("Pointer is NULL (file: %s, function: %s, line: %d)\n", __FILE__, __func__, __LINE__);              \
        return return_value;                                                                                                \
    }

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */

#endif
