/**
 * @file timeStm32f4.c
 * @brief Time management API implementation for stm32f4
 * @author Cyprien Ménard
 * @date 23/06/2025
 * @see timeStm32f4.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "time.h"
#include "stm32f4xx.h"      // For SystemCoreClock and device registers
#include "core_cm4.h"       // For DWT/CoreDebug (CMSIS for Cortex-M4)

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void time_init()
{
    // Enable TRC
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // Unlock DWT and enable the cycle counter
    // DWT->LAR = 0xC5ACCE55; // Unlock if present (optional, for Cortex-M7/M33)
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

time_us_t time_us()
{
    // Return microseconds since time_init()
    return (time_us_t)(DWT->CYCCNT / (SystemCoreClock / 1000000UL));
}

time_ms_t time_ms()
{
    // Return milliseconds since time_init()
    return (time_ms_t)(DWT->CYCCNT / (SystemCoreClock / 1000UL));
}

/* ***************************************** Public callback functions definitions *************************************** */
