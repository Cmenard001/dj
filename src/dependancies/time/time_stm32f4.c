/**
 * @file time_stm32f4.c
 * @brief Time management
 * @author Cyprien Ménard
 * @date 13/10/2025
 * @see time.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "drivers/stm32f4/hal.h"
#include "system/assert/system_assert.h"
#include "time.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */
extern TIM_HandleTypeDef htim10;
static TIM_HandleTypeDef *us_timer_handle = &htim10; // Timer used for microsecond timing

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

inline void time_init(void)
{
    // Nothing to do for STM32F4
    if (HAL_TIM_Base_Start(us_timer_handle) != HAL_OK)
    {
        SYSTEM_ASSERT(false);
    }
}

inline time_ms_t time_get_absolute_time_ms(void)
{
    return (time_ms_t)HAL_GetTick();
}

inline time_us_t time_get_absolute_time_us(void)
{
    return (time_us_t)(HAL_GetTick() * 1000 + us_timer_handle->Instance->CNT);
}

/* ***************************************** Public callback functions definitions *************************************** */
