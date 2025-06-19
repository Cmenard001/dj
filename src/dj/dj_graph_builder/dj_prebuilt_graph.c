/**
 * @file dj_prebuilt_graph.c
 * @brief Prebuilt graph is a graph that is built before the match and is used to build the graph faster
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_prebuilt_graph.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_prebuilt_graph.h"
#include "../dj_graph/dj_graph_node.h"
#include "../dj_obstacle/dj_polygon.h"
#include "dj_graph_builder.h"
#include "dj_graph_rebuilder.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

static dj_graph_builder_t prebuilt_graph DJ_DEP_HEAP_VARS_SUFFIX;

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_prebuilt_graph_init()
{
    dj_graph_builder_init(&prebuilt_graph);
    // Build the graph
    dj_debug_printf("Build prebuilt graph\n");
    dj_graph_builder_build_all(&prebuilt_graph, NULL, true);
}

void dj_prebuilt_graph_deinit()
{
    dj_graph_builder_deinit(&prebuilt_graph);
}

void rebuild_graph_with_prebuilt_graph(dj_graph_builder_t *graph_builder, dj_viewer_status_t *viewer)
{
    dj_control_non_null(graph_builder, );
    dj_control_non_null(viewer, );
    dj_graph_builder_rebuild(graph_builder, &prebuilt_graph.m_graph, viewer);
}

void first_rebuild_graph_with_prebuilt_graph(dj_graph_builder_t *builder, GEOMETRY_point_t start, GEOMETRY_point_t end)
{
    dj_control_non_null(builder, );
    dj_graph_builder_first_rebuild(builder, &prebuilt_graph.m_graph, start, end);
}

void update_prebuilt_graph_on_obstacle_enabling(dj_obstacle_static_t *obstacle, bool enable)
{
    dj_control_non_null(obstacle, );

    // Force all the obstacles to be imported again in the obstacle manager
    dj_obstacle_manager_clear_obstacles(&prebuilt_graph.m_obstacle_manager);

    // Rebuild the graph with the obstacle
    if (enable)
    {
        // Rebuild the graph with the obstacle
        dj_graph_builder_build_with_obstacle(&prebuilt_graph, obstacle, NULL, true, true);
    }
    else
    {
        // Rebuild the graph without the obstacle
        dj_graph_builder_build_without_obstacle(&prebuilt_graph, obstacle, NULL, true);
    }
}

/* ******************************************* Public callback functions declarations ************************************ */
