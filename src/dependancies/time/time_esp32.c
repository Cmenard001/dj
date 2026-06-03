/**
 * @file time_esp32.c
 * @brief Time management
 * @author Cyprien Ménard
 * @date 13/10/2025
 * @see time.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "drivers/esp32/driver_esp32.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

inline void time_init(void)
{
    // Nothing to do for ESP32
}

inline time_ms_t time_get_absolute_time_ms(void)
{
    return (time_ms_t)xTaskGetTickCount();
}

inline time_us_t time_get_absolute_time_us(void)
{
    return (time_us_t)esp_timer_get_time();
}

/* ***************************************** Public callback functions definitions *************************************** */
