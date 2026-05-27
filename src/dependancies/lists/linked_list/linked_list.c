/**
 * @file linked_list.h
 * @brief Linked list library
 * @author Cyprien Ménard
 */

/* ************************************* Includes *********************************************** */

#include "linked_list.h"
#include "system/log/log.h"
#include <stdlib.h>
#include <string.h>

/* ************************************* Private macros ***************************************** */

/* ************************************* Private type definition ******************************** */

LOG_REGISTER("utils/lists/linked_list");

/* ************************************* Private functions prototypes *************************** */

/* ************************************* Private variables ************************************** */

/* ************************************* Public variables *************************************** */

/* ************************************* Private functions ************************************** */

/* ************************************* Public functions *************************************** */

linked_list_t *linked_list_create(size_t element_size)
{
    linked_list_t *list = (linked_list_t *)malloc(sizeof(linked_list_t));
    if (list == NULL)
    {
        LOGE("linked_list_create: malloc failed");
        return NULL;
    }

    linked_list_init(list, element_size);

    return list;
}

void linked_list_destroy(linked_list_t *this)
{
    // Free all elements
    linked_list_element_t *current = this->m_head;
    while (current != NULL)
    {
        linked_list_element_t *next = current->m_next;
        free(current->m_data);
        free(current);
        current = next;
    }
}

void linked_list_init(linked_list_t *this, size_t element_size)
{
    this->m_head = NULL;
    this->m_element_size = element_size;
}

void linked_list_deinit(linked_list_t *this)
{
    linked_list_element_t *current = this->m_head;
    while (current != NULL)
    {
        linked_list_element_t *next = current->m_next;
        free(current->m_data);
        free(current);
        current = next;
    }
    this->m_head = NULL;
}

void *linked_list_add(linked_list_t *this, void *element)
{
    linked_list_element_t *new_element =
        (linked_list_element_t *)malloc(sizeof(linked_list_element_t));
    if (new_element == NULL)
    {
        LOGE("linked_list_add: malloc failed");
        return NULL;
    }

    new_element->m_data = malloc(this->m_element_size);
    if (new_element->m_data == NULL)
    {
        LOGE("linked_list_add: malloc failed");
        free(new_element);
        return NULL;
    }

    memcpy(new_element->m_data, element, this->m_element_size);
    new_element->m_next = NULL; // This is the last element
    // Add the new element to the end of the list
    if (this->m_head == NULL)
    {
        this->m_head = new_element;
    }
    else
    {
        linked_list_element_t *current = this->m_head;
        while (current->m_next != NULL)
        {
            current = current->m_next;
        }
        current->m_next = new_element;
    }
    return new_element->m_data;
}

void *linked_list_get(linked_list_t *this, size_t index)
{
    linked_list_element_t *current = this->m_head;
    size_t i = 0;
    while (current != NULL)
    {
        if (i == index)
        {
            return current->m_data;
        }
        current = current->m_next;
        i++;
    }
    return NULL;
}

void linked_list_set(linked_list_t *this, int32_t index, void *element)
{
    // Go to the element at the specified index
    linked_list_element_t *current = this->m_head;
    int32_t i = 0;
    while (current != NULL)
    {
        if (i == index)
        {
            // Overwrite the current element with the new element
            memcpy(current->m_data, element, this->m_element_size);
            return;
        }
        current = current->m_next;
        i++;
    }
}

void linked_list_remove(linked_list_t *this, size_t index)
{
    linked_list_element_t *current = this->m_head;
    linked_list_element_t *previous = NULL;
    size_t i = 0;
    while (current != NULL)
    {
        if (i == index)
        {
            if (previous == NULL)
            {
                this->m_head = current->m_next;
            }
            else
            {
                previous->m_next = current->m_next;
            }
            free(current->m_data);
            free(current);
            return;
        }
        previous = current;
        current = current->m_next;
        i++;
    }
}

size_t linked_list_size(linked_list_t *this)
{
    linked_list_element_t *current = this->m_head;
    size_t size = 0;
    while (current != NULL)
    {
        size++;
        current = current->m_next;
    }
    return size * this->m_element_size;
}

uint32_t linked_list_count(linked_list_t *this)
{
    linked_list_element_t *current = this->m_head;
    uint32_t count = 0;
    while (current != NULL)
    {
        count++;
        current = current->m_next;
    }
    return count;
}

bool linked_list_is_empty(linked_list_t *this)
{
    return this->m_head == NULL;
}

void linked_list_clear(linked_list_t *this)
{
    linked_list_element_t *current = this->m_head;
    while (current != NULL)
    {
        linked_list_element_t *next = current->m_next;
        free(current->m_data);
        free(current);
        current = next;
    }
    this->m_head = NULL;
}

void linked_list_foreach(linked_list_t *this, void (*callback)(void *element))
{
    linked_list_element_t *current = this->m_head;
    while (current != NULL)
    {
        callback(current->m_data);
        current = current->m_next;
    }
}

int32_t linked_list_find(linked_list_t *this, void *element)
{
    linked_list_element_t *current = this->m_head;
    uint32_t index = 0;
    while (current != NULL)
    {
        if (memcmp(current->m_data, element, this->m_element_size) == 0)
        {
            return (int32_t)index;
        }
        current = current->m_next;
        index++;
    }
    return ERROR_NOT_FOUND;
}

void linked_list_remove_element(linked_list_t *this, void *element)
{
    linked_list_element_t *current = this->m_head;
    linked_list_element_t *previous = NULL;
    while (current != NULL)
    {
        if (memcmp(current->m_data, element, this->m_element_size) == 0)
        {
            if (previous == NULL)
            {
                this->m_head = current->m_next;
            }
            else
            {
                previous->m_next = current->m_next;
            }
            free(current->m_data);
            free(current);
            return;
        }
        previous = current;
        current = current->m_next;
    }
}

/* ************************************* Public callback functions ****************************** */
