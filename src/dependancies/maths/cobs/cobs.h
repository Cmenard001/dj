/**
 * @file cobs.h
 * @brief COBS (Consistent Overhead Byte Stuffing) encoder/decoder
 * @author Dorian Benech feat. claude-4.5-sonnet
 * @date 11/10/2025
 * @see cobs.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Calculate maximum size of COBS encoded data
 * @param n Size of input data in bytes
 * @return Maximum size after COBS encoding (worst case)
 */
#define COBS_ENCODE_MAX_SIZE(n) ((n) + ((n) + 253) / 254 + 1)

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Encode data using COBS algorithm
 * COBS (Consistent Overhead Byte Stuffing) eliminates all 0x00 bytes from the input data,
 * replacing them with a special encoding that adds at most 1 byte of overhead per 254 bytes.
 * The encoded data can then be delimited using 0x00 bytes.
 * @param input Pointer to input data to encode
 * @param input_len Length of input data in bytes
 * @param output Pointer to output buffer for encoded data
 * @param output_max_len Maximum size of output buffer
 * @return Length of encoded data in bytes, or 0 on error (buffer too small)
 */
uint16_t cobs_encode(const uint8_t *input,
                     uint16_t input_len,
                     uint8_t *output,
                     uint16_t output_max_len);

/**
 * @brief Decode COBS encoded data
 * Decodes data that was previously encoded with cobs_encode().
 * Returns 0 if the input data has invalid COBS framing.
 * @param input Pointer to COBS encoded data
 * @param input_len Length of encoded data in bytes
 * @param output Pointer to output buffer for decoded data
 * @param output_max_len Maximum size of output buffer
 * @return Length of decoded data in bytes, or 0 on error (invalid frame or buffer too small)
 */
uint16_t cobs_decode(const uint8_t *input,
                     uint16_t input_len,
                     uint8_t *output,
                     uint16_t output_max_len);

/* ******************************************* Public callback functions declarations ************************************ */
