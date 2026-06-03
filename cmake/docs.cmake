## Doxygen documentation target
##
## Adds a non-ALL `doxygen` target that renders the HTML documentation into
## ${CMAKE_BINARY_DIR}/docs/html. Build it explicitly with:
##     cmake --build <build-dir> --target doxygen
## The target is only created when Doxygen is installed, so cross-compile
## configures (or machines without Doxygen) are unaffected.

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME        "dj")
    set(DOXYGEN_RECURSIVE           YES)
    set(DOXYGEN_EXTRACT_ALL         YES)
    set(DOXYGEN_EXTRACT_STATIC      YES)
    set(DOXYGEN_GENERATE_HTML       YES)
    set(DOXYGEN_GENERATE_LATEX      NO)
    set(DOXYGEN_HAVE_DOT            ${DOXYGEN_DOT_FOUND})
    set(DOXYGEN_OUTPUT_DIRECTORY    "${CMAKE_BINARY_DIR}/docs")
    set(DOXYGEN_EXCLUDE_PATTERNS
        "*/STM32F4xx_StdPeriph_Driver/*"
        "*/startup_files/*"
        "*/build/*"
        "*/utils/*")

    doxygen_add_docs(doxygen
        "${SRC_ABS}"
        COMMENT "Generating Doxygen documentation (HTML)")
else()
    message(STATUS "Doxygen not found — the `doxygen` target is unavailable")
endif()
