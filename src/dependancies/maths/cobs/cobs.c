/**
 * @file cobs.c
 * @brief COBS (Consistent Overhead Byte Stuffing) encoder/decoder
 * @author Dorian Benech feat. claude-4.5-sonnet
 * @date 11/10/2025
 * @see cobs.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "cobs.h"
#include "system/assert/system_assert.h"
#include <stddef.h>
#include <stdint.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

uint16_t cobs_encode(const uint8_t *input,
                     uint16_t input_len,
                     uint8_t *output,
                     uint16_t output_max_len)
{
    SYSTEM_ASSERT(input != NULL || input_len == 0);
    SYSTEM_ASSERT(output != NULL || output_max_len == 0);
    if (input == NULL || output == NULL)
    {
        return 0;
    }

    if (input_len == 0)
    {
        return 0;
    }

    // Check if output buffer is large enough
    uint16_t max_encoded_size = (uint16_t)COBS_ENCODE_MAX_SIZE(input_len);
    if (output_max_len < max_encoded_size)
    {
        return 0;
    }

    uint16_t read_index = 0;
    uint16_t write_index = 1;
    uint16_t code_index = 0;
    uint8_t code = 1;

    while (read_index < input_len)
    {
        if (input[read_index] == 0)
        {
            // Found a zero byte - write the code and start new block
            output[code_index] = code;
            code = 1;
            code_index = write_index++;
            read_index++;
        }
        else
        {
            // Copy non-zero byte
            output[write_index++] = input[read_index++];
            code++;

            if (code == 0xFF)
            {
                // Block is full (254 bytes) - write code and start new block
                output[code_index] = code;
                code = 1;
                code_index = write_index++;
            }
        }
    }

    // Write final code
    output[code_index] = code;

    return write_index;
}

uint16_t cobs_decode(const uint8_t *input,
                     uint16_t input_len,
                     uint8_t *output,
                     uint16_t output_max_len)
{
    SYSTEM_ASSERT(input != NULL || input_len == 0);
    SYSTEM_ASSERT(output != NULL || output_max_len == 0);
    if (input == NULL || output == NULL)
    {
        return 0;
    }

    if (input_len == 0)
    {
        return 0;
    }

    uint16_t read_index = 0;
    uint16_t write_index = 0;
    uint8_t code;
    uint8_t i;

    while (read_index < input_len)
    {
        code = input[read_index];

        // Validate code byte (must be 1-255)
        if (code == 0)
        {
            // Invalid COBS frame - code byte cannot be 0
            return 0;
        }

        read_index++;

        // Copy 'code - 1' bytes
        for (i = 1; i < code; i++)
        {
            if (read_index >= input_len)
            {
                // Unexpected end of input
                return 0;
            }

            if (write_index >= output_max_len)
            {
                // Output buffer too small
                return 0;
            }

            output[write_index++] = input[read_index++];
        }

        // Add zero byte if not at end and code is not 0xFF
        if (code < 0xFF && read_index < input_len)
        {
            if (write_index >= output_max_len)
            {
                // Output buffer too small
                return 0;
            }

            output[write_index++] = 0;
        }
    }

    return write_index;
}

/* ***************************************** Public callback functions definitions *************************************** */
