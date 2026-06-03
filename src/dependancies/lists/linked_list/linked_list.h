/**
 * @file linked_list.h
 * @brief Linked list library
 * @author Cyprien Ménard
 */

#pragma once

/* ************************************* Includes *********************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ************************************* Public macros ****************************************** */

#define ERROR_NOT_FOUND -1

/* ************************************* Public type definition ********************************* */

/**
 * @brief Generic types linked list
 *
 */
typedef struct linked_list_node
{
    void *m_data;
    struct linked_list_node *m_next;
} linked_list_element_t;

typedef struct
{
    linked_list_element_t *m_head;
    size_t m_element_size;
} linked_list_t;

/* ************************************* Public variables *************************************** */

/* ************************************* Public functions *************************************** */

/**
 * @brief Function to create a new linked list
 *
 * @param element_size size of the elements to be stored in the list
 * @return linked_list_t* pointer to the created list
 */
linked_list_t *linked_list_create(size_t element_size);

/**
 * @brief Function to destroy a linked list
 *
 * @param list pointer to the list to be destroyed
 */
void linked_list_destroy(linked_list_t *this);

/**
 * @brief Function to initialize a linked list
 *
 * @param list pointer to the list to be initialized
 * @param element_size size of the elements to be stored in the list
 */
void linked_list_init(linked_list_t *this, size_t element_size);

/**
 * @brief Function to deinitialize a linked list
 *
 * @param list pointer to the list to be deinitialized
 */
void linked_list_deinit(linked_list_t *this);

/**
 * @brief Function to add an element to the list
 *
 * @param list pointer to the list
 * @param element pointer to the element to be added
 * @return void* pointer to the added element
 */
void *linked_list_add(linked_list_t *this, void *element);

/**
 * @brief Function to get the element at the specified index
 *
 * @param list pointer to the list
 * @param index index of the element to be retrieved
 * @return void* pointer to the element
 */
void *linked_list_get(linked_list_t *this, size_t index);

/**
 * @brief Function to set the element at the specified index
 *
 * @param list pointer to the list
 * @param index index of the element to be set
 * @param element pointer to the element to be set
 */
void linked_list_set(linked_list_t *this, int32_t index, void *element);

/**
 * @brief Function to remove the element at the specified index
 *
 * @param list pointer to the list
 * @param index index of the element to be removed
 */
void linked_list_remove(linked_list_t *this, size_t index);

/**
 * @brief Function to get the size of the list
 *
 * @param list pointer to the list
 * @return size_t size of the list
 */
size_t linked_list_size(linked_list_t *this);

/**
 * @brief Function to get the number of elements in the list
 *
 * @param list pointer to the list
 * @return uint32_t number of elements in the list
 */
uint32_t linked_list_count(linked_list_t *this);

/**
 * @brief Function to check if the list is empty
 *
 * @param list pointer to the list
 * @return bool true if the list is empty, false otherwise
 */
bool linked_list_is_empty(linked_list_t *this);

/**
 * @brief Function to clear the list
 *
 * @param list pointer to the list
 */
void linked_list_clear(linked_list_t *this);

/**
 * @brief Function to iterate over the list
 *
 * @param list pointer to the list
 * @param callback function to be called for each element
 */
void linked_list_foreach(linked_list_t *this, void (*callback)(void *element));

/**
 * @brief Function to find an element in the list
 *
 * @param list pointer to the list
 * @param element pointer to the element to be found
 * @return int32_t index of the element, ERROR_NOT_FOUND if the element is not found
 */
int32_t linked_list_find(linked_list_t *this, void *element);

/**
 * @brief Function to remove an element from the list
 *
 * @param list pointer to the list
 * @param element pointer to the element to be removed
 */
void linked_list_remove_element(linked_list_t *this, void *element);

/* ************************************* Public callback functions ****************************** */
