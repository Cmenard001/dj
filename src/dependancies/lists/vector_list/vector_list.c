/**
 * @file vector_list.h
 * @brief Vector list library. A vector list is a list that stores elements in an dynamic array
 * @author Cyprien Ménard
 */

/* ************************************* Includes *********************************************** */

#include "vector_list.h"
#include "system/log/log.h"

#include <stdlib.h>
#include <string.h>

/* ************************************* Private macros ***************************************** */

/* ************************************* Private type definition ******************************** */

LOG_REGISTER("utils/lists/vector_list");

/* ************************************* Private functions prototypes *************************** */

/* ************************************* Private variables ************************************** */

/* ************************************* Public variables *************************************** */

/* ************************************* Private functions ************************************** */

/* ************************************* Public functions *************************************** */

vector_list_t *vector_list_create(size_t element_size, uint32_t capacity)
{
    vector_list_t *this = (vector_list_t *)malloc(sizeof(vector_list_t));
    if (this == NULL)
    {
        LOGE("vector_list_create: malloc failed");
        return NULL;
    }
    vector_list_init(this, element_size, capacity);
    return this;
}

void vector_list_destroy(vector_list_t *this)
{
    vector_list_deinit(this);
    free(this);
}

void vector_list_init(vector_list_t *this, size_t element_size, uint32_t capacity)
{
    if (element_size == 0)
    {
        LOGE("vector_list_init: element_size is 0");
        return;
    }
    this->m_element_size = element_size;
    this->m_capacity = capacity * element_size;
    this->m_current_size = 0;
    this->m_data = malloc(element_size * capacity);
    if (this->m_data == NULL)
    {
        LOGE("vector_list_init: malloc failed");
    }
}

void vector_list_deinit(vector_list_t *this)
{
    free(this->m_data);
}

void *vector_list_add(vector_list_t *this, void *element)
{
    if (this == NULL)
    {
        LOGE("vector_list_add: list is NULL");
        return NULL;
    }
    if (this->m_current_size < this->m_capacity)
    {
        if (element != NULL)
        {
            memcpy((char *)this->m_data + this->m_current_size, element, this->m_element_size);
        }
        this->m_current_size += this->m_element_size;
    }
    else
    {
        // Resize the list
        this->m_capacity *= 2;
        void *old_data = this->m_data;
        this->m_data = malloc(this->m_capacity);
        if (this->m_data == NULL)
        {
            LOGE("vector_list_add: malloc failed");
            return NULL;
        }
        memcpy(this->m_data, old_data, this->m_current_size);
        free(old_data);
        // Add the element
        if (element != NULL)
        {
            memcpy((char *)this->m_data + this->m_current_size, element, this->m_element_size);
        }
        this->m_current_size += this->m_element_size;
    }
    return (char *)this->m_data + this->m_current_size - this->m_element_size;
}

void *vector_list_get(vector_list_t *this, uint32_t index)
{
    if (this == NULL)
    {
        LOGE("vector_list_get: list is NULL");
        return NULL;
    }
    if (index * this->m_element_size < this->m_current_size)
    {
        return (char *)this->m_data + index * this->m_element_size;
    }
    return NULL;
}

uint32_t vector_list_count(vector_list_t *this)
{
    if (this == NULL)
    {
        LOGE("vector_list_count: list is NULL");
        return 0;
    }
    if (this->m_element_size == 0)
    {
        LOGE("vector_list_count: element_size is 0");
        return 0;
    }
    if (this->m_current_size < this->m_element_size)
    {
        return 0;
    }
    return (uint32_t)(this->m_current_size / this->m_element_size);
}

bool vector_list_is_empty(vector_list_t *this)
{
    if (this == NULL)
    {
        LOGE("vector_list_is_empty: list is NULL");
        return true;
    }
    return (this->m_current_size == 0);
}

void vector_list_set(vector_list_t *this, uint32_t index, void *element)
{
    if (this == NULL)
    {
        LOGE("vector_list_set: list is NULL");
        return;
    }
    if (index * this->m_element_size < this->m_current_size)
    {
        memcpy((char *)this->m_data + index * this->m_element_size, element, this->m_element_size);
    }
}

void vector_list_clear(vector_list_t *this)
{
    if (this == NULL)
    {
        LOGE("vector_list_clear: list is NULL");
        return;
    }
    this->m_current_size = 0;
}

void vector_list_reset(vector_list_t *this, uint32_t index)
{
    if (this == NULL)
    {
        LOGE("vector_list_reset: list is NULL");
        return;
    }
    size_t new_size = (index + 1) * this->m_element_size;
    if (new_size < this->m_current_size)
    {
        this->m_current_size = new_size;
    }
}

/* ************************************* Public callback functions ****************************** */
