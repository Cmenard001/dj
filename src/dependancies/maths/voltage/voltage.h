/**
 * @file voltage.h
 * @brief Voltage type definition
 * @author Dorian Benech
 * @date 08/12/2025
 * @see voltage.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

typedef float voltage_t; /**< Voltage type in Volts */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Convert ADC value to voltage
 * @param adc_value ADC raw value from the ADC
 * @param reference_voltage Reference voltage of the ADC
 * @param adc_resolution Resolution of the ADC (e.g., 4096 for 12 bits)
 * @return voltage_t Converted voltage value
 */
voltage_t voltage_from_adc_value(uint32_t adc_value,
                                 voltage_t reference_voltage,
                                 uint32_t adc_resolution);
/* ******************************************* Public callback functions declarations ************************************ */
