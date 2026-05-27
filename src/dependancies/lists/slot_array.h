/**
 * @file slot_array.h
 * @brief Slot arrays are containers that can hold a fixed number of elements. They are used to store
 * elements that can be frequently removed. The add and access operations are slower than magic_array,
 * but the remove operation is faster. The remove operation does not shift the elements, it just marks
 * the slot as empty. The add operation will fill the first empty slot it finds. The access operation
 * will return the element at the given index, or NULL if the slot is empty.
 * @note These arrays are static, meaning their maximum size cannot change.
 * However, we use them like dynamic arrays, meaning we can add, remove, read, and modify elements.
 * @note Unlike magic_array, slot_array does not compact on removal, making it ideal for frequent
 * add/remove operations but at the cost of potential fragmentation.
 * @note These arrays should be private but you can automatically create accessors
 * @note The type can be public, meaning you can put it in the header.
 *
 * @author Cyprien Ménard
 * @date 13/02/2026
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/**
 * @note Usage example for a slot array of integers with maximum size 10:
 * @code
 * // In the .h
 * // Type declaration
 * SLOT_ARRAY_TYPEDEF(your_slot_array, int, 10);
 * // Accessor declarations
 * SLOT_ARRAY_DECLARATION(your_slot_array, int)
 *
 * // In the .c
 * // Accessor definitions
 * SLOT_ARRAY_DEFINITION(your_slot_array, int)
 *
 * // In the .c
 * void your_function()
 * {
 *    your_slot_array_t your_slot_array;
 *    your_slot_array_init(&your_slot_array);
 *    int element = 5;
 *    your_slot_array_add(&your_slot_array, &element);
 *
 *    // Remove an element (slot is marked as empty, no shifting)
 *    your_slot_array_remove(&your_slot_array, 0);
 *
 *    // Get returns NULL if slot is empty
 *    int *ptr = your_slot_array_get(&your_slot_array, 0);
 *    if (ptr != NULL) {
 *        // Slot is occupied
 *    }
 *
 *    // Check if a specific slot is occupied
 *    if (your_slot_array_is_occupied(&your_slot_array, 0)) {
 *        // Slot 0 is occupied
 *    }
 *
 *    // Iterate over all occupied slots
 *    your_slot_array_foreach(&your_slot_array, callback, user_data);
 * }
 * @endcode
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "system/assert/system_assert.h"
#include "system/log/log.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* ***************************************************** Public macros *************************************************** */

#define SLOT_ARRAY_NOT_FOUND           (uint32_t)(-1)

/**
 * @brief Macro to define the initialization function for a slot array
 * @note This macro must be used in the .c file
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_INIT_DEF(type) void type##_init(type##_t *array)

#define SLOT_ARRAY_FUNC_INIT(type)                                                                 \
    SLOT_ARRAY_FUNC_INIT_DEF(type)                                                                 \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (init)");                     \
        array->max_size = sizeof(array->array) / sizeof(array->array[0]);                          \
        SYSTEM_ASSERT_MSG(array->max_size > 0, "Slot array max_size must be > 0");                 \
        array->current_size = 0;                                                                   \
        for (uint32_t i = 0; i < array->max_size; i++)                                             \
        {                                                                                          \
            array->occupied[i] = false;                                                            \
        }                                                                                          \
    }

/**
 * @brief Macro to define the function to add an element in a slot array
 * @note This macro must be used in the .c file
 * @note The element will be added in the first free slot found
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 * @param element_type The type of the element to add
 */
#define SLOT_ARRAY_FUNC_ADD_DEF(type, element_type)                                                \
    element_type *type##_add(type##_t *array, const element_type *element)

#define SLOT_ARRAY_FUNC_ADD(type, element_type)                                                    \
    SLOT_ARRAY_FUNC_ADD_DEF(type, element_type)                                                    \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (add)");                      \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        if (array->current_size < array->max_size)                                                 \
        {                                                                                          \
            for (uint32_t i = 0; i < array->max_size; i++)                                         \
            {                                                                                      \
                if (!array->occupied[i])                                                           \
                {                                                                                  \
                    if (element != NULL)                                                           \
                    {                                                                              \
                        array->array[i] = *element;                                                \
                    }                                                                              \
                    array->occupied[i] = true;                                                     \
                    array->current_size++;                                                         \
                    return &array->array[i];                                                       \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
        LOGE("%s array is full", #type);                                                           \
        return NULL;                                                                               \
    }

/**
 * @brief Macro to define the function to remove an element from a slot array
 * @note This macro must be used in the .c file
 * @note The slot is just marked as empty, no shifting occurs
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_REMOVE_DEF(type) void type##_remove(type##_t *array, uint32_t index)

#define SLOT_ARRAY_FUNC_REMOVE(type)                                                               \
    SLOT_ARRAY_FUNC_REMOVE_DEF(type)                                                               \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (remove)");                   \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        SYSTEM_ASSERT_MSG(index < array->max_size, "Index out of bounds (remove)");                \
        if (array->occupied[index])                                                                \
        {                                                                                          \
            array->occupied[index] = false;                                                        \
            array->current_size--;                                                                 \
        }                                                                                          \
    }

/**
 * @brief Macro to define the function to read an element from a slot array
 * @note This macro must be used in the .c file
 * @note Returns NULL if the slot is empty
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 * @param element_type The type of the element to read
 */
#define SLOT_ARRAY_FUNC_GET_DEF(type, element_type)                                                \
    element_type *type##_get(const type##_t *array, uint32_t index)

#define SLOT_ARRAY_FUNC_GET(type, element_type)                                                    \
    SLOT_ARRAY_FUNC_GET_DEF(type, element_type)                                                    \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (get)");                      \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        SYSTEM_ASSERT_MSG(index < array->max_size, "Index out of bounds (get)");                   \
        if (array->occupied[index])                                                                \
        {                                                                                          \
            return (element_type *)&array->array[index];                                           \
        }                                                                                          \
        return NULL;                                                                               \
    }

/**
 * @brief Macro to define the function to modify an element in a slot array
 * @note This macro must be used in the .c file
 * @note Does nothing if the slot is empty
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 * @param element_type The type of the element to modify
 */
#define SLOT_ARRAY_FUNC_SET_DEF(type, element_type)                                                \
    void type##_set(type##_t *array, uint32_t index, const element_type *element)

#define SLOT_ARRAY_FUNC_SET(type, element_type)                                                    \
    SLOT_ARRAY_FUNC_SET_DEF(type, element_type)                                                    \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (set)");                      \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        SYSTEM_ASSERT_MSG(index < array->max_size, "Index out of bounds (set)");                   \
        if (array->occupied[index] && element != NULL)                                             \
        {                                                                                          \
            array->array[index] = *element;                                                        \
        }                                                                                          \
    }

/**
 * @brief Macro to define the function to clear all elements in a slot array
 * @note This macro must be used in the .c file
 * @note This function does not free memory, it just marks all slots as empty
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_CLEAR_DEF(type) void type##_clear(type##_t *array)

#define SLOT_ARRAY_FUNC_CLEAR(type)                                                                \
    SLOT_ARRAY_FUNC_CLEAR_DEF(type)                                                                \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (clear)");                    \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        array->current_size = 0;                                                                   \
        for (uint32_t i = 0; i < array->max_size; i++)                                             \
        {                                                                                          \
            array->occupied[i] = false;                                                            \
        }                                                                                          \
    }

/**
 * @brief Macro to define the function to check if a slot array is empty
 * @note This macro must be used in the .c file
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_IS_EMPTY_DEF(type) bool type##_is_empty(const type##_t *array)

#define SLOT_ARRAY_FUNC_IS_EMPTY(type)                                                             \
    SLOT_ARRAY_FUNC_IS_EMPTY_DEF(type)                                                             \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (is_empty)");                 \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        return array->current_size == 0;                                                           \
    }

/**
 * @brief Macro to define the function to check if a slot array is full
 * @note This macro must be used in the .c file
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_IS_FULL_DEF(type) bool type##_is_full(const type##_t *array)

#define SLOT_ARRAY_FUNC_IS_FULL(type)                                                              \
    SLOT_ARRAY_FUNC_IS_FULL_DEF(type)                                                              \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (is_full)");                  \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        return array->current_size == array->max_size;                                             \
    }

/**
 * @brief Macro to define the function to get the size of a slot array
 * @note This macro must be used in the .c file
 * @note Returns the number of occupied slots
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_SIZE_DEF(type) uint32_t type##_size(const type##_t *array)

#define SLOT_ARRAY_FUNC_SIZE(type)                                                                 \
    SLOT_ARRAY_FUNC_SIZE_DEF(type)                                                                 \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (size)");                     \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        return array->current_size;                                                                \
    }

/**
 * @brief Macro to define the function to get the maximum size of a slot array
 * @note This macro must be used in the .c file
 *
 * @param type The type of the array elements
 */
#define SLOT_ARRAY_FUNC_MAX_SIZE_DEF(type) uint32_t type##_max_size(const type##_t *array)

#define SLOT_ARRAY_FUNC_MAX_SIZE(type)                                                             \
    SLOT_ARRAY_FUNC_MAX_SIZE_DEF(type)                                                             \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (max_size)");                 \
        SYSTEM_ASSERT_MSG(array->max_size > 0, "Array max_size must be > 0");                      \
        return array->max_size;                                                                    \
    }

/**
 * @brief Macro to define the function to check if a slot is occupied
 * @note This macro must be used in the .c file
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_IS_OCCUPIED_DEF(type)                                                      \
    bool type##_is_occupied(const type##_t *array, uint32_t index)

#define SLOT_ARRAY_FUNC_IS_OCCUPIED(type)                                                          \
    SLOT_ARRAY_FUNC_IS_OCCUPIED_DEF(type)                                                          \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (is_occupied)");              \
        SYSTEM_ASSERT_MSG(index < array->max_size, "Index out of bounds (is_occupied)");           \
        return array->occupied[index];                                                             \
    }

/**
 * @brief Macro to define the function to find the first free slot in a slot array
 * @note This macro must be used in the .c file
 * @note Returns SLOT_ARRAY_NOT_FOUND if no free slot is found
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 */
#define SLOT_ARRAY_FUNC_FIND_FIRST_FREE_DEF(type)                                                  \
    uint32_t type##_find_first_free(const type##_t *array)

#define SLOT_ARRAY_FUNC_FIND_FIRST_FREE(type)                                                      \
    SLOT_ARRAY_FUNC_FIND_FIRST_FREE_DEF(type)                                                      \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (find_first_free)");          \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        for (uint32_t i = 0; i < array->max_size; i++)                                             \
        {                                                                                          \
            if (!array->occupied[i])                                                               \
            {                                                                                      \
                return i;                                                                          \
            }                                                                                      \
        }                                                                                          \
        return SLOT_ARRAY_NOT_FOUND;                                                               \
    }

/**
 * @brief Macro to define the function to iterate over occupied slots
 * @note This macro must be used in the .c file
 * @note Callback is called for each occupied slot with the index and element
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 * @param element_type The type of the element
 */
#define SLOT_ARRAY_FUNC_FOREACH_DEF(type, element_type)                                            \
    void type##_foreach(const type##_t *array,                                                     \
                        void (*callback)(uint32_t index, element_type *element, void *user_data),  \
                        void *user_data)

#define SLOT_ARRAY_FUNC_FOREACH(type, element_type)                                                \
    SLOT_ARRAY_FUNC_FOREACH_DEF(type, element_type)                                                \
    {                                                                                              \
        SYSTEM_ASSERT_MSG(array != NULL, "Slot array pointer is NULL (foreach)");                  \
        SYSTEM_ASSERT_MSG(callback != NULL, "Callback pointer is NULL (foreach)");                 \
        SYSTEM_ASSERT_MSG(array->current_size <= array->max_size, "Array invariant violated");     \
        for (uint32_t i = 0; i < array->max_size; i++)                                             \
        {                                                                                          \
            if (array->occupied[i])                                                                \
            {                                                                                      \
                callback(i, (element_type *)&array->array[i], user_data);                          \
            }                                                                                      \
        }                                                                                          \
    }

/**
 * @brief Macro to define the type of a slot array
 * @note This macro can be used in the .h or .c file
 *
 * @param type The type of the array elements
 * (do not suffix with _t, it's automatic)
 * @param element_type The type of the array elements
 * @param _max_size The maximum size of the array
 */
#define SLOT_ARRAY_TYPEDEF(type, element_type, _max_size)                                          \
    typedef struct                                                                                 \
    {                                                                                              \
        element_type array[_max_size];                                                             \
        bool occupied[_max_size];                                                                  \
        uint32_t max_size;                                                                         \
        uint32_t current_size;                                                                     \
    } type##_t;

/**
 * @brief Macro to define all functions of a slot array
 * @note This macro must be used in a .c file
 *
 * @param type The type of the array elements
 * do not suffix with _t, it's automatic
 * must be the same as used in SLOT_ARRAY_TYPEDEF
 */
#define SLOT_ARRAY_DEFINITION(type, element_type)                                                  \
    SLOT_ARRAY_FUNC_INIT(type)                                                                     \
    SLOT_ARRAY_FUNC_ADD(type, element_type)                                                        \
    SLOT_ARRAY_FUNC_REMOVE(type)                                                                   \
    SLOT_ARRAY_FUNC_GET(type, element_type)                                                        \
    SLOT_ARRAY_FUNC_SET(type, element_type)                                                        \
    SLOT_ARRAY_FUNC_CLEAR(type)                                                                    \
    SLOT_ARRAY_FUNC_IS_EMPTY(type)                                                                 \
    SLOT_ARRAY_FUNC_IS_FULL(type)                                                                  \
    SLOT_ARRAY_FUNC_SIZE(type)                                                                     \
    SLOT_ARRAY_FUNC_MAX_SIZE(type)                                                                 \
    SLOT_ARRAY_FUNC_IS_OCCUPIED(type)                                                              \
    SLOT_ARRAY_FUNC_FIND_FIRST_FREE(type)                                                          \
    SLOT_ARRAY_FUNC_FOREACH(type, element_type)

/**
 * @brief Macro to declare all functions of a slot array to make them public
 * @note This macro must be used in a .h file
 *
 * @param type The type of the array elements
 * do not suffix with _t, it's automatic
 * must be the same as used in SLOT_ARRAY_TYPEDEF
 */
#define SLOT_ARRAY_DECLARATION(type, element_type)                                                 \
    SLOT_ARRAY_FUNC_INIT_DEF(type);                                                                \
    SLOT_ARRAY_FUNC_ADD_DEF(type, element_type);                                                   \
    SLOT_ARRAY_FUNC_REMOVE_DEF(type);                                                              \
    SLOT_ARRAY_FUNC_GET_DEF(type, element_type);                                                   \
    SLOT_ARRAY_FUNC_SET_DEF(type, element_type);                                                   \
    SLOT_ARRAY_FUNC_CLEAR_DEF(type);                                                               \
    SLOT_ARRAY_FUNC_IS_EMPTY_DEF(type);                                                            \
    SLOT_ARRAY_FUNC_IS_FULL_DEF(type);                                                             \
    SLOT_ARRAY_FUNC_SIZE_DEF(type);                                                                \
    SLOT_ARRAY_FUNC_MAX_SIZE_DEF(type);                                                            \
    SLOT_ARRAY_FUNC_IS_OCCUPIED_DEF(type);                                                         \
    SLOT_ARRAY_FUNC_FIND_FIRST_FREE_DEF(type);                                                     \
    SLOT_ARRAY_FUNC_FOREACH_DEF(type, element_type);

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */
