cmake_minimum_required(VERSION 3.5)

include(cmake/tools.cmake)

####################################################################################################
# Common source files for all targets (from src directory)
####################################################################################################
add_sources(
    main.c
    )

####################################################################################################
# Common include directories for all targets (from src directory)
####################################################################################################
add_include_dirs(
    src
    )

project(${TARGET} C)
