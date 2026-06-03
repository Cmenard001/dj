## Unit tests (GoogleTest)
##
## Verifies that every test of every benchmark suite finds a path. GoogleTest is
## fetched (pinned) so no system package is required; it only needs network at
## the first configure.

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)

# Don't override the parent project's compiler/linker settings on Windows builds
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# The test binary reuses dj_core but must provide the platform implementations of
# time_*/printf* that dj_core references (native Linux flavour here).
add_executable(dj_tests
    test_dj_pathfinding.cpp
    ${SRC_ABS}/api/time/timeLinux.c
    ${SRC_ABS}/api/printf/printfEmpty.c
)

target_link_libraries(dj_tests PRIVATE dj_core gtest_main m)

include(GoogleTest)
gtest_discover_tests(dj_tests)
