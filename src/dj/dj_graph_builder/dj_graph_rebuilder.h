/**
 * @file dj_graph_rebuilder.h
 * @brief Rebuilder is used to rebuild the graph when the viewer moves
 * @see dj_graph_builder.h
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_graph_rebuilder.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_graph_builder/dj_graph_builder.h"
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to rebuild the graph
 * @param builder The graph builder to rebuild
 * @param prebuilt_graph The prebuilt graph to use
 * @param viewer_status The status of the viewer (used to build dynamic polygons)
 * @note This function is used to rebuild the graph when the viewer moves
 * @note It is more efficient to use this function than to destroy and recreate the graph
 * @pre The graph must be rebuilt with dj_graph_builder_first_rebuild
 * @post The graph will be update from the point of view of the viewer (in space time)
 */
void dj_graph_builder_rebuild(dj_graph_builder_t *builder,
                              dj_graph_graph_t *prebuilt_graph,
                              dj_viewer_status_t *viewer_status);

/**
 * @brief Function to rebuild the graph (the first time)
 * @param builder The graph builder to rebuild
 * @param prebuilt_graph The prebuilt graph to use
 * @param start The start point of the graph
 * @param ends Array of end points
 * @param end_count Number of end points
 * @note This function is used to rebuild the graph at launch of new dj order
 * @note It is more efficient to use this function than to destroy and recreate the graph
 * @post The graph will be built from the prebuilt graph with the start and all end nodes
 */
void dj_graph_builder_first_rebuild(dj_graph_builder_t *builder,
                                    dj_graph_graph_t *prebuilt_graph,
                                    const point_t *start,
                                    const point_t *ends,
                                    uint32_t end_count);

/* ******************************************* Public callback functions declarations ************************************ */
