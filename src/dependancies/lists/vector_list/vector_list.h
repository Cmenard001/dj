/**
 * @file vector_list.h
 * @brief Vector list library. A vector list is a list that stores elements in an dynamic array
 * @author Cyprien Ménard
 */

#pragma once

/* ************************************* Includes *********************************************** */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* ************************************* Public macros ****************************************** */

#define ERROR_NOT_FOUND -1

/* ************************************* Public type definition ********************************* */

/**
 * @brief Generic types vector list
 * @note A vector list is a list that stores elements in an dynamic array
 * @note The array is resized when the capacity is reached
 */
typedef struct
{
    void *m_data;
    size_t m_element_size;
    size_t m_capacity;
    size_t m_current_size;
} vector_list_t;

/* ************************************* Public variables *************************************** */

/* ************************************* Public functions *************************************** */

/**
 * @brief Function to create a new vector list
 *
 * @param element_size size of the elements to be stored in the list
 * @param capacity Number of elements that the list can initially store
 * @return vector_list_t* pointer to the created list
 */
vector_list_t *vector_list_create(size_t element_size, uint32_t capacity);

/**
 * @brief Function to destroy a vector list
 *
 * @param list pointer to the list to be destroyed
 */
void vector_list_destroy(vector_list_t *this);

/**
 * @brief Function to initialize a vector list
 *
 * @param list pointer to the list to be initialized
 * @param element_size size of the elements to be stored in the list
 * @param capacity Number of elements that the list can initially store
 */
void vector_list_init(vector_list_t *this, size_t element_size, uint32_t capacity);

/**
 * @brief Function to deinitialize a vector list
 *
 * @param list pointer to the list to be deinitialized
 */
void vector_list_deinit(vector_list_t *this);

/**
 * @brief Function to add an element to the list
 *
 * @param list pointer to the list
 * @param element pointer to the element to be added
 * @note The element is copied to the list
 * @return void* pointer to the added element
 */
void *vector_list_add(vector_list_t *this, void *element);

/**
 * @brief Function to get an element from the list
 *
 * @param list pointer to the list
 * @param index index of the element to be retrieved
 * @return void* pointer to the element
 */
void *vector_list_get(vector_list_t *this, uint32_t index);

/**
 * @brief Function to get the size of the list
 *
 * @param list pointer to the list
 * @return uint32_t size of the list
 */
uint32_t vector_list_count(vector_list_t *this);

/**
 * @brief Function to check if the list is empty
 *
 * @param list pointer to the list
 * @return bool true if the list is empty, false otherwise
 */
bool vector_list_is_empty(vector_list_t *this);

/**
 * @brief Function to set an element in the list
 *
 * @param list pointer to the list
 * @param index index of the element to be set
 * @param element pointer to the element to be set
 */
void vector_list_set(vector_list_t *this, uint32_t index, void *element);

/**
 * @brief Function to clear the list
 *
 * @param list pointer to the list
 */
void vector_list_clear(vector_list_t *this);

/**
 * @brief Function to reset the list at a specific index
 *
 * @param list pointer to the list
 * @param index last index to keep
 *
 * @note This function will remove all elements after the index
 */
void vector_list_reset(vector_list_t *this, uint32_t index);

/* ************************************* Public callback functions ****************************** */
