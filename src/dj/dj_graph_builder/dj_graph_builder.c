/**
 * @file dj_graph_builder.c
 * @brief Graph builder main orchestration - simplified version
 * @author Cyprien Ménard
 * @date 12/2024 (Refactored 01/2025)
 * @see dj_graph_builder.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
#include "system/log/log.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_link_ops.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_node_ops.h"
#include "utils/dj/dj_logs/dj_time_marker.h"

LOG_REGISTER("utils/dj/graph_builder");

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions definitions ******************************************* */

void dj_graph_builder_init(dj_graph_builder_t *builder,
                           dj_obstacle_importer_t *obstacle_importer,
                           const polygon_t *restricted_workspace_polygon,
                           bool display)
{
    SYSTEM_ASSERT(builder != NULL);
    SYSTEM_ASSERT(obstacle_importer != NULL);

    dj_graph_graph_init(&builder->graph, display);
    dj_obstacle_manager_init(&builder->obstacle_manager, obstacle_importer);
    builder->restricted_workspace_polygon = restricted_workspace_polygon;
    builder->null_kinematics_detected = false;
    builder->null_kinematics_first_rebuild_done = false;
}

void dj_graph_builder_build_all(dj_graph_builder_t *builder,
                                dj_viewer_status_t *viewer_status,
                                bool is_prebuilt)
{
    SYSTEM_ASSERT(builder != NULL);

    // Build the nodes
    dj_mark_start_time(BUILD_GRAPH_NODES);
    dj_graph_builder_build_nodes(builder, viewer_status);
    dj_mark_end_time(BUILD_GRAPH_NODES);

    // Build the links
    dj_mark_start_time(BUILD_GRAPH_LINKS);
    dj_graph_builder_build_all_links(builder, viewer_status, is_prebuilt);
    dj_mark_end_time(BUILD_GRAPH_LINKS);

#ifdef DJ_ENABLE_SUPER_LOGGING
    dj_graph_graph_print(&builder->graph);
#endif
}

/* ******************************************* Public callback functions definitions ************************************* */
