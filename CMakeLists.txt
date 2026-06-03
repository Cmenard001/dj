cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(dj C ASM)

message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")

if(NOT TARGET_NAME)
    message(FATAL_ERROR "TARGET_NAME cache variable is not set. Please use a CMake preset that sets it.")
endif()
if(NOT SPECIFIC_CMAKE_PATH)
    message(FATAL_ERROR "SPECIFIC_CMAKE_PATH cache variable is not set. Please use a CMake preset that sets it.")
endif()

message(STATUS "TARGET_NAME: ${TARGET_NAME}")
message(STATUS "SPECIFIC_CMAKE_PATH: ${SPECIFIC_CMAKE_PATH}")

include(cmake/tools.cmake)

####################################################################################################
# Generate source-tree symlinks (Linux/DevContainer) or copies (Windows fallback) for DJ includes
####################################################################################################
set(SRC_ABS "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(MAKE_DIRECTORY "${SRC_ABS}/utils")
foreach(_pair
    "dj|${SRC_ABS}/dj"
    "maths|${SRC_ABS}/dependancies/maths"
    "macros|${SRC_ABS}/dependancies/macros"
    "time|${SRC_ABS}/api/time"
    "lists|${SRC_ABS}/dependancies/lists"
    "shape|${SRC_ABS}/dependancies/shape"
)
    string(REPLACE "|" ";" _p "${_pair}")
    list(GET _p 0 _name)
    list(GET _p 1 _target)
    file(CREATE_LINK "${_target}" "${SRC_ABS}/utils/${_name}" SYMBOLIC COPY_ON_ERROR)
endforeach()

# Top-level src/ symlinks for stubs stored in src/dependancies/
foreach(_name "system" "propulsion" "hmi")
    file(CREATE_LINK "${SRC_ABS}/dependancies/${_name}" "${SRC_ABS}/${_name}" SYMBOLIC COPY_ON_ERROR)
endforeach()

# Pass TARGET_NAME as a compile-time string constant
add_compile_definitions(DJ_TARGET_NAME="${TARGET_NAME}")

####################################################################################################
# Common source files for all targets (from src directory)
####################################################################################################

add_sources(
    # DJ engine (new version)
    dj/dj_engine.c
    dj/dj_graph/dj_graph_graph.c
    dj/dj_graph/dj_graph_link.c
    dj/dj_graph/dj_graph_node.c
    dj/dj_graph/dj_graph_path.c
    dj/dj_graph_builder/dj_graph_builder.c
    dj/dj_graph_builder/dj_graph_builder_intersection.c
    dj/dj_graph_builder/dj_graph_builder_link_ops.c
    dj/dj_graph_builder/dj_graph_builder_node_ops.c
    dj/dj_graph_builder/dj_graph_builder_precompute.c
    dj/dj_graph_builder/dj_graph_rebuilder.c
    dj/dj_launcher/dj_launcher.c
    dj/dj_logs/dj_time_marker.c
    dj/dj_obstacle/dj_obstacle.c
    dj/dj_obstacle/dj_obstacle_dynamic.c
    dj/dj_obstacle/dj_obstacle_manager.c
    dj/dj_obstacle/dj_obstacle_static.c
    dj/dj_obstacle/dj_oversize_obstacle.c
    dj/dj_obstacle/dj_polygon.c
    dj/dj_obstacle_importer/dj_obstacle_id.c
    dj/dj_obstacle_importer/dj_obstacle_importer.c
    dj/dj_solver/dj_solver.c
    dj/dj_solver/dj_solver_astar/dj_solver_astar.c
    dj/dj_solver/dj_solver_common/dj_solver_common.c
    dj/dj_solver/dj_solver_dijkstra/dj_solver_dijkstra.c
    dj/dj_viewer/dj_viewer_status.c

    # Maths library
    dependancies/maths/maths.c
    dependancies/maths/angle/angle.c
    dependancies/maths/axis/axis.c
    dependancies/maths/circle/circle.c
    dependancies/maths/cluster/cluster.c
    dependancies/maths/distance/distance.c
    dependancies/maths/matrix/matrix_f32.c
    dependancies/maths/point/point.c
    dependancies/maths/polygon/polygon.c
    dependancies/maths/position/position.c
    dependancies/maths/segment/segment.c
    dependancies/maths/vector/vector.c
    dependancies/maths/kinematics/angular_acceleration/angular_acceleration.c
    dependancies/maths/kinematics/angular_speed/angular_speed.c
    dependancies/maths/kinematics/linear_acceleration/linear_acceleration.c
    dependancies/maths/kinematics/linear_speed/linear_speed.c
    dependancies/maths/kinematics/torque/torque.c

    # DJ tester framework
    dj_tester/dj_test/dj_test.c
    dj_tester/dj_test_params/dj_test_params.c
    dj_tester/dj_test_report/dj_test_report.c
    dj_tester/dj_test_set/dj_test_set.c
    dj_tester/dj_test_suite/dj_test_suite.c
    dj_tester/dj_tester.c
    dj_tester/pf_calculator/pf_calculator_path_duration.c
    dj_tester/pf_calculator/pf_calculator_dynamic_obstacles.c

    point_generator/point_generator.c

    pos_simulator/pos_simulator.c
    )

####################################################################################################
# Common include directories for all targets (from src directory)
####################################################################################################
add_include_dirs(
    ${SRC_PREFIX}
    )

####################################################################################################
# Platform-agnostic core library reused by every executable (per-target) and the unit tests.
# The platform implementations of time_*/printf* are intentionally left out so that consumers
# (target executable or test binary) provide them — they resolve at link time.
####################################################################################################
add_library(dj_core STATIC ${SRCS})
target_include_directories(dj_core PUBLIC ${INCLUDE_DIRS})

# Print the sources and include directories
print_sources_and_include_dirs()

# Build the target-specific executable (links against dj_core)
include(${SPECIFIC_CMAKE_PATH})

####################################################################################################
# Documentation (Doxygen) — adds a non-ALL `doxygen` target when Doxygen is available
####################################################################################################
include(cmake/docs.cmake)

####################################################################################################
# Unit tests (GTest) — native Linux only, opt-in (off by default so cross-compile configures
# and ordinary native builds don't fetch GoogleTest)
####################################################################################################
option(DJ_BUILD_TESTS "Build the GTest unit tests (native Linux only)" OFF)
if(DJ_BUILD_TESTS AND TARGET_NAME STREQUAL "linux")
    enable_language(CXX)     # GoogleTest (and the test sources) need a C++ compiler
    enable_testing()
    add_subdirectory(test)
endif()
