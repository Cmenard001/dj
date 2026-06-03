/**
 * @file system_assert.h
 * @brief System assert stub — maps to standard assert for the test build
 * @author Cyprien Ménard
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

#include "system/log/log.h"
#include "utils/macros/macros.h"
#include <assert.h>
#include <stddef.h>

#define SYSTEM_ASSERT(x)          assert(x)
#define SYSTEM_ASSERT_MSG(x, msg) assert(x)
