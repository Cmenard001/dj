/**
 * @file dj_obstacle_importer.h
 * @brief Importer of dj obstacles
 * @author Cyprien MÃ©nard
 * @date 12/2024
 * @see dj_obsacle_importer.h
 */

/* ******************************************************* Includes ****************************************************** */

#include "dj_obstacle_importer.h"

#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_graph_builder/dj_prebuilt_graph.h"
#include "../dj_logs/dj_logs.h"
#include "../dj_obstacle/dj_obstacle_dynamic.h"
#include "../dj_obstacle/dj_obstacle_static.h"
#include "../dj_obstacle/dj_oversize_obstacle.h"
#include "../dj_obstacle/dj_polygon.h"
#include "dj_obstacle_id.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

#define START_END_AREA_SIZE (450)
#define PAMI_START_AREA_BIG_SIZE (450)
#define PAMI_START_AREA_SMALL_SIZE (150)
#define COLUMN_BIG_SIZE (400)
#define COLUMN_SMALL_SIZE (100)
#define BUILDING_AREA_BIG_SIZE (450)
#define BUILDING_AREA_SMALL_SIZE (150)
#define BUILDING_AREA_TOO_LITTLE_SIZE (350)

/**
 * @brief Size we consider that have adversarial robot
 * @note This size if from the center of the robot (our)
 * @note Unit : mm
 */
#define ADV_ROBOT_SIZE (250)

#define CREATE_START_END_AREA_OBSTACLE(x, y)                                                                                \
    (dj_polygon_t *)&DJ_CREATE_POLYGON({x, COLOR_Y(y)},                                                                     \
                                       {x, COLOR_Y(y + START_END_AREA_SIZE)},                                               \
                                       {x + START_END_AREA_SIZE, COLOR_Y(y + START_END_AREA_SIZE)},                         \
                                       {x + START_END_AREA_SIZE, COLOR_Y(y)})

#define CREATE_BUILDING_AREA_TOO_LITTLE_OBSTACLE(x, y)                                                                      \
    (dj_polygon_t *)&DJ_CREATE_POLYGON({x, COLOR_Y(y)},                                                                     \
                                       {x, COLOR_Y(y + START_END_AREA_SIZE)},                                               \
                                       {x + BUILDING_AREA_TOO_LITTLE_SIZE, COLOR_Y(y + START_END_AREA_SIZE)},               \
                                       {x + BUILDING_AREA_TOO_LITTLE_SIZE, COLOR_Y(y)})

#define CREATE_PAMI_START_AREA_OBSTACLE_VERTICAL(x, y)                                                                      \
    (dj_polygon_t *)&DJ_CREATE_POLYGON({x, COLOR_Y(y)},                                                                     \
                                       {x, COLOR_Y(y + PAMI_START_AREA_SMALL_SIZE)},                                        \
                                       {x + PAMI_START_AREA_BIG_SIZE, COLOR_Y(y + PAMI_START_AREA_SMALL_SIZE)},             \
                                       {x + PAMI_START_AREA_BIG_SIZE, COLOR_Y(y)})

#define CREATE_SMALL_BUILDING_AREA_OBSTACLE_HORIZONTAL(x, y)                                                                \
    (dj_polygon_t *)&DJ_CREATE_POLYGON({x, COLOR_Y(y)},                                                                     \
                                       {x, COLOR_Y(y + BUILDING_AREA_BIG_SIZE)},                                            \
                                       {x + BUILDING_AREA_SMALL_SIZE, COLOR_Y(y + BUILDING_AREA_BIG_SIZE)},                 \
                                       {x + BUILDING_AREA_SMALL_SIZE, COLOR_Y(y)})

#define CREATE_COLUMN_OBSTACLE_VERTICAL(x, y)                                                                               \
    (dj_polygon_t *)&DJ_CREATE_POLYGON({x, COLOR_Y(y)},                                                                     \
                                       {x, COLOR_Y(y + COLUMN_SMALL_SIZE)},                                                 \
                                       {x + COLUMN_BIG_SIZE, COLOR_Y(y + COLUMN_SMALL_SIZE)},                               \
                                       {x + COLUMN_BIG_SIZE, COLOR_Y(y)})

#define CREATE_COLUMN_OBSTACLE_HORIZONTAL(x, y)                                                                             \
    (dj_polygon_t *)&DJ_CREATE_POLYGON({x, COLOR_Y(y)},                                                                     \
                                       {x, COLOR_Y(y + COLUMN_BIG_SIZE)},                                                   \
                                       {x + COLUMN_SMALL_SIZE, COLOR_Y(y + COLUMN_BIG_SIZE)},                               \
                                       {x + COLUMN_SMALL_SIZE, COLOR_Y(y)})

/*
 * @brief Margin used when their is not enough space to oversize with DJ_DEFAULT_MARGIN margin
 */
#define SMALL_MARGIN (0)

/* ************************************************ Private type definition ********************************************** */

MAGIC_ARRAY_TYPEDEF(dynamic_obstacles_list, dj_obstacle_dynamic_t, 5);

static void dj_importer_static_init(
    dj_static_obstacle_id_e id, dj_polygon_t *polygon, bool is_enabled, dj_obsrtacle_oversize_mode_e mode, uint8_t margin);
static void dj_importer_dynamic_init(dj_dynamic_obstacle_id_e id, dj_polygon_t *polygon, bool is_enabled);
static uint32_t dj_importer_get_static_obstacle_index(dj_static_obstacle_id_e id);
static uint32_t dj_importer_get_dynamic_obstacle_index(dj_dynamic_obstacle_id_e id);

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/**
 * @brief List of static obstacles
 */
static static_obstacles_list_t static_obstacles;

/**
 * @brief List of dynamic obstacles
 */
static dynamic_obstacles_list_t dynamic_obstacles;

/**
 * @brief Enable or disable the dynamic obstacles
 * @see dj_obstacle_importer_enable_dynamic_obstacles
 */
static bool dynamic_obstacles_enabled = true;

/* ********************************************** Private functions definitions ****************************************** */

MAGIC_ARRAY_SRC(dynamic_obstacles_list, dj_obstacle_dynamic_t)

/**
 * @brief Function to initialize an obstacle
 *
 * @param id ID of the obstacle
 * @param polygon Shape of the obstacle
 * @param is_enabled true if the obstacle is enabled, false otherwise
 * @param mode Oversize mode
 * @param margin Margin to take into account in millimeters
 */
static void dj_importer_static_init(
    dj_static_obstacle_id_e id, dj_polygon_t *polygon, bool is_enabled, dj_obsrtacle_oversize_mode_e mode, uint8_t margin)
{
    dj_control_non_null(polygon, )
        // Oversize the obstacle
        dj_oversize_obstacle(polygon, mode, margin);
    // Add the obstacle to the list
    dj_obstacle_static_t *new_obstacle = static_obstacles_list_add(&static_obstacles, NULL);
    dj_control_non_null(new_obstacle, ) dj_obstacle_static_init(new_obstacle, polygon, id, is_enabled);
    dj_debug_printf("Obstacle %d added, points : ", id);
    for (uint8_t i = 0; i < polygon->nb_points; i++)
    {
        dj_debug_printf("(%d, %d) ", polygon->points[i].x, polygon->points[i].y);
    }
    dj_debug_printf("\n");
}

/**
 * @brief Function to initialize an obstacle
 * @warning Please take a lot of margin for the dynamic obstacles
 *
 * @param id ID of the obstacle
 * @param polygon Shape of the obstacle
 * @param is_enabled true if the obstacle is enabled, false otherwise
 */
static void dj_importer_dynamic_init(dj_dynamic_obstacle_id_e id, dj_polygon_t *polygon, bool is_enabled)
{
    dj_control_non_null(polygon, )
        // Oversize the obstacle
        dj_oversize_obstacle(polygon, DJ_OBSRTACLE_OVERSIZE_MODE_1_POINT, 0);
    dj_obstacle_dynamic_t *new_obstacle = dynamic_obstacles_list_add(&dynamic_obstacles, NULL);
    dj_control_non_null(new_obstacle, ) dj_obstacle_dynamic_init(new_obstacle,
                                                                 polygon,
                                                                 &(GEOMETRY_point_t){-10000, -10000},
                                                                 &(GEOMETRY_vector_t){0, 0},
                                                                 &(GEOMETRY_vector_t){0, 0},
                                                                 id,
                                                                 is_enabled);
}

/**
 * @brief Function to get the index of a static obstacle in the list
 *
 * @param id ID of the obstacle
 * @return Index of the obstacle in the list
 */
static uint32_t dj_importer_get_static_obstacle_index(dj_static_obstacle_id_e id)
{
    for (uint32_t i = 0; i < static_obstacles_list_size(&static_obstacles); i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(&static_obstacles, i);
        dj_control_non_null(obstacle, -1);
        if (obstacle->m_id == id)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Function to get the index of a dynamic obstacle in the list
 *
 * @param id ID of the obstacle
 * @return Index of the obstacle in the list
 */
static uint32_t dj_importer_get_dynamic_obstacle_index(dj_dynamic_obstacle_id_e id)
{
    for (uint32_t i = 0; i < dynamic_obstacles_list_size(&dynamic_obstacles); i++)
    {
        dj_obstacle_dynamic_t *obstacle = dynamic_obstacles_list_get(&dynamic_obstacles, i);
        dj_control_non_null(obstacle, -1);
        if (obstacle->m_id == id)
        {
            return i;
        }
    }
    return -1;
}

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_importer_init()
{
    dynamic_obstacles_enabled = true;
    static_obstacles_list_init(&static_obstacles);
    dynamic_obstacles_list_init(&dynamic_obstacles);
}

void dj_obstacle_import_default_obstacles()
{
    /*
        Initialisation des obstacles statiques
        Attention :
            L'ordre des points est important pour former un polygone fermÃ©

        Exemple d'initialisation d'un obstacle statique en forme de carrÃ© de 100mm de cÃ´tÃ© en (0, 0)
        dj_importer_static_init(STATIC_OBSTACLE_EXEMPLE,
                &DJ_CREATE_POLYGON(
                    {0   , COLOR_Y(0  )},     // Points 1 of the polygon
                    {0   , COLOR_Y(100)},     // Points 2 of the polygon
                    {100 , COLOR_Y(100)},     // Points 3 of the polygon
                    {100 , COLOR_Y(0  )}),    // Points 4 of the polygon
                true,                         // Obstacle enabled
                DJ_DEFAULT_OVERSIZE_MODE,     // Oversize mode
                DJ_DEFAULT_MARGIN);           // Margin
    */

    /*
        Start / end area + building area
        1) Big square :
            Start / end area
            Building area 1 & 2
        2) Small rectangle :
            Building area 3 & 4
    */
    dj_importer_static_init(STATIC_OBSTACLE_START_END_AREA_ADV,
                            CREATE_START_END_AREA_OBSTACLE(0, 2400),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);

    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_OUR_1,
                            CREATE_BUILDING_AREA_TOO_LITTLE_OBSTACLE(900, 2550),
                            false,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            SMALL_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_ADV_1,
                            CREATE_BUILDING_AREA_TOO_LITTLE_OBSTACLE(900, 0),
                            false,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            SMALL_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_OUR_2,
                            CREATE_START_END_AREA_OBSTACLE(1550, 1000),
                            false,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            SMALL_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_ADV_2,
                            CREATE_START_END_AREA_OBSTACLE(1550, 1550),
                            false,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            SMALL_MARGIN);

    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_OUR_3,
                            CREATE_SMALL_BUILDING_AREA_OBSTACLE_HORIZONTAL(1850, 550),
                            false,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_ADV_3,
                            CREATE_SMALL_BUILDING_AREA_OBSTACLE_HORIZONTAL(1850, 2005),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_OUR_4,
                            CREATE_SMALL_BUILDING_AREA_OBSTACLE_HORIZONTAL(1850, 2550),
                            false,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_BUILDING_AREA_ADV_4,
                            CREATE_SMALL_BUILDING_AREA_OBSTACLE_HORIZONTAL(1850, 0),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);

    /* Obstacles columns storage */
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_1,
                            CREATE_COLUMN_OBSTACLE_VERTICAL(475, 25),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_2,
                            CREATE_COLUMN_OBSTACLE_VERTICAL(1400, 25),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_3,
                            CREATE_COLUMN_OBSTACLE_HORIZONTAL(225, 625),
                            true,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_4,
                            CREATE_COLUMN_OBSTACLE_HORIZONTAL(1700, 575),
                            true,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_5,
                            CREATE_COLUMN_OBSTACLE_HORIZONTAL(1000, 900),
                            true,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_6,
                            CREATE_COLUMN_OBSTACLE_HORIZONTAL(1000, 1700),
                            true,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_7,
                            CREATE_COLUMN_OBSTACLE_HORIZONTAL(225, 1975),
                            true,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_8,
                            CREATE_COLUMN_OBSTACLE_HORIZONTAL(1700, 2025),
                            true,
                            DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_9,
                            CREATE_COLUMN_OBSTACLE_VERTICAL(475, 2875),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_COLUMN_10,
                            CREATE_COLUMN_OBSTACLE_VERTICAL(1400, 2875),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);

    /* Obstacles PAMI start area */
    dj_importer_static_init(STATIC_OBSTACLE_PAMI_START_AREA_OUR,
                            CREATE_PAMI_START_AREA_OBSTACLE_VERTICAL(0, 0),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);
    dj_importer_static_init(STATIC_OBSTACLE_PAMI_START_AREA_ADV,
                            CREATE_PAMI_START_AREA_OBSTACLE_VERTICAL(0, 2850),
                            true,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);

    /* Obstacle platform */
    dj_importer_static_init(
        STATIC_OBSTACLE_PLATFORM,
        (dj_polygon_t *)&DJ_CREATE_POLYGON(
            {0, 650}, {200, 650}, {200, 1050}, {450, 1050}, {450, 1950}, {200, 1950}, {200, 2350}, {0, 2350}),
        true,
        DJ_DEFAULT_OVERSIZE_MODE,
        DJ_DEFAULT_MARGIN);

    /* Obstacles PAMI trajectories areas */
    dj_importer_static_init(
        STATIC_OBSTACLE_PAMI_START_TRAJECTORY_AREA,
        (dj_polygon_t *)&DJ_CREATE_POLYGON({0, COLOR_Y(0)}, {0, COLOR_Y(800)}, {600, COLOR_Y(800)}, {600, COLOR_Y(0)}),
        false,
        DJ_DEFAULT_OVERSIZE_MODE,
        DJ_DEFAULT_MARGIN);

    dj_importer_static_init(STATIC_OBSTACLE_PAMI_END_TRAJECTORY_AREA,
                            (dj_polygon_t *)&DJ_CREATE_POLYGON({0, COLOR_Y(1000)},
                                                               {0, COLOR_Y(3000)},
                                                               {900, COLOR_Y(3000)},
                                                               {900, COLOR_Y(2550)},
                                                               {1000, COLOR_Y(2100)},
                                                               {1000, COLOR_Y(1000)}),
                            false,
                            DJ_DEFAULT_OVERSIZE_MODE,
                            DJ_DEFAULT_MARGIN);

    /*
        Initialisation des obstacles dynamiques
        Attention :
            L'ordre des points est important pour former un polygone fermÃ©
            !!! Le polygon doit Ãªtre centrÃ© sur le point (0, 0) au mieux en (0, 0) !!!
            (Ã§a ne fonctionnera pas du tout sinon)

        Exemple d'initialisation d'un obstacle dynamique en forme de carrÃ© de 100mm de cÃ´tÃ©
        dj_importer_dynamic_init(DYNAMIC_OBSTACLE_EXEMPLE,
                &DJ_CREATE_POLYGON(
                    {-50, -50},   // Points 1 of the polygon
                    {-50,  50},   // Points 2 of the polygon
                    { 50,  50},   // Points 3 of the polygon
                    { 50, -50}),  // Points 4 of the polygon
                true);            // Obstacle enabled
    */

    // dj_importer_dynamic_init(DYNAMIC_OBSTACLE_ADV_ROBOT,
    //                          (dj_polygon_t *)&DJ_CREATE_POLYGON({-ADV_ROBOT_SIZE, -ADV_ROBOT_SIZE},
    //                                                             {-ADV_ROBOT_SIZE, ADV_ROBOT_SIZE},
    //                                                             {ADV_ROBOT_SIZE, ADV_ROBOT_SIZE},
    //                                                             {ADV_ROBOT_SIZE, -ADV_ROBOT_SIZE}),
    //                          true);
}

void dj_obstacle_importer_deinit()
{
    // Deinitialize the obstacles
    for (uint32_t i = 0; i < static_obstacles_list_size(&static_obstacles); i++)
    {
        dj_obstacle_static_t *obstacle = static_obstacles_list_get(&static_obstacles, i);
        dj_control_non_null(obstacle, ) dj_obstacle_static_deinit(obstacle);
    }
    for (uint32_t i = 0; i < dynamic_obstacles_list_size(&dynamic_obstacles); i++)
    {
        dj_obstacle_dynamic_t *obstacle
            = dynamic_obstacles_list_get(&dynamic_obstacles, dj_importer_get_dynamic_obstacle_index(i));
        dj_control_non_null(obstacle, ) dj_obstacle_dynamic_deinit(obstacle);
    }
}

dj_static_obstacle_id_e dj_obstacle_importer_import_static_obstacle(dj_polygon_t *shape,
                                                                    bool is_enabled,
                                                                    dj_obsrtacle_oversize_mode_e mode,
                                                                    uint8_t margin)
{
    dj_control_non_null(shape, -1)
        // Create a unique ID for the obstacle and import it
        dj_static_obstacle_id_e id
        = dj_obstacle_id_generate_static_obstacle_id();
    dj_importer_static_init(id, shape, is_enabled, mode, margin);
    // Recalculate the prebuilt graph
    dj_prebuilt_graph_deinit();
    dj_prebuilt_graph_init();
    return id;
}

dj_dynamic_obstacle_id_e dj_obstacle_importer_import_dynamic_obstacle(dj_polygon_t *initial_shape, bool is_enabled)
{
    dj_control_non_null(initial_shape, -1)
        // Create a unique ID for the obstacle and import it
        dj_dynamic_obstacle_id_e id
        = dj_obstacle_id_generate_dynamic_obstacle_id();
    dj_importer_dynamic_init(id, initial_shape, is_enabled);
    return id;
}

void dj_obstacle_importer_refresh_dynamic_obstacle(dj_dynamic_obstacle_id_e obstacle_id,
                                                   GEOMETRY_vector_t *acceleration,
                                                   GEOMETRY_vector_t *initial_speed,
                                                   GEOMETRY_point_t *initial_position)
{
    dj_control_non_null(acceleration, ) dj_control_non_null(initial_speed, ) dj_control_non_null(initial_position, )

        // Refresh the position of the dynamic obstacle
        if (obstacle_id < 0 || obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count())
    {
        dj_error_printf("Invalid obstacle id");
        return;
    }
    dj_obstacle_dynamic_t *obstacle
        = dynamic_obstacles_list_get(&dynamic_obstacles, dj_importer_get_dynamic_obstacle_index(obstacle_id));
    dj_control_non_null(obstacle, ) dj_obstacle_dynamic_refresh(obstacle, acceleration, initial_speed, initial_position);
}

void dj_obstacle_importer_enable_static_obstacle(dj_static_obstacle_id_e obstacle_id, bool enable)
{
    // Enable or disable the obstacle with the id
    if (obstacle_id < 0 || obstacle_id >= dj_obstacle_id_get_static_obstacle_count())
    {
        dj_error_printf("Invalid obstacle id");
        return;
    }
    dj_obstacle_static_t *obstacle
        = static_obstacles_list_get(&static_obstacles, dj_importer_get_static_obstacle_index(obstacle_id));
    dj_control_non_null(obstacle, ) obstacle->m_is_enabled = enable;
}

void dj_obstacle_importer_enable_dynamic_obstacle(dj_dynamic_obstacle_id_e obstacle_id, bool enable)
{
    // Enable or disable the obstacle with the id
    if (obstacle_id < 0 || obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count())
    {
        dj_error_printf("Invalid obstacle id");
        return;
    }
    dj_obstacle_dynamic_t *obstacle
        = dynamic_obstacles_list_get(&dynamic_obstacles, dj_importer_get_dynamic_obstacle_index(obstacle_id));
    dj_control_non_null(obstacle, ) obstacle->m_is_enabled = enable;
}

bool dj_obstacle_importer_static_is_enabled(dj_static_obstacle_id_e obstacle_id)
{
    // Check if the obstacle with the id is enabled
    if (obstacle_id < 0 || obstacle_id >= dj_obstacle_id_get_static_obstacle_count())
    {
        dj_error_printf("Invalid obstacle id");
        return false;
    }
    dj_obstacle_static_t *obstacle
        = static_obstacles_list_get(&static_obstacles, dj_importer_get_static_obstacle_index(obstacle_id));
    dj_control_non_null(obstacle, false) return obstacle->m_is_enabled;
}

bool dj_obstacle_importer_dynamic_is_enabled(dj_dynamic_obstacle_id_e obstacle_id)
{
    // Check if the obstacle with the id is enabled
    if (obstacle_id < 0 || obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count())
    {
        dj_error_printf("Invalid obstacle id");
        return false;
    }
    dj_obstacle_dynamic_t *obstacle
        = dynamic_obstacles_list_get(&dynamic_obstacles, dj_importer_get_dynamic_obstacle_index(obstacle_id));
    dj_control_non_null(obstacle, false) return obstacle->m_is_enabled;
}

dj_obstacle_static_t *dj_obstacle_importer_get_static_obstacle(dj_static_obstacle_id_e obstacle_id)
{
    // Get the static obstacle with the id
    if (obstacle_id < 0 || obstacle_id >= dj_obstacle_id_get_static_obstacle_count())
    {
        dj_error_printf("Invalid obstacle id");
        return NULL;
    }
    dj_obstacle_static_t *obstacle
        = static_obstacles_list_get(&static_obstacles, dj_importer_get_static_obstacle_index(obstacle_id));
    dj_control_non_null(obstacle, NULL) return obstacle;
}

dj_obstacle_dynamic_t *dj_obstacle_importer_get_dynamic_obstacle(dj_dynamic_obstacle_id_e obstacle_id)
{
    // Get the dynamic obstacle with the id
    if (obstacle_id < 0 || obstacle_id >= dj_obstacle_id_get_dynamic_obstacle_count())
    {
        dj_error_printf("Invalid obstacle id");
        return NULL;
    }
    if (!dynamic_obstacles_enabled)
    {
        return NULL;
    }
    return dynamic_obstacles_list_get(&dynamic_obstacles, dj_importer_get_dynamic_obstacle_index(obstacle_id));
}

void dj_obstacle_importer_enable_dynamic_obstacles(bool enable)
{
    // Enable or disable all the dynamic obstacles
    dynamic_obstacles_enabled = enable;
}

/* ******************************************* Public callback functions declarations ************************************ */
