/**
 * @file log.h
 * @brief Logging stub — no-op for this platform (test build)
 * @author Cyprien Ménard
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* No-op logging macros for the test build */
#define LOG_REGISTER(name)     /* no-op */
#define LOGD(fmt, ...)         /* no-op */
#define LOGI(fmt, ...)         /* no-op */
#define LOGW(fmt, ...)         /* no-op */
#define LOGE(fmt, ...)         /* no-op */
