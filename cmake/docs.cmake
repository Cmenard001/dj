## Doxygen documentation target
##
## Adds a non-ALL `doxygen` target that renders a modern, browsable HTML
## documentation into ${CMAKE_BINARY_DIR}/docs/html. Build it explicitly with:
##     cmake --build <build-dir> --target doxygen
## The target is only created when Doxygen is installed, so cross-compile
## configures (or machines without Doxygen) are unaffected.

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(NOT DOXYGEN_FOUND)
    message(STATUS "Doxygen not found — the `doxygen` target is unavailable")
    return()
endif()

####################################################################################################
# Fetch the doxygen-awesome-css theme (light: two stylesheets, built-in auto dark mode).
# Downloaded once into the build tree; offline-safe (falls back to the stock theme on failure).
####################################################################################################
set(AWESOME_VERSION "v2.3.4")
set(AWESOME_DIR      "${CMAKE_BINARY_DIR}/doxygen-awesome-css")
set(AWESOME_OK       TRUE)

foreach(_css "doxygen-awesome.css" "doxygen-awesome-sidebar-only.css")
    if(NOT EXISTS "${AWESOME_DIR}/${_css}")
        file(DOWNLOAD
            "https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/${AWESOME_VERSION}/${_css}"
            "${AWESOME_DIR}/${_css}"
            STATUS _dl_status
            TLS_VERIFY ON)
        list(GET _dl_status 0 _dl_code)
        if(NOT _dl_code EQUAL 0)
            list(GET _dl_status 1 _dl_msg)
            message(STATUS "doxygen-awesome-css download failed (${_dl_msg}) — using stock theme")
            set(AWESOME_OK FALSE)
            break()
        endif()
    endif()
endforeach()

####################################################################################################
# Documentation settings
####################################################################################################
set(DOXYGEN_PROJECT_NAME        "dj")
set(DOXYGEN_PROJECT_BRIEF       "DJ pathfinding algorithm & multi-target benchmark")
set(DOXYGEN_OUTPUT_DIRECTORY    "${CMAKE_BINARY_DIR}/docs")

# Tune the parser for this C codebase
set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
set(DOXYGEN_TYPEDEF_HIDES_STRUCT  YES)
set(DOXYGEN_MARKDOWN_SUPPORT      YES)
set(DOXYGEN_EXTRACT_ALL           YES)
set(DOXYGEN_EXTRACT_STATIC        YES)
set(DOXYGEN_RECURSIVE             YES)
set(DOXYGEN_SORT_MEMBER_DOCS      YES)
set(DOXYGEN_QUIET                 YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED  NO)

# Use the project README as the documentation landing page
if(EXISTS "${CMAKE_SOURCE_DIR}/README.md")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
endif()

# Cross-referenced source browser
set(DOXYGEN_SOURCE_BROWSER          YES)
set(DOXYGEN_REFERENCED_BY_RELATION  YES)
set(DOXYGEN_REFERENCES_RELATION     YES)

# Graphs (require Graphviz/dot) — interactive SVG diagrams
set(DOXYGEN_HAVE_DOT              ${DOXYGEN_DOT_FOUND})
set(DOXYGEN_DOT_IMAGE_FORMAT     svg)
set(DOXYGEN_INTERACTIVE_SVG      YES)
# Per-function call / caller flow graphs
set(DOXYGEN_CALL_GRAPH           YES)
set(DOXYGEN_CALLER_GRAPH         YES)
# Data-structure relations (UML-style, with members)
set(DOXYGEN_CLASS_GRAPH          YES)
set(DOXYGEN_COLLABORATION_GRAPH  YES)
set(DOXYGEN_UML_LOOK             YES)
set(DOXYGEN_DOT_UML_DETAILS      YES)
set(DOXYGEN_TEMPLATE_RELATIONS   YES)
set(DOXYGEN_GRAPHICAL_HIERARCHY  YES)
# Header inclusion graphs
set(DOXYGEN_INCLUDE_GRAPH        YES)
set(DOXYGEN_INCLUDED_BY_GRAPH    YES)
# Module/group and directory dependency graphs
set(DOXYGEN_GROUP_GRAPHS         YES)
set(DOXYGEN_DIRECTORY_GRAPH      YES)
set(DOXYGEN_DIR_GRAPH_MAX_DEPTH  6)
# Render graphs in full (no node/depth truncation) and ship a legend
set(DOXYGEN_DOT_GRAPH_MAX_NODES  500)
set(DOXYGEN_MAX_DOT_GRAPH_DEPTH  0)
set(DOXYGEN_GENERATE_LEGEND      YES)

# HTML output
set(DOXYGEN_GENERATE_HTML        YES)
set(DOXYGEN_GENERATE_LATEX       NO)
set(DOXYGEN_GENERATE_TREEVIEW    YES)
set(DOXYGEN_DISABLE_INDEX        NO)
set(DOXYGEN_FULL_SIDEBAR         NO)
set(DOXYGEN_HTML_COLORSTYLE      LIGHT)   # required by doxygen-awesome (auto dark mode via CSS)

if(AWESOME_OK)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET
        "${AWESOME_DIR}/doxygen-awesome.css"
        "${AWESOME_DIR}/doxygen-awesome-sidebar-only.css")
endif()

set(DOXYGEN_EXCLUDE_PATTERNS
    "*/STM32F4xx_StdPeriph_Driver/*"
    "*/startup_files/*"
    "*/cmsis/*"
    "*/build/*"
    "*/utils/*")

####################################################################################################
# Target
####################################################################################################
set(_doxygen_inputs "${SRC_ABS}")
if(EXISTS "${CMAKE_SOURCE_DIR}/README.md")
    list(APPEND _doxygen_inputs "${CMAKE_SOURCE_DIR}/README.md")
endif()

doxygen_add_docs(doxygen
    ${_doxygen_inputs}
    COMMENT "Generating Doxygen documentation (HTML)")
