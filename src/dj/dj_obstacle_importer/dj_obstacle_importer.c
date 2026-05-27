/**
 * @file dj_obstacle_importer.h
 * @brief Importer of dj obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obsacle_importer.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle_importer.h"
#include "hmi/strategy/hmi_strategy.h"
#include "hmi/strategy/hmi_strategy.pb.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "utils/dj/dj_obstacle/dj_obstacle_dynamic.h"
#include "utils/dj/dj_obstacle/dj_obstacle_static.h"
#include "utils/dj/dj_obstacle/dj_oversize_obstacle.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_id.h"
#include "utils/macros/macros.h"
#include "utils/shape/shape.h"
#include "utils/time/time.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

LOG_REGISTER("utils/dj/obstacle_importer");

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Size we consider that have adversarial robot
 * @note This size if from the center of the robot (our)
 * @note Unit : mm
 */
#define ADV_ROBOT_SIZE              (250)

/**
 * @brief Period to display obstacles (in ms)
 */
#define DISPLAY_OBSTACLES_PERIOD_MS (1000)

/* ************************************************ Private type definition ********************************************** */

static void dj_importer_static_init(dj_obstacle_importer_t *importer,
                                    dj_static_obstacle_id_t id,
                                    dj_polygon_t *polygon,
                                    bool is_enabled,
                                    dj_obstacle_oversize_mode_t mode,
                                    distance_t oversize_distance,
                                    distance_t smooth_extraction_radius);
static void dj_importer_dynamic_init(dj_obstacle_importer_t *importer,
                                     dj_dynamic_obstacle_id_t id,
                                     dj_polygon_t *polygon,
                                     dj_obstacle_oversize_mode_t mode,
                                     distance_t oversize_distance,
                                     bool is_enabled,
                                     dj_obstacle_dynamic_get_position_cb_t get_position,
                                     void *get_position_args);
static uint32_t dj_importer_get_static_obstacle_index(const dj_obstacle_importer_t *importer,
                                                      dj_static_obstacle_id_t id);
static uint32_t dj_importer_get_dynamic_obstacle_index(const dj_obstacle_importer_t *importer,
                                                       dj_dynamic_obstacle_id_t id);
static void dj_obstacle_display_callback(hmi_strategy_input_id_t input_id,
                                         hmi_strategy_event_type_t event_type,
                                         void *args);
static void dj_obstacle_display(dj_obstacle_importer_t *importer);

/* ********************************************* Private functions declarations ****************************************** */

MAGIC_ARRAY_DECLARATION(dynamic_obstacles_list, dj_obstacle_dynamic_t);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

MAGIC_ARRAY_DEFINITION(dynamic_obstacles_list, dj_obstacle_dynamic_t)

/**
 * @brief Generate a color based on an ID using HSV to RGB conversion
 * @param id The obstacle ID
 * @return RGB color as uint32_t (0xRRGGBB)
 */
static uint32_t dj_importer_generate_color(uint32_t id)
{
    // Use golden ratio to generate well-distributed hues
    const float golden_ratio = 0.618033988749895f;
    float hue = fmodf((float)id * golden_ratio, 1.0f);

    // Fixed saturation and value for vibrant colors
    float saturation = 0.8f;
    float value = 0.9f;

    // HSV to RGB conversion
    float h = hue * 6.0f;
    int i = (int)h;
    float f = h - (float)i;
    float p = value * (1.0f - saturation);
    float q = value * (1.0f - f * saturation);
    float t = value * (1.0f - (1.0f - f) * saturation);

    float r, g, b;
    switch (i % 6)
    {
        case 0:
            r = value;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = value;
            b = p;
            break;
        case 2:
            r = p;
            g = value;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = value;
            break;
        case 4:
            r = t;
            g = p;
            b = value;
            break;
        default:
            r = value;
            g = p;
            b = q;
            break;
    }

    uint8_t red = (uint8_t)(r * 255.0f);
    uint8_t green = (uint8_t)(g * 255.0f);
    uint8_t blue = (uint8_t)(b * 255.0f);

    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
}

/**
 * @brief Function to initialize an obstacle
 * @param importer Pointer to the obstacle importer structure
 * @param id ID of the obstacle
 * @param polygon Shape of the obstacle
 * @param is_enabled true if the obstacle is enabled, false otherwise
 * @param mode Oversize mode
 * @param oversize_distance Distance to oversize the obstacle
 * @param smooth_extraction_radius Radius for smooth extraction (mm)
 */
static void dj_importer_static_init(dj_obstacle_importer_t *importer,
                                    dj_static_obstacle_id_t id,
                                    dj_polygon_t *polygon,
                                    bool is_enabled,
                                    dj_obstacle_oversize_mode_t mode,
                                    distance_t oversize_distance,
                                    distance_t smooth_extraction_radius)
{
    SYSTEM_ASSERT(polygon != NULL);
    // Oversize the obstacle
    dj_oversize_obstacle(polygon, mode, oversize_distance);
    // Add the obstacle to the list
    dj_obstacle_static_t *new_obstacle =
        static_obstacles_list_add(&importer->static_obstacles, NULL);
    SYSTEM_ASSERT(new_obstacle != NULL);
    dj_obstacle_static_init(new_obstacle, polygon, id, smooth_extraction_radius, is_enabled);
    LOGD("Obstacle %d added, points : ", id);
    for (uint32_t i = 0; i < polygon->nb_points; i++)
    {
        LOGD("(%f, %f) ", polygon->points[i].x, polygon->points[i].y);
    }
    LOGD("");
}

/**
 * @brief Function to initialize an obstacle
 * @warning Please take a lot of margin for the dynamic obstacles
 * @param importer Pointer to the obstacle importer structure
 * @param id ID of the obstacle
 * @param polygon Shape of the obstacle
 * @param mode Oversize mode
 * @param oversize_distance Distance to oversize the obstacle
 * @param is_enabled true if the obstacle is enabled, false otherwise
 */
static void dj_importer_dynamic_init(dj_obstacle_importer_t *importer,
                                     dj_dynamic_obstacle_id_t id,
                                     dj_polygon_t *polygon,
                                     dj_obstacle_oversize_mode_t mode,
                                     distance_t oversize_distance,
                                     bool is_enabled,
                                     dj_obstacle_dynamic_get_position_cb_t get_position,
                                     void *get_position_args)
{
    SYSTEM_ASSERT(polygon != NULL);
    // Oversize the obstacle
    dj_oversize_obstacle(polygon, mode, oversize_distance);
    dj_obstacle_dynamic_t *new_obstacle =
        dynamic_obstacles_list_add(&importer->dynamic_obstacles, NULL);
    SYSTEM_ASSERT(new_obstacle != NULL);
    dj_obstacle_dynamic_init(new_obstacle,
                             get_position,
                             get_position_args,
                             polygon,
                             &(point_t){10000, 10000},
                             &(linear_speed_2d_vector_t){0, 0},
                             &(linear_acceleration_2d_vector_t){0, 0},
                             id,
                             is_enabled);
}

/**
 * @brief Function to get the index of a static obstacle in the list
 * @param id ID of the obstacle
 * @return Index of the obstacle in the list
 */
static uint32_t dj_importer_get_static_obstacle_index(const dj_obstacle_importer_t *importer,
                                                      dj_static_obstacle_id_t id)
{
    for (uint32_t i = 0; i < static_obstacles_list_size(&importer->static_obstacles); i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(&importer->static_obstacles, i);
        SYSTEM_ASSERT(obstacle != NULL);
        if (obstacle->id == id)
        {
            return i;
        }
    }
    return (uint32_t)-1;
}

/**
 * @brief Function to get the index of a dynamic obstacle in the list
 * @param importer Pointer to the obstacle importer structure
 * @param id ID of the obstacle
 * @return Index of the obstacle in the list
 */
static uint32_t dj_importer_get_dynamic_obstacle_index(const dj_obstacle_importer_t *importer,
                                                       dj_dynamic_obstacle_id_t id)
{
    for (uint32_t i = 0; i < dynamic_obstacles_list_size(&importer->dynamic_obstacles); i++)
    {
        dj_obstacle_dynamic_t *obstacle =
            dynamic_obstacles_list_get(&importer->dynamic_obstacles, i);
        SYSTEM_ASSERT(obstacle != NULL);
        if (obstacle->id == id)
        {
            return i;
        }
    }
    return (uint32_t)-1;
}

/**
 * @brief Callback function for the HMI strategy input event to display or hide static obstacles
 * @param input_id ID of the input that triggered the event
 * @param event_type Type of the event (typically ON or OFF)
 * @param args Pointer to the obstacle importer structure
 */
static void dj_obstacle_display_callback(hmi_strategy_input_id_t input_id,
                                         hmi_strategy_event_type_t event_type,
                                         void *args)
{
    UNUSED(input_id);

    SYSTEM_ASSERT(args != NULL);
    dj_obstacle_importer_t *importer = (dj_obstacle_importer_t *)args;

    bool display = (event_type == HMI_STRATEGY_EVENT_TYPE_ON);
    importer->is_displaying_obstacles = display;
    dj_obstacle_display(importer);
}

/**
 * @brief Function to display or hide static obstacles on the HMI
 * @param importer Pointer to the obstacle importer structure
 */
static void dj_obstacle_display(dj_obstacle_importer_t *importer)
{
    SYSTEM_ASSERT(importer != NULL);
    if (importer->is_displaying_obstacles)
    {
        LOGI("DJ Static Obstacles Display enabled");
        // Initialize shape handles
        for (uint32_t i = 0; i < DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES; i++)
        {
            if (importer->static_obstacle_shapes[i] == NULL)
            {
                importer->static_obstacle_shapes[i] = shape_new();
            }
        }

        for (uint32_t i = 0; i < static_obstacles_list_size(&importer->static_obstacles); i++)
        {
            dj_obstacle_static_t *obstacle =
                static_obstacles_list_get(&importer->static_obstacles, i);
            SYSTEM_ASSERT(obstacle != NULL);
            dj_polygon_t *polygon = &obstacle->shape;

            shape_handle_t shape = importer->static_obstacle_shapes[i];
            SYSTEM_ASSERT(shape != NULL);

            // Generate unique color for this obstacle based on its ID
            uint32_t color = dj_importer_generate_color((uint32_t)obstacle->id);
            // Black outline if enabled, same color if disabled
            uint32_t outline_color = obstacle->is_enabled ? 0x000000 : color;
            shape_set_outline(shape, outline_color, 5.0f);
            shape_set_fill(shape, color);                       // Colored fill
            float opacity = obstacle->is_enabled ? 0.4f : 0.1f; // More transparent if disabled
            shape_set_opacity(shape, opacity);

            // Set label
            char label[64];
            snprintf(label, sizeof(label), "Static %ld", (long)obstacle->id);
            shape_set_label(shape, label);

            // Draw polygon
            if (polygon->nb_points > 0)
            {
                shape_draw_polygon(shape, polygon->points, polygon->nb_points, true);
            }
        }
    }
    else
    {
        LOGD("DJ Static Obstacles Display disabled");
        // Clear shapes
        for (uint32_t i = 0; i < DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES; i++)
        {
            if (importer->static_obstacle_shapes[i] != NULL)
            {
                shape_free(importer->static_obstacle_shapes[i]);
                importer->static_obstacle_shapes[i] = NULL;
            }
        }
    }
}

/* *********************************************** Public functions declarations ***************************************** */

MAGIC_ARRAY_DEFINITION(static_obstacles_list, dj_obstacle_static_t);

void dj_obstacle_importer_init(dj_obstacle_importer_t *importer)
{
    SYSTEM_ASSERT(importer != NULL);
    importer->dynamic_obstacles_enabled = true;
    static_obstacles_list_init(&importer->static_obstacles);
    dynamic_obstacles_list_init(&importer->dynamic_obstacles);
    dj_obstacle_id_generator_init(&importer->id_generator);

    hmi_strategy_register_input_event_callback(
        HMI_STRATEGY_INPUT_ID_SWITCH_DJ_STATIC_OBSTACLES_DISPLAY,
        HMI_STRATEGY_EVENT_TYPE_ALL,
        dj_obstacle_display_callback,
        importer);
}

dj_static_obstacle_id_t dj_obstacle_importer_import_static_obstacle(
    dj_obstacle_importer_t *importer,
    dj_polygon_t *shape,
    bool is_enabled,
    dj_obstacle_oversize_mode_t mode,
    distance_t oversize_distance,
    distance_t smooth_extraction_radius)
{
    SYSTEM_ASSERT(importer != NULL);
    SYSTEM_ASSERT(shape != NULL);
    // Create a unique ID for the obstacle and import it
    dj_static_obstacle_id_t id =
        dj_obstacle_id_generate_static_obstacle_id(&importer->id_generator);
    dj_importer_static_init(
        importer, id, shape, is_enabled, mode, oversize_distance, smooth_extraction_radius);

    // Update the display of obstacles on the HMI
    dj_obstacle_display(importer);

    return id;
}

dj_dynamic_obstacle_id_t dj_obstacle_importer_import_dynamic_obstacle(
    dj_obstacle_importer_t *importer,
    dj_polygon_t *initial_shape,
    dj_obstacle_oversize_mode_t mode,
    distance_t oversize_distance,
    bool is_enabled,
    dj_obstacle_dynamic_get_position_cb_t get_position)
{
    SYSTEM_ASSERT(importer != NULL);
    SYSTEM_ASSERT(initial_shape != NULL);
    // Create a unique ID for the obstacle and import it
    dj_dynamic_obstacle_id_t id =
        dj_obstacle_id_generate_dynamic_obstacle_id(&importer->id_generator);
    dj_importer_dynamic_init(
        importer, id, initial_shape, mode, oversize_distance, is_enabled, get_position, NULL);
    return id;
}

void dj_obstacle_importer_set_dynamic_obstacle_position_args(dj_obstacle_importer_t *importer,
                                                             void *args)
{
    SYSTEM_ASSERT(importer != NULL);
    // Set the additional arguments for dynamic obstacle position calculation
    for (uint32_t i = 0; i < dynamic_obstacles_list_size(&importer->dynamic_obstacles); i++)
    {
        dj_obstacle_dynamic_t *obstacle =
            dynamic_obstacles_list_get(&importer->dynamic_obstacles, i);
        SYSTEM_ASSERT(obstacle != NULL);
        obstacle->get_position_args = args;
    }
}

void dj_obstacle_importer_refresh_dynamic_obstacle(dj_obstacle_importer_t *importer,
                                                   dj_dynamic_obstacle_id_t obstacle_id,
                                                   dj_polygon_t *shape,
                                                   linear_acceleration_2d_vector_t *acceleration,
                                                   linear_speed_2d_vector_t *initial_speed,
                                                   point_t *initial_position,
                                                   distance_t oversize_distance,
                                                   dj_obstacle_oversize_mode_t mode)
{
    SYSTEM_ASSERT(acceleration != NULL);
    SYSTEM_ASSERT(initial_speed != NULL);
    SYSTEM_ASSERT(initial_position != NULL);

    // Refresh the position of the dynamic obstacle
    if (obstacle_id < 0 ||
        obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count(&importer->id_generator))
    {
        LOGE("Invalid obstacle id");
        return;
    }
    dj_obstacle_dynamic_t *obstacle =
        dynamic_obstacles_list_get(&importer->dynamic_obstacles,
                                   dj_importer_get_dynamic_obstacle_index(importer, obstacle_id));
    SYSTEM_ASSERT(obstacle != NULL);
    // Oversize the obstacle
    dj_oversize_obstacle(shape, mode, oversize_distance);
    dj_obstacle_dynamic_refresh(obstacle, shape, acceleration, initial_speed, initial_position);
}

void dj_obstacle_importer_enable_static_obstacle(dj_obstacle_importer_t *importer,
                                                 dj_static_obstacle_id_t obstacle_id,
                                                 bool enable)
{
    SYSTEM_ASSERT(importer != NULL);
    // Enable or disable the obstacle with the id
    if (obstacle_id < 0 ||
        obstacle_id >= dj_obstacle_id_get_static_obstacle_count(&importer->id_generator))
    {
        LOGE("Invalid obstacle id");
        return;
    }
    dj_obstacle_static_t *obstacle =
        static_obstacles_list_get(&importer->static_obstacles,
                                  dj_importer_get_static_obstacle_index(importer, obstacle_id));
    SYSTEM_ASSERT(obstacle != NULL);
    obstacle->is_enabled = enable;

    // Update the display of obstacles on the HMI
    dj_obstacle_display(importer);
}

void dj_obstacle_importer_enable_dynamic_obstacle(dj_obstacle_importer_t *importer,
                                                  dj_dynamic_obstacle_id_t obstacle_id,
                                                  bool enable)
{
    SYSTEM_ASSERT(importer != NULL);
    // Enable or disable the obstacle with the id
    if (obstacle_id < 0 ||
        obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count(&importer->id_generator))
    {
        LOGE("Invalid obstacle id");
        return;
    }
    dj_obstacle_dynamic_t *obstacle =
        dynamic_obstacles_list_get(&importer->dynamic_obstacles,
                                   dj_importer_get_dynamic_obstacle_index(importer, obstacle_id));
    SYSTEM_ASSERT(obstacle != NULL);
    obstacle->is_enabled = enable;
}

bool dj_obstacle_importer_static_is_enabled(const dj_obstacle_importer_t *importer,
                                            dj_static_obstacle_id_t obstacle_id)
{
    SYSTEM_ASSERT(importer != NULL);
    // Check if the obstacle with the id is enabled
    if (obstacle_id < 0 ||
        obstacle_id >= dj_obstacle_id_get_static_obstacle_count(&importer->id_generator))
    {
        LOGE("Invalid obstacle id");
        return false;
    }
    dj_obstacle_static_t *obstacle =
        static_obstacles_list_get(&importer->static_obstacles,
                                  dj_importer_get_static_obstacle_index(importer, obstacle_id));
    SYSTEM_ASSERT(obstacle != NULL);
    return obstacle->is_enabled;
}

bool dj_obstacle_importer_dynamic_is_enabled(const dj_obstacle_importer_t *importer,
                                             dj_dynamic_obstacle_id_t obstacle_id)
{
    SYSTEM_ASSERT(importer != NULL);
    // Check if the obstacle with the id is enabled
    if (obstacle_id < 0 ||
        obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count(&importer->id_generator))
    {
        LOGE("Invalid obstacle id");
        return false;
    }
    dj_obstacle_dynamic_t *obstacle =
        dynamic_obstacles_list_get(&importer->dynamic_obstacles,
                                   dj_importer_get_dynamic_obstacle_index(importer, obstacle_id));
    SYSTEM_ASSERT(obstacle != NULL);
    return obstacle->is_enabled;
}

dj_obstacle_static_t *dj_obstacle_importer_get_static_obstacle(
    const dj_obstacle_importer_t *importer, dj_static_obstacle_id_t obstacle_id)
{
    // Get the static obstacle with the id
    if (obstacle_id < 0 ||
        obstacle_id >= dj_obstacle_id_get_static_obstacle_count(&importer->id_generator))
    {
        LOGE("Invalid obstacle id");
        return NULL;
    }

    uint32_t obstacle_index = dj_importer_get_static_obstacle_index(importer, obstacle_id);
    if (obstacle_index == (uint32_t)-1)
    {
        return NULL;
    }
    dj_obstacle_static_t *obstacle =
        static_obstacles_list_get(&importer->static_obstacles, obstacle_index);
    return obstacle;
}

dj_obstacle_dynamic_t *dj_obstacle_importer_get_dynamic_obstacle(
    const dj_obstacle_importer_t *importer, dj_dynamic_obstacle_id_t obstacle_id)
{
    // Get the dynamic obstacle with the id
    if (obstacle_id < 0 ||
        obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count(&importer->id_generator))
    {
        LOGE("Invalid obstacle id");
        return NULL;
    }
    if (!importer->dynamic_obstacles_enabled)
    {
        return NULL;
    }
    return dynamic_obstacles_list_get(&importer->dynamic_obstacles,
                                      dj_importer_get_dynamic_obstacle_index(importer,
                                                                             obstacle_id));
}

bool dj_obstacle_importer_all_dynamic_null_kinematics(const dj_obstacle_importer_t *importer)
{
    SYSTEM_ASSERT(importer != NULL);

    uint32_t nb_dynamic = dynamic_obstacles_list_size(&importer->dynamic_obstacles);
    for (uint32_t i = 0; i < nb_dynamic; i++)
    {
        const dj_obstacle_dynamic_t *obstacle =
            dynamic_obstacles_list_get(&importer->dynamic_obstacles, i);
        if (obstacle == NULL)
        {
            continue;
        }
        if (obstacle->initial_speed.x != 0.0f || obstacle->initial_speed.y != 0.0f ||
            obstacle->acceleration.x != 0.0f || obstacle->acceleration.y != 0.0f)
        {
            return false;
        }
    }
    return true;
}

void dj_obstacle_importer_enable_dynamic_obstacles(dj_obstacle_importer_t *importer, bool enable)
{
    SYSTEM_ASSERT(importer != NULL);
    // Enable or disable all the dynamic obstacles
    importer->dynamic_obstacles_enabled = enable;
}

/* ******************************************* Public callback functions declarations ************************************ */
