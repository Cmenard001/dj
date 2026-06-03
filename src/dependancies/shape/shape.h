/**
 * @file shape.h
 * @brief Shape display stub — no-op for the test build (no HMI)
 * @author Cyprien Ménard
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

#include "utils/maths/point/point.h"
#include "utils/maths/vector/vector.h"
#include <stdint.h>

/* Opaque handle — NULL in the test build */
typedef void *shape_handle_t;

/* ShapeArrowType enum stub */
typedef enum { ShapeArrowType_SHAPE_ARROW_NONE = 0 } ShapeArrowType;

/* All shape functions are no-ops in the test build */
static inline shape_handle_t shape_new(void)                                                                         { return NULL; }
static inline void shape_free(shape_handle_t h)                                                                      { (void)h; }
static inline void shape_set_outline(shape_handle_t h, uint32_t color, float width)                                  { (void)h; (void)color; (void)width; }
static inline void shape_set_fill(shape_handle_t h, uint32_t color)                                                  { (void)h; (void)color; }
static inline void shape_set_opacity(shape_handle_t h, float opacity)                                                { (void)h; (void)opacity; }
static inline void shape_set_label(shape_handle_t h, const char *label)                                              { (void)h; (void)label; }
static inline void shape_draw_polygon(shape_handle_t h, const void *points, uint32_t nb_points, int filled)          { (void)h; (void)points; (void)nb_points; (void)filled; }
static inline void shape_draw_circle(shape_handle_t h, point_t pos, float radius)                                    { (void)h; (void)pos; (void)radius; }
static inline void shape_draw_vector(shape_handle_t h, point_t pos, vector_2d_distance_t vec, ShapeArrowType arrow)  { (void)h; (void)pos; (void)vec; (void)arrow; }
