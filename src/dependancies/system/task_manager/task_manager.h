/**
 * @file task_manager.h
 * @brief Task manager stub — no-op for the test build
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/**
 * @brief No-op: register a periodic main-thread process.
 */
#define task_manager_register_process_main(name, period_ms, func, arg) ((void)0)
