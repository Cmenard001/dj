/**
 * @file dj_tester.c
 * @brief Tester for the DJ module — multi-suite orchestration with results file
 * @author Cyprien Ménard
 * @date 27/05/2026
 * @see dj_tester.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_tester.h"
#include "dj_tester/dj_test_params/dj_test_params.h"
#include "dj_tester/dj_test_report/dj_test_report.h"
#include "dj_tester/dj_test_set/dj_test_set.h"
#include "dj_tester/dj_test_suite/dj_test_suite.h"
#include "utils/dj/dj_obstacle/dj_oversize_obstacle.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

/* **************************************************** Private macros *************************************************** */

#define DJ_HW_NAME_MAX_LEN (64)
#define DJ_RESULTS_DIR     "results"

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

static void get_hw_name(char *buf, size_t buf_len);
static void build_result_filename(const char *hw_name, char *buf, size_t buf_len);

/* ************************************************** Private variables ************************************************** */

/**
 * @brief Test suites — edit here to add/remove suites and obstacles
 */
static const dj_test_suite_config_t g_suites[] = {
    {
        .name             = "Playground 2025",
        .nb_tests         = 50,
        .playground_min_x = 0,
        .playground_max_x = 2000,
        .playground_min_y = 0,
        .playground_max_y = 3000,
        .nb_obstacles     = 10,
        .obstacles        = {
            {
                /* Plateforme (gauche) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 0,   .y = 650},
                    {.x = 200, .y = 650},
                    {.x = 200, .y = 1050},
                    {.x = 450, .y = 1050},
                    {.x = 450, .y = 1950},
                    {.x = 200, .y = 1950},
                    {.x = 200, .y = 2350},
                    {.x = 0,   .y = 2350}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_DEFAULT_OVERSIZE_MODE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Zone départ/arrivée adversaire (coin haut-gauche) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 0,   .y = 2400},
                    {.x = 0,   .y = 2850},
                    {.x = 450, .y = 2850},
                    {.x = 450, .y = 2400}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_DEFAULT_OVERSIZE_MODE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Zone construction adversaire 3 (droite, haut) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1850, .y = 2005},
                    {.x = 1850, .y = 2455},
                    {.x = 2000, .y = 2455},
                    {.x = 2000, .y = 2005}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_DEFAULT_OVERSIZE_MODE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Zone construction adversaire 4 (droite, bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1850, .y = 0},
                    {.x = 1850, .y = 450},
                    {.x = 2000, .y = 450},
                    {.x = 2000, .y = 0}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_DEFAULT_OVERSIZE_MODE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Colonne 3 (gauche, milieu bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 225, .y = 625},
                    {.x = 225, .y = 1025},
                    {.x = 325, .y = 1025},
                    {.x = 325, .y = 625}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Colonne 4 (droite, milieu bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1700, .y = 575},
                    {.x = 1700, .y = 975},
                    {.x = 1800, .y = 975},
                    {.x = 1800, .y = 575}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Colonne 5 (centre, bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1000, .y = 900},
                    {.x = 1000, .y = 1300},
                    {.x = 1100, .y = 1300},
                    {.x = 1100, .y = 900}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Colonne 6 (centre, haut) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1000, .y = 1700},
                    {.x = 1000, .y = 2100},
                    {.x = 1100, .y = 2100},
                    {.x = 1100, .y = 1700}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Colonne 7 (gauche, milieu haut) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 225, .y = 1975},
                    {.x = 225, .y = 2375},
                    {.x = 325, .y = 2375},
                    {.x = 325, .y = 1975}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
            {
                /* Colonne 8 (droite, milieu haut) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1700, .y = 2025},
                    {.x = 1700, .y = 2425},
                    {.x = 1800, .y = 2425},
                    {.x = 1800, .y = 2025}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 50.0f,
            },
        },
    },
    {
        .name             = "Playground 2026",
        .nb_tests         = 50,
        .playground_min_x = 0,
        .playground_max_x = 2000,
        .playground_min_y = 0,
        .playground_max_y = 3000,
        .nb_obstacles     = 10,
        .obstacles        = {
            {
                /* Zone départ adversaire (coin haut-gauche) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 0,   .y = 2400},
                    {.x = 0,   .y = 3000},
                    {.x = 450, .y = 3000},
                    {.x = 450, .y = 2400}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 2 (gauche, milieu bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 450, .y = 1150},
                    {.x = 450, .y = 1350},
                    {.x = 650, .y = 1350},
                    {.x = 650, .y = 1150}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 3 (gauche, milieu haut) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 450, .y = 1650},
                    {.x = 450, .y = 1850},
                    {.x = 650, .y = 1850},
                    {.x = 650, .y = 1650}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 5 (centre, bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1100, .y = 700},
                    {.x = 1100, .y = 900},
                    {.x = 1300, .y = 900},
                    {.x = 1300, .y = 700}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 6 (centre, milieu) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1100, .y = 1400},
                    {.x = 1100, .y = 1600},
                    {.x = 1300, .y = 1600},
                    {.x = 1300, .y = 1400}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 7 (centre, haut) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1100, .y = 2100},
                    {.x = 1100, .y = 2300},
                    {.x = 1300, .y = 2300},
                    {.x = 1300, .y = 2100}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 9 (droite, bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1750, .y = 600},
                    {.x = 1750, .y = 800},
                    {.x = 2000, .y = 800},
                    {.x = 2000, .y = 600}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 10 (droite, milieu) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1750, .y = 1400},
                    {.x = 1750, .y = 1600},
                    {.x = 2000, .y = 1600},
                    {.x = 2000, .y = 1400}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Dispose 11 (droite, haut) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1750, .y = 2200},
                    {.x = 1750, .y = 2400},
                    {.x = 2000, .y = 2400},
                    {.x = 2000, .y = 2200}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
            {
                /* Storage 5 (centre, milieu bas) */
                .polygon = DJ_CREATE_POLYGON(
                    {.x = 1125, .y = 1050},
                    {.x = 1125, .y = 1250},
                    {.x = 1275, .y = 1250},
                    {.x = 1275, .y = 1050}
                ),
                .is_enabled               = true,
                .oversize_mode            = DJ_OBSTACLE_OVERSIZE_MODE_2_POINTS,
                .margin                   = 10.0f,
                .smooth_extraction_radius = 100.0f,
            },
        },
    },
};

#define DJ_TESTER_NB_SUITES ((uint32_t)(sizeof(g_suites) / sizeof(g_suites[0])))

/* ********************************************** Private functions definitions ****************************************** */

static void get_hw_name(char *buf, size_t buf_len)
{
#ifdef __linux__
    /* On Linux we can prompt interactively */
    printf("Hardware name (press Enter to use \"%s\"): ", DJ_TARGET_NAME);
    fflush(stdout);
    if (fgets(buf, (int)buf_len, stdin) != NULL)
    {
        /* Strip trailing newline */
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
        {
            buf[len - 1] = '\0';
            len--;
        }
        if (len > 0)
        {
            return; /* user typed a name */
        }
    }
    /* Empty input — fall through to default */
#endif
    /* Non-Linux or empty input: use the CMake TARGET_NAME */
    strncpy(buf, DJ_TARGET_NAME, buf_len - 1);
    buf[buf_len - 1] = '\0';
}

static void build_result_filename(const char *hw_name, char *buf, size_t buf_len)
{
    time_t    now = time(NULL);
    struct tm *t  = localtime(&now);
    char date_str[16];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", t);
    snprintf(buf, buf_len, "%s/%s_%s.txt", DJ_RESULTS_DIR, date_str, hw_name);
}

/* ********************************************** Public functions definitions ******************************************* */

void dj_tester_launch_tests()
{
    /* Get hardware name */
    char hw_name[DJ_HW_NAME_MAX_LEN];
    get_hw_name(hw_name, sizeof(hw_name));

    printf("=== DJ Pathfinding Benchmark — Hardware: %s ===\n\n", hw_name);

    /* Global report accumulates all suite sections */
    dj_test_report_t global_report;
    dj_test_report_init(&global_report);

    char header_buf[128];
    snprintf(header_buf, sizeof(header_buf),
             "DJ Pathfinding Benchmark\nHardware: %s\n\n", hw_name);
    dj_test_report_add_message(&global_report, header_buf);

    uint32_t total_tests   = 0;
    uint32_t total_success = 0;

    for (uint32_t s = 0; s < DJ_TESTER_NB_SUITES; s++)
    {
        const dj_test_suite_config_t *suite = &g_suites[s];
        printf("[Suite %u/%u] %s — %u tests\n",
               s + 1, DJ_TESTER_NB_SUITES, suite->name, suite->nb_tests);

        dj_test_set_t test_set;
        dj_test_suite_run(suite, &test_set);

        uint32_t  success_count = dj_test_set_get_success_count(&test_set);
        time_ms_t wall_ms       = dj_test_set_get_duration(&test_set);
        time_us_t cpu_us        = dj_test_set_get_cpu_duration(&test_set);

        total_tests   += test_set.m_num_tests;
        total_success += success_count;

        /* Print suite summary to console */
        printf("  Success : %u/%u\n", success_count, test_set.m_num_tests);
        printf("  Wall    : %ld ms\n", (long)wall_ms);
        printf("  CPU     : %ld us\n\n", (long)cpu_us);

        /* Append suite section to the global report */
        char section_buf[256];
        snprintf(section_buf, sizeof(section_buf),
                 "--- Suite: %s ---\n"
                 "Tests     : %u\n"
                 "Success   : %u/%u\n"
                 "Wall time : %ld ms\n"
                 "CPU time  : %ld us\n\n",
                 suite->name,
                 test_set.m_num_tests,
                 success_count, test_set.m_num_tests,
                 (long)wall_ms,
                 (long)cpu_us);
        dj_test_report_add_message(&global_report, section_buf);
    }

    /* Totals */
    char totals_buf[128];
    snprintf(totals_buf, sizeof(totals_buf),
             "=== TOTAL: %u/%u ===\n", total_success, total_tests);
    dj_test_report_add_message(&global_report, totals_buf);
    printf("%s\n", totals_buf);

    /* Print full report to console */
    dj_test_report_print(&global_report);

    /* Save to results/<date>_<hw_name>.txt */
    char result_filename[128];
    build_result_filename(hw_name, result_filename, sizeof(result_filename));
    dj_test_report_print_to_file(&global_report, result_filename);
    printf("Report saved to: %s\n", result_filename);
}

/* ***************************************** Public callback functions definitions *************************************** */
