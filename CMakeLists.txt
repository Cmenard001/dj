cmake_minimum_required(VERSION 3.5)

project(dj C)

include(cmake/tools.cmake)

####################################################################################################
# Common source files for all targets (from src directory)
####################################################################################################
set(SRC_PREFIX src)

add_sources(
    main.c
    dj/dj_dependencies/QS_maths/QS_maths_angle.c
    dj/dj_dependencies/QS_maths/QS_maths_circle.c
    dj/dj_dependencies/QS_maths/QS_maths_cluster.c
    dj/dj_dependencies/QS_maths/QS_maths_point.c
    dj/dj_dependencies/QS_maths/QS_maths_polygon.c
    dj/dj_dependencies/QS_maths/QS_maths_position.c
    dj/dj_dependencies/QS_maths/QS_maths_segment.c
    dj/dj_dependencies/QS_maths/QS_maths_vector.c
    dj/dj_dependencies/QS_maths/QS_maths.c
    dj/dj_dependencies/dj_dependencies.c
    dj/dj_graph/dj_graph_graph.c
    dj/dj_graph/dj_graph_link.c
    dj/dj_graph/dj_graph_node.c
    dj/dj_graph/dj_graph_path.c
    dj/dj_graph_builder/dj_graph_builder.c
    dj/dj_graph_builder/dj_graph_rebuilder.c
    dj/dj_graph_builder/dj_prebuilt_graph.c
    dj/dj_launcher/dj_launcher.c
    dj/dj_logs/dj_time_marker.c
    dj/dj_obstacle/dj_obstacle_dynamic.c
    dj/dj_obstacle/dj_obstacle_manager.c
    dj/dj_obstacle/dj_obstacle_static.c
    dj/dj_obstacle/dj_obstacle.c
    dj/dj_obstacle/dj_oversize_obstacle.c
    dj/dj_obstacle/dj_polygon.c
    dj/dj_obstacle_importer/dj_obstacle_id.c
    dj/dj_obstacle_importer/dj_obstacle_importer.c
    dj/dj_prop_config/dj_prop_config.c
    dj/dj_solver/dj_solver_astar/dj_solver_astar.c
    dj/dj_solver/dj_solver_common/dj_solver_common.c
    dj/dj_solver/dj_solver_dijkstra/dj_solver_dijkstra.c
    dj/dj_solver/dj_solver.c
    dj/dj_viewer/dj_viewer_status.c
    dj/dj.c
    )


# Print the sources and include directories
print_sources_and_include_dirs()

####################################################################################################
# Common include directories for all targets (from src directory)
####################################################################################################
add_include_dirs(
    ${SRC_PREFIX}
    )

add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
