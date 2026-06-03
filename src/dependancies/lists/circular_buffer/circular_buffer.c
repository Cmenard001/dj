/**
 * @file circular_buffer.c
 * @brief Generic circular buffer implementation
 * @author Dorian Benech
 * @date 18/09/2024
 * @see circular_buffer.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#include "circular_buffer.h"
#include "system/assert/system_assert.h"
#include <string.h>

/* ************************************************ Private type definition ********************************************** */

/* ********************************************** Private functions definitions ****************************************** */

static void advance_pointer(circular_buffer_t *cb, bool allow_overwrite)
{
    if (cb->count == cb->max && allow_overwrite)
    {
        // Buffer is full, advance tail to overwrite oldest element
        cb->tail = (cb->tail + 1) % cb->max;
    }
    else if (cb->count < cb->max)
    {
        cb->count++;
    }
    cb->head = (cb->head + 1) % cb->max;
}

static void retreat_pointer(circular_buffer_t *cb)
{
    cb->tail = (cb->tail + 1) % cb->max;
    cb->count--;
}

/* ********************************************** Public functions definitions ******************************************* */

void circular_buffer_init(circular_buffer_t *cb,
                          void *buffer,
                          int32_t buffer_size,
                          size_t element_size)
{
    SYSTEM_ASSERT_MSG(buffer != NULL, "buffer is NULL");
    SYSTEM_ASSERT_MSG(buffer_size > 0, "size is not greater than 0");
    SYSTEM_ASSERT_MSG(element_size > 0, "element_size is not greater than 0");

    // Check if buffer_size is divisible by element_size
    if ((size_t)buffer_size % element_size != 0)
    {
        SYSTEM_ASSERT_MSG(false,
                          "buffer_size must be divisible by element_size: Hint: buffer_size should "
                          "be sizeof(buffer)");
        // // Adjust buffer_size to make it divisible by removing excess bytes
        // buffer_size = buffer_size - (buffer_size % (int32_t)element_size);
        // SYSTEM_ASSERT_MSG(buffer_size > 0, "adjusted buffer_size is 0");
    }

    cb->buffer = buffer;
    cb->max =
        (int32_t)((size_t)buffer_size / element_size); // Convert total size to number of elements
    cb->element_size = element_size;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->is_initialized = true;
}

inline bool circular_buffer_add(circular_buffer_t *cb, const void *item)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(item != NULL, "item is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    if (cb->count == cb->max)
    {
        return false;
    }

    memcpy((char *)cb->buffer + ((size_t)cb->head * cb->element_size), item, cb->element_size);
    advance_pointer(cb, false);
    return true;
}

inline bool circular_buffer_get(circular_buffer_t *cb, void *item)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(item != NULL, "item is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    if (cb->count == 0)
    {
        return false;
    }

    memcpy(item, (char *)cb->buffer + ((size_t)cb->tail * cb->element_size), cb->element_size);
    retreat_pointer(cb);
    return true;
}

bool circular_buffer_get_latest(const circular_buffer_t *cb, void *item)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(item != NULL, "item is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    if (cb->count == 0)
    {
        return false;
    }

    int32_t latest_index = (cb->head - 1 + cb->max) % cb->max;
    memcpy(item, (char *)cb->buffer + ((size_t)latest_index * cb->element_size), cb->element_size);
    return true;
}

inline bool circular_buffer_is_empty(const circular_buffer_t *cb)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    return cb->count == 0;
}

inline bool circular_buffer_is_full(const circular_buffer_t *cb)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    return cb->count == cb->max;
}

inline int32_t circular_buffer_count(const circular_buffer_t *cb)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    return cb->count;
}

inline bool circular_buffer_pick(const circular_buffer_t *cb, void *item)
{
    return circular_buffer_peek_at(cb, 0, item);
}

bool circular_buffer_peek_at(const circular_buffer_t *cb, int32_t index, void *item)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(item != NULL, "item is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    if (index < 0 || index >= cb->count)
    {
        return false;
    }

    int32_t actual_index = (cb->tail + index) % cb->max;
    memcpy(item, (char *)cb->buffer + ((size_t)actual_index * cb->element_size), cb->element_size);
    return true;
}

inline bool circular_buffer_peek_latest(const circular_buffer_t *cb, void *item)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(item != NULL, "item is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");

    int32_t count = circular_buffer_count(cb);

    if (count <= 0)
    {
        return false;
    }

    return circular_buffer_peek_at(cb, count - 1, item);
}

inline bool circular_buffer_add_overwrite(circular_buffer_t *cb, const void *item)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(item != NULL, "item is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");

    memcpy((char *)cb->buffer + ((size_t)cb->head * cb->element_size), item, cb->element_size);
    advance_pointer(cb, true);
    return true;
}

inline bool circular_buffer_skip_pop(circular_buffer_t *cb)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    if (cb->count == 0)
    {
        return false;
    }

    retreat_pointer(cb);
    return true;
}

inline void circular_buffer_reset(circular_buffer_t *cb)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

inline void *circular_buffer_peek_last(const circular_buffer_t *cb)
{
    SYSTEM_ASSERT_MSG(cb != NULL, "cb is NULL");
    SYSTEM_ASSERT_MSG(cb->is_initialized, "cb is not initialized");
    if (cb->count == 0)
    {
        return NULL;
    }

    int32_t last_index = (cb->head - 1 + cb->max) % cb->max;
    return (char *)cb->buffer + ((size_t)last_index * cb->element_size);
}
