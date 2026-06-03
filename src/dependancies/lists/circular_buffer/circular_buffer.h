/**
 * @file circular_buffer.h
 * @brief Generic circular buffer implementation
 * @author Dorian Benech
 * @date 18/09/2024
 * @see circular_buffer.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* ************************************************** Public types definition ******************************************** */

typedef struct circular_buffer_t
{
    void *buffer;
    int32_t head;
    int32_t tail;
    int32_t max;
    size_t element_size;
    int32_t count;
    bool is_initialized;
} circular_buffer_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize a circular buffer
 * @param cb Pointer to the circular buffer
 * @param buffer Pointer to the buffer array
 * @param buffer_size Size of the buffer array in bytes
 * @param element_size Size of each element in bytes
 */
void circular_buffer_init(circular_buffer_t *cb,
                          void *buffer,
                          int32_t buffer_size,
                          size_t element_size);

/**
 * @brief Add an element to the circular buffer
 * @note This function will return false if the buffer is full
 * @param cb Pointer to the circular buffer
 * @param item Pointer to the item to add
 * @return true if successful, false if buffer is full
 */
bool circular_buffer_add(circular_buffer_t *cb, const void *item);

/**
 * @brief Get an element from the circular buffer
 * @param cb Pointer to the circular buffer
 * @param item Pointer to store the retrieved item
 * @return true if successful, false if buffer is empty
 */
bool circular_buffer_get(circular_buffer_t *cb, void *item);

/**
 * @brief Get the latest item added to the circular buffer
 * @param cb Pointer to the circular buffer
 * @param item Pointer to store the retrieved item
 * @return true if successful, false if buffer is empty
 */
bool circular_buffer_get_latest(const circular_buffer_t *cb, void *item);

/**
 * @brief Check if the circular buffer is empty
 * @param cb Pointer to the circular buffer
 * @return true if empty, false otherwise
 */
bool circular_buffer_is_empty(const circular_buffer_t *cb);

/**
 * @brief Check if the circular buffer is full
 * @param cb Pointer to the circular buffer
 * @return true if full, false otherwise
 */
bool circular_buffer_is_full(const circular_buffer_t *cb);

/**
 * @brief Get the number of elements in the circular buffer
 * @param cb Pointer to the circular buffer
 * @return Number of elements in the buffer
 */
int32_t circular_buffer_count(const circular_buffer_t *cb);

/**
 * @brief Pick the oldest item in the buffer without removing it
 *
 * @param cb Pointer to the circular buffer
 * @param item Pointer to the memory where the item will be stored
 * @return true if an item was successfully picked, false if the buffer is empty
 */
bool circular_buffer_pick(const circular_buffer_t *cb, void *item);

/**
 * @brief Peek at an item in the buffer at a specific index without removing it
 * @param cb Pointer to the circular buffer
 * @param index Index of the item to peek at (0 = oldest item)
 * @param item Pointer to store the peeked item
 * @return true if an item was successfully peeked, false if index is out of bounds
 */
bool circular_buffer_peek_at(const circular_buffer_t *cb, int32_t index, void *item);

/**
 * @brief Peek the latest item in the buffer without removing it
 * @param cb Pointer to the circular buffer
 * @param item Pointer to store the peeked item
 * @return true if an item was successfully peeked, false if the buffer is empty
 */
bool circular_buffer_peek_latest(const circular_buffer_t *cb, void *item);

/**
 * @brief Add an element to the circular buffer with overwrite capability
 * @note This function will overwrite the oldest item in the buffer if the buffer is full
 * @param cb Pointer to the circular buffer
 * @param item Pointer to the item to add
 * @return true always (item is always added, overwriting oldest if necessary)
 */
bool circular_buffer_add_overwrite(circular_buffer_t *cb, const void *item);

/**
 * @brief Remove the oldest item from the buffer without retrieving it
 *
 * This function removes the oldest item from the buffer by advancing the tail pointer,
 * without copying or returning the item data. This is useful when you want to discard
 * items without processing them.
 *
 * @param cb Pointer to the circular buffer
 * @return true if an item was successfully removed, false if the buffer is empty
 */
bool circular_buffer_skip_pop(circular_buffer_t *cb);

/**
 * @brief Reset the circular buffer
 * @param cb Pointer to the circular buffer
 */
void circular_buffer_reset(circular_buffer_t *cb);

/**
 * @brief Get a pointer to the last item added to the buffer without removing it
 * @param cb Pointer to the circular buffer
 * @return Pointer to the last item added, or NULL if the buffer is empty
 */
void *circular_buffer_peek_last(const circular_buffer_t *cb);
