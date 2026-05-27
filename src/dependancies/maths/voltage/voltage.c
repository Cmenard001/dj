/**
 * @file voltage.c
 * @brief Voltage type definition implementation
 * @author Dorian Benech
 * @date 08/12/2025
 * @see voltage.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "voltage.h"
#include "system/assert/system_assert.h"
#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

inline voltage_t voltage_from_adc_value(uint32_t adc_value,
                                        voltage_t reference_voltage,
                                        uint32_t adc_resolution)
{
    SYSTEM_ASSERT(adc_resolution > 0);
    SYSTEM_ASSERT(adc_value < adc_resolution);
    SYSTEM_ASSERT(reference_voltage >= 0.0f);
    return (reference_voltage * (voltage_t)adc_value) / (voltage_t)(adc_resolution - 1);
}

/* ***************************************** Public callback functions definitions *************************************** */
