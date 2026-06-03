/**
 * @file test_dj_pathfinding.cpp
 * @brief Unit tests asserting that every benchmark test finds a path.
 *
 * The tests reuse the exact suites configured for the benchmark
 * (via dj_tester_get_suites) and check that each generated start/destination
 * pair yields a valid path (dj_test_t::m_success). Point generation is
 * deterministic (fixed-seed LCG) and reset before each suite, so the result is
 * reproducible regardless of test execution order.
 */

extern "C" {
#include "api/printf/printf.h"
#include "api/time/time.h"
#include "dj_tester/dj_test_set/dj_test_set.h"
#include "dj_tester/dj_test_suite/dj_test_suite.h"
#include "dj_tester/dj_tester.h"
#include "point_generator/point_generator.h"
}

#include <gtest/gtest.h>

namespace {

/** Initialise printf/time subsystems once before any test runs. */
class DjEnvironment : public ::testing::Environment {
   public:
    void SetUp() override {
        printfInit();
        time_init();
    }
};

::testing::Environment* const kDjEnvironment =
    ::testing::AddGlobalTestEnvironment(new DjEnvironment());

/** Number of benchmark suites (evaluated at registration time). */
uint32_t suite_count() {
    uint32_t count = 0;
    (void)dj_tester_get_suites(&count);
    return count;
}

class PathfindingSuiteTest : public ::testing::TestWithParam<uint32_t> {};

TEST_P(PathfindingSuiteTest, EveryTestFindsAPath) {
    uint32_t count = 0;
    const dj_test_suite_config_t* suites = dj_tester_get_suites(&count);
    const uint32_t index = GetParam();
    ASSERT_LT(index, count);
    const dj_test_suite_config_t* suite = &suites[index];

    // Make point generation independent of execution order / previous suites.
    point_generator_reset();

    dj_test_set_t set;
    dj_test_suite_run(suite, &set);

    EXPECT_GT(set.m_num_tests, 0) << "suite '" << suite->name
                                  << "' produced no tests";
    for (uint16_t i = 0; i < set.m_num_tests; ++i) {
        SCOPED_TRACE(::testing::Message()
                     << "suite='" << suite->name << "' test #" << i);
        EXPECT_TRUE(set.m_tests[i].m_success) << "no path found";
    }
}

INSTANTIATE_TEST_SUITE_P(AllSuites, PathfindingSuiteTest,
                         ::testing::Range<uint32_t>(0, suite_count()),
                         [](const ::testing::TestParamInfo<uint32_t>& info) {
                             return "Suite" + std::to_string(info.param);
                         });

}  // namespace
