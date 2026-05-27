/**
 * @file crc32.h
 * @brief CRC32 implementation
 * @author Dorian Benech feat. claude-4.5-sonnet
 * @date 07/10/2025
 * @see crc32.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Calculate the CRC32 of a data buffer (IEEE 802.3 standard)
 * @note Uses polynomial 0x04C11DB7 with final XOR (standard CRC-32)
 * @param previous_crc Previous CRC32 value (0xFFFFFFFF on first call)
 * @param data Pointer to the data buffer
 * @param length Length of the data buffer
 * @return The CRC32 of the data buffer
 */
uint32_t crc32_compute(uint32_t previous_crc, const uint8_t *data, uint32_t length);

/* ******************************************* Public callback functions declarations ************************************ */
