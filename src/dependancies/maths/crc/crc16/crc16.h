/**
 * @file crc16.h
 * @brief CRC16 implementation
 * @author Dorian Benech feat. claude-4.5-sonnet
 * @date 07/10/2025
 * @see crc16.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Calculate the CRC16 of a data buffer
 * @param previous_crc Previous CRC16 value (0 on first call)
 * @param data Pointer to the data buffer
 * @param length Length of the data buffer
 * @return The CRC16 of the data buffer
 */
uint16_t crc16_compute(uint16_t previous_crc, const uint8_t *data, uint32_t length);

/* ******************************************* Public callback functions declarations ************************************ */
