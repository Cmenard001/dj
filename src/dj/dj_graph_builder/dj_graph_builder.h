/**
 * @file dj_graph_builder.h
 * @brief Graph builder is used to build the graph from the obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_builder.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "system/assert/system_assert.h"
#include "utils/dj/dj_config.h"
#include "utils/dj/dj_graph/dj_graph_graph.h"
#include "utils/dj/dj_graph_builder/dj_graph_builder_precompute.h"
#include "utils/dj/dj_obstacle/dj_obstacle_manager.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_importer.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

typedef struct
{
    float min_x;
    float max_x;
    float min_y;
    float max_y;
    bool is_enabled;
} dj_extraction_bbox_t;

typedef struct
{
    dj_graph_graph_t graph;
    dj_obstacle_manager_t obstacle_manager;
    precomputed_polygon_t precomputed[DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES];
    dj_extraction_bbox_t extraction_bboxes[DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES];
    /**
     * @brief True if all dynamic obstacles had null speed and acceleration at the last
     *        first_rebuild call. Enables the fast path in dj_graph_builder_rebuild.
     */
    bool null_kinematics_detected;
    /**
     * @brief True once the first full rebuild has been executed after a first_rebuild call
     *        with null_kinematics_detected set. The fast path in rebuild is only used after
     *        this flag is set.
     */
    bool null_kinematics_first_rebuild_done;
    /**
     * @brief polygon to limit the workspace for graph building.
     */
    const polygon_t *restricted_workspace_polygon;
} dj_graph_builder_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize the graph builder
 * @param builder The graph builder to initialize
 * @param obstacle_importer The obstacle importer to use
 * @param restricted_workspace_polygon Polygon to limit the workspace for graph building
 * @param display If true, the built graph will be displayed
 */
void dj_graph_builder_init(dj_graph_builder_t *builder,
                           dj_obstacle_importer_t *obstacle_importer,
                           const polygon_t *restricted_workspace_polygon,
                           bool display);

/**
 * @brief Function to build the graph
 * @param builder The graph builder to build
 * @param viewer_status The status of the viewer (used to build dynamic polygons) (NULL if you want to a graph without dynamic obstacles)
 */
void dj_graph_builder_build_all(dj_graph_builder_t *builder,
                                dj_viewer_status_t *viewer_status,
                                bool is_prebuilt);

/**
 * @brief Function to link a node with all other nodes which do not intersect with an obstacle
 * @param builder Builder to build the links
 * @param node Node to link
 * @param obstacles List of all obstacles
 * @param is_prebuilt If the node is prebuilt
 * @param verbose If true, the function will print the links
 */
void dj_graph_builder_link_node(dj_graph_builder_t *builder,
                                dj_graph_node_t *node,
                                static_obstacles_list_t *obstacles,
                                bool is_prebuilt,
                                bool verbose);

/**
 * @brief Add a node to the graph with verification
 * The node will be added only if :
 * - it is not already in the graph
 * - it is not on an obstacle
 * - it is on the playground
 *
 * @param builder The graph builder
 * @param node The node to add
 * @param type The type of the node
 * @param viewer_status The status of the viewer (used to get dynamic obstacles from a point of view in space time) (NULL if you want to a graph without dynamic obstacles)
 * @return dj_graph_node_t* The node added
 * @return NULL if the node is not added
 */
dj_graph_node_t *dj_graph_builder_add_node_with_verif(dj_graph_builder_t *builder,
                                                      dj_graph_node_t *node,
                                                      dj_graph_node_type_t type,
                                                      dj_viewer_status_t *viewer_status);

/**
 * @brief Rebuild the graph considering a new obstacle
 * @param builder The graph builder
 * @param obstacle The obstacle to add
 * @param viewer_status The viewer status to build the graph
 * @param is_prebuilt If the nodes and links to add are prebuilt
 * @param add_obstacle_as_node If true, the obstacle will be added as a node in the graph
 */
void dj_graph_builder_build_with_obstacle(dj_graph_builder_t *builder,
                                          dj_obstacle_static_t *obstacle,
                                          dj_viewer_status_t *viewer_status,
                                          bool is_prebuilt,
                                          bool add_obstacle_as_node);

/* ******************************************* Public callback functions declarations ************************************ */
