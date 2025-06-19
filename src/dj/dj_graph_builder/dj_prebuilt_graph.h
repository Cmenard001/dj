/**
 * @file dj_prebuilt_graph.h
 * @brief Prebuilt graph is a graph that is built before the match and is used to build the graph faster
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_prebuilt_graph.c
 */

#ifndef __DJ_PREBUILT_GRAPH_H__
#define __DJ_PREBUILT_GRAPH_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj_graph_builder.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize the prebuilt graph
 * @post The prebuilt graph will be built and ready to be used
 * @warning This function must be called at start of match (when color is known)
 */
void dj_prebuilt_graph_init();

/**
 * @brief Function to deinitialize the prebuilt graph
 * @post The prebuilt graph will be destroyed
 * @warning This function can be called only if the prebuilt graph has been initialized
 */
void dj_prebuilt_graph_deinit();

/**
 * @brief Function to rebuild the graph
 *
 * @param builder The graph builder to rebuild
 * @param viewer_status The status of the viewer (used to build dynamic polygons)
 * @note This function is used to rebuild the graph when the viewer moves
 * @note It is more efficient to use this function than to destroy and recreate the graph
 * @pre The graph must be rebuilt with dj_graph_builder_first_rebuild
 * @post The graph will be update from the point of view of the viewer (in space time)
 */
void rebuild_graph_with_prebuilt_graph(dj_graph_builder_t *graph_builder, dj_viewer_status_t *viewer);

/**
 * @brief Function to rebuild the graph (the first time)
 *
 * @param builder The graph builder to rebuild
 * @param start The start point of the graph
 * @param end The end point of the graph
 * @note This function is used to rebuild the graph at launch of new dj order
 * @note It is more efficient to use this function than to destroy and recreate the graph
 * @post The graph will be built from the prebuilt graph with the start and end nodes
 */
void first_rebuild_graph_with_prebuilt_graph(dj_graph_builder_t *builder, GEOMETRY_point_t start, GEOMETRY_point_t end);

/**
 * @brief Function to update the prebuilt graph when a static obstacle is enabled or disabled
 *
 * @param obstacle The obstacle to update
 * @param enable If true, the obstacle is enabled, else it is disabled
 * @note This function is used to update the prebuilt graph when a static obstacle is enabled or disabled
 */
void update_prebuilt_graph_on_obstacle_enabling(dj_obstacle_static_t *obstacle, bool enable);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
