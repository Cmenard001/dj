/**
 * @file QS_magic_array.h
 * @brief Les tableaux magiques sont des tableaux basiques mais on peut les manipuler
 * avec des fonctions qui contrôlent et font certaines choses pour nous.
 * @note Ces tableaux sont statiques, c'est-à-dire que leur taille maximale ne peut pas changer.
 * Par contre, on les utilise comme des tableaux dynamiques, c'est-à-dire qu'on peut ajouter,
 * supprimer, lire, modifier des éléments.
 * @note Ces tableaux doivent être privés mais on peut créer automatiquement les accesseurs
 * @note Le type peut par contre être public, c'est-à-dire que vous pouvez le mettre dans le header.
 *
 * @author Cyprien Ménard
 * @date 12/12/2024
 */

/**
 * @note Exemple d'utilisation pour un tableau magique d'entiers de taille maximale 10:
 * @code
 * // Dans le .h
 * // Déclaration du type
 * MAGIC_ARRAY_TYPEDEF(votre_tableau_magique, int, 10);
 * // Déclaration des accesseurs
 * MAGIC_ARRAY_HEADER(votre_tableau_magique, int)
 *
 * // Dans le .c
 * // Définition des accesseurs
 * MAGIC_ARRAY_SRC(votre_tableau_magique, int)
 *
 * // Dans le .c
 * void votre_fonction()
 * {
 *    votre_tableau_magique_t votre_tableau_magique;
 *    votre_tableau_magique_init(&votre_tableau_magique);
 *    int element = 5;
 *    votre_tableau_magique_add(&votre_tableau_magique, &element);
 * }
 * @endcode
 */

#ifndef QS_MAGIC_ARRAY_H
#define QS_MAGIC_ARRAY_H

/* ******************************************************* Includes ****************************************************** */

#define error_printf printf

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* ***************************************************** Public macros *************************************************** */

#define MAGIC_ARRAY_NOT_FOUND (uint32_t)(-1)

/**
 * @brief Macro pour déclarer la fonction d'initialisation d'un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_INIT_DEF(type) void type##_init(type##_t *array)

#define MAGIC_ARRAY_FUNC_INIT(type)                                                                                         \
    MAGIC_ARRAY_FUNC_INIT_DEF(type)                                                                                         \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (init)\n", #type);                                                        \
            return;                                                                                                         \
        }                                                                                                                   \
        array->max_size = sizeof(array->array) / sizeof(array->array[0]);                                                   \
        array->current_size = 0;                                                                                            \
    }

/**
 * @brief Macro pour déclarer la fonction d'ajout d'un élément dans un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 * @param element_type Le type de l'élément à ajouter
 */
#define MAGIC_ARRAY_FUNC_ADD_DEF(type, element_type) element_type *type##_add(type##_t *array, element_type *element)

#define MAGIC_ARRAY_FUNC_ADD(type, element_type)                                                                            \
    MAGIC_ARRAY_FUNC_ADD_DEF(type, element_type)                                                                            \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            printf("Error: %s array is NULL (add)\n", #type);                                                               \
            return NULL;                                                                                                    \
        }                                                                                                                   \
        if (array->current_size < array->max_size)                                                                          \
        {                                                                                                                   \
            if (element != NULL)                                                                                            \
            {                                                                                                               \
                array->array[array->current_size] = *element;                                                               \
            }                                                                                                               \
            return &array->array[array->current_size++];                                                                    \
        }                                                                                                                   \
        else                                                                                                                \
        {                                                                                                                   \
            printf("Error: %s array is full\n", #type);                                                                     \
            return NULL;                                                                                                    \
        }                                                                                                                   \
    }

/**
 * @brief Macro pour déclarer la fonction de suppression d'un élément dans un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_REMOVE_DEF(type) void type##_remove(type##_t *array, uint32_t index)

#define MAGIC_ARRAY_FUNC_REMOVE(type)                                                                                       \
    MAGIC_ARRAY_FUNC_REMOVE_DEF(type)                                                                                       \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (remove)\n", #type);                                                      \
            return;                                                                                                         \
        }                                                                                                                   \
        if (index < array->current_size)                                                                                    \
        {                                                                                                                   \
            for (uint32_t i = index; i < array->current_size - 1; i++)                                                      \
            {                                                                                                               \
                array->array[i] = array->array[i + 1];                                                                      \
            }                                                                                                               \
            array->current_size--;                                                                                          \
        }                                                                                                                   \
        else                                                                                                                \
        {                                                                                                                   \
            error_printf("Error: %s array does not have an element at index %d (remove)\n", #type, (int)index);             \
        }                                                                                                                   \
    }

/**
 * @brief Macro pour déclarer la fonction de lecture d'un élément dans un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 * @param element_type Le type de l'élément à lire
 */
#define MAGIC_ARRAY_FUNC_GET_DEF(type, element_type) element_type *type##_get(type##_t *array, uint32_t index)

#define MAGIC_ARRAY_FUNC_GET(type, element_type)                                                                            \
    MAGIC_ARRAY_FUNC_GET_DEF(type, element_type)                                                                            \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (get)\n", #type);                                                         \
            return NULL;                                                                                                    \
        }                                                                                                                   \
        if (index < array->current_size)                                                                                    \
        {                                                                                                                   \
            return &array->array[index];                                                                                    \
        }                                                                                                                   \
        else                                                                                                                \
        {                                                                                                                   \
            error_printf("Error: %s array does not have an element at index %d (get)\n", #type, (int)index);                \
            return NULL;                                                                                                    \
        }                                                                                                                   \
    }

/**
 * @brief Macro pour déclarer la fonction de modification d'un élément dans un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 * @param element_type Le type de l'élément à modifier
 */
#define MAGIC_ARRAY_FUNC_SET_DEF(type, element_type) void type##_set(type##_t *array, uint32_t index, element_type *element)

#define MAGIC_ARRAY_FUNC_SET(type, element_type)                                                                            \
    MAGIC_ARRAY_FUNC_SET_DEF(type, element_type)                                                                            \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (set)\n", #type);                                                         \
            return;                                                                                                         \
        }                                                                                                                   \
        if (index < array->current_size)                                                                                    \
        {                                                                                                                   \
            if (element != NULL)                                                                                            \
            {                                                                                                               \
                array->array[index] = *element;                                                                             \
            }                                                                                                               \
        }                                                                                                                   \
        else                                                                                                                \
        {                                                                                                                   \
            error_printf("Error: %s array does not have an element at index %d (set)\n", #type, (int)index);                \
        }                                                                                                                   \
    }

/**
 * @brief Macro pour déclarer la fonction de suppression de tous les éléments dans un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 * @note Cette fonction ne libère pas la mémoire des éléments, elle les écrase simplement
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_CLEAR_DEF(type) void type##_clear(type##_t *array)

#define MAGIC_ARRAY_FUNC_CLEAR(type)                                                                                        \
    MAGIC_ARRAY_FUNC_CLEAR_DEF(type)                                                                                        \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (clear)\n", #type);                                                       \
            return;                                                                                                         \
        }                                                                                                                   \
        array->current_size = 0;                                                                                            \
    }

/**
 * @brief Macro pour déclarer la fonction de vérification si un tableau magique est vide
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_IS_EMPTY_DEF(type) bool type##_is_empty(type##_t *array)

#define MAGIC_ARRAY_FUNC_IS_EMPTY(type)                                                                                     \
    MAGIC_ARRAY_FUNC_IS_EMPTY_DEF(type)                                                                                     \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (is_empty)\n", #type);                                                    \
            return true;                                                                                                    \
        }                                                                                                                   \
        return memcmp(array, &(type##_t){0}, sizeof(type##_t)) == 0;                                                        \
    }

/**
 * @brief Macro pour déclarer la fonction de vérification si un tableau magique est plein
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_IS_FULL_DEF(type) bool type##_is_full(type##_t *array)

#define MAGIC_ARRAY_FUNC_IS_FULL(type)                                                                                      \
    MAGIC_ARRAY_FUNC_IS_FULL_DEF(type)                                                                                      \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (is_full)\n", #type);                                                     \
            return false;                                                                                                   \
        }                                                                                                                   \
        return array->current_size == array->max_size;                                                                      \
    }

/**
 * @brief Macro pour déclarer la fonction de taille d'un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_SIZE_DEF(type) uint32_t type##_size(type##_t *array)

#define MAGIC_ARRAY_FUNC_SIZE(type)                                                                                         \
    MAGIC_ARRAY_FUNC_SIZE_DEF(type)                                                                                         \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (size)\n", #type);                                                        \
            return 0;                                                                                                       \
        }                                                                                                                   \
        return array->current_size;                                                                                         \
    }

/**
 * @brief Macro pour déclarer la fonction d'obtention de la taille maximale d'un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 */
#define MAGIC_ARRAY_FUNC_MAX_SIZE_DEF(type) uint32_t type##_max_size(type##_t *array)

#define MAGIC_ARRAY_FUNC_MAX_SIZE(type)                                                                                     \
    MAGIC_ARRAY_FUNC_MAX_SIZE_DEF(type)                                                                                     \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (max_size)\n", #type);                                                    \
            return 0;                                                                                                       \
        }                                                                                                                   \
        return array->max_size;                                                                                             \
    }

/**
 * @brief Macro pour déclarer la fonction de vérification si un tableau magique contient un élément
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 * @param element_type Le type de l'élément à lire
 */
#define MAGIC_ARRAY_FUNC_CONTAINS_DEF(type, element_type) bool type##_contains(type##_t *array, element_type *element)

#define MAGIC_ARRAY_FUNC_CONTAINS(type, element_type)                                                                       \
    MAGIC_ARRAY_FUNC_CONTAINS_DEF(type, element_type)                                                                       \
    {                                                                                                                       \
        if (array == NULL || element == NULL)                                                                               \
        {                                                                                                                   \
            error_printf("Error: %s array or element is NULL (contains)\n", #type);                                         \
            return false;                                                                                                   \
        }                                                                                                                   \
        for (uint32_t i = 0; i < array->current_size; i++)                                                                  \
        {                                                                                                                   \
            if (memcmp(&array->array[i], element, sizeof(element_type)) == 0)                                               \
            {                                                                                                               \
                return true;                                                                                                \
            }                                                                                                               \
        }                                                                                                                   \
        return false;                                                                                                       \
    }

/**
 * @brief Macro pour déclarer la fonction d'obtention de l'index d'un élément dans un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 * @param element_type Le type de l'élément à lire
 */
#define MAGIC_ARRAY_FUNC_INDEX_OF_DEF(type, element_type) uint32_t type##_index_of(type##_t *array, element_type *element)

#define MAGIC_ARRAY_FUNC_INDEX_OF(type, element_type)                                                                       \
    MAGIC_ARRAY_FUNC_INDEX_OF_DEF(type, element_type)                                                                       \
    {                                                                                                                       \
        if (array == NULL || element == NULL)                                                                               \
        {                                                                                                                   \
            error_printf("Error: %s array or element is NULL (index_of)\n", #type);                                         \
            return MAGIC_ARRAY_NOT_FOUND;                                                                                   \
        }                                                                                                                   \
        for (uint32_t i = 0; i < array->current_size; i++)                                                                  \
        {                                                                                                                   \
            if (memcmp(&array->array[i], element, sizeof(element_type)) == 0)                                               \
            {                                                                                                               \
                return i;                                                                                                   \
            }                                                                                                               \
        }                                                                                                                   \
        return MAGIC_ARRAY_NOT_FOUND;                                                                                       \
    }

/**
 * @brief Macro pour déclarer la fonction d'obtention de l'index du dernier élément d'un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 * @param element_type Le type de l'élément à lire
 */
#define MAGIC_ARRAY_FUNC_LAST_INDEX_OF_DEF(type, element_type)                                                              \
    uint32_t type##_last_index_of(type##_t *array, element_type *element)

#define MAGIC_ARRAY_FUNC_LAST_INDEX_OF(type, element_type)                                                                  \
    MAGIC_ARRAY_FUNC_LAST_INDEX_OF_DEF(type, element_type)                                                                  \
    {                                                                                                                       \
        if (array == NULL || element == NULL)                                                                               \
        {                                                                                                                   \
            error_printf("Error: %s array or element is NULL (last_index_of)\n", #type);                                    \
            return MAGIC_ARRAY_NOT_FOUND;                                                                                   \
        }                                                                                                                   \
        for (uint32_t i = array->current_size - 1; i >= 0; i--)                                                             \
        {                                                                                                                   \
            if (memcmp(&array->array[i], element, sizeof(element_type)) == 0)                                               \
            {                                                                                                               \
                return i;                                                                                                   \
            }                                                                                                               \
        }                                                                                                                   \
        return MAGIC_ARRAY_NOT_FOUND;                                                                                       \
    }

/**
 * @brief Macro pour déclarer la fonction de copie d'un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_COPY_DEF(type) void type##_copy(type##_t *array, type##_t *copy)

#define MAGIC_ARRAY_FUNC_COPY(type)                                                                                         \
    MAGIC_ARRAY_FUNC_COPY_DEF(type)                                                                                         \
    {                                                                                                                       \
        if (array == NULL || copy == NULL)                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array or copy is NULL (copy)\n", #type);                                                \
            return;                                                                                                         \
        }                                                                                                                   \
        for (uint32_t i = 0; i < array->current_size; i++)                                                                  \
        {                                                                                                                   \
            copy->array[i] = array->array[i];                                                                               \
        }                                                                                                                   \
        copy->current_size = array->current_size;                                                                           \
    }

/**
 * @brief Macro pour déclarer la fonction de réinitialisation de la taille d'un tableau magique
 * @note Cette macro doit être utilisé dans le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 */
#define MAGIC_ARRAY_FUNC_RESET_DEF(type) void type##_reset(type##_t *array, uint32_t last_keep_index)

#define MAGIC_ARRAY_RESET(type)                                                                                             \
    MAGIC_ARRAY_FUNC_RESET_DEF(type)                                                                                        \
    {                                                                                                                       \
        if (array == NULL)                                                                                                  \
        {                                                                                                                   \
            error_printf("Error: %s array is NULL (reset)\n", #type);                                                       \
            return;                                                                                                         \
        }                                                                                                                   \
        if (last_keep_index >= array->current_size)                                                                         \
        {                                                                                                                   \
            error_printf("Error: %s array last keep index is greater than current size\n", #type);                          \
            return;                                                                                                         \
        }                                                                                                                   \
        array->current_size = last_keep_index + 1;                                                                          \
    }

/**
 * @brief Macro pour définir le type d'un tableau magique
 * @note Cette macro peut être utilisé dans le .h ou le .c
 *
 * @param type Le type des éléments du tableau
 * (ne pas suffixer par _t, c'est automatique)
 * @param element_type Le type des éléments du tableau
 * @param _max_size La taille maximale du tableau
 */
#define MAGIC_ARRAY_TYPEDEF(type, element_type, _max_size)                                                                  \
    typedef struct                                                                                                          \
    {                                                                                                                       \
        element_type array[_max_size];                                                                                      \
        uint32_t max_size;                                                                                                  \
        uint32_t current_size;                                                                                              \
    } type##_t;

/**
 * @brief Macro pour déclarer les fonctions d'un tableau magique
 * @note Cette macro doit être utilisé dans un .c
 *
 * @param type Le type des éléments du tableau
 * ne pas suffixer par _t, c'est automatique
 * doit être le même que celui utilisé dans MAGIC_ARRAY_TYPEDEF
 */
#define MAGIC_ARRAY_SRC(type, element_type)                                                                                 \
    MAGIC_ARRAY_FUNC_INIT(type)                                                                                             \
    MAGIC_ARRAY_FUNC_ADD(type, element_type)                                                                                \
    MAGIC_ARRAY_FUNC_REMOVE(type)                                                                                           \
    MAGIC_ARRAY_FUNC_GET(type, element_type)                                                                                \
    MAGIC_ARRAY_FUNC_SET(type, element_type)                                                                                \
    MAGIC_ARRAY_FUNC_CLEAR(type)                                                                                            \
    MAGIC_ARRAY_FUNC_IS_EMPTY(type)                                                                                         \
    MAGIC_ARRAY_FUNC_IS_FULL(type)                                                                                          \
    MAGIC_ARRAY_FUNC_SIZE(type)                                                                                             \
    MAGIC_ARRAY_FUNC_MAX_SIZE(type)                                                                                         \
    MAGIC_ARRAY_FUNC_CONTAINS(type, element_type)                                                                           \
    MAGIC_ARRAY_FUNC_INDEX_OF(type, element_type)                                                                           \
    MAGIC_ARRAY_FUNC_LAST_INDEX_OF(type, element_type)                                                                      \
    MAGIC_ARRAY_FUNC_COPY(type)                                                                                             \
    MAGIC_ARRAY_RESET(type)

/**
 * @brief Macro pour déclarer les fonctions d'un tableau magique dans un header pour les rendre publiques
 * @note Cette macro doit être utilisé dans un .h
 *
 * @param type Le type des éléments du tableau
 * ne pas suffixer par _t, c'est automatique
 * doit être le même que celui utilisé dans MAGIC_ARRAY_TYPEDEF
 */
#define MAGIC_ARRAY_HEADER(type, element_type)                                                                              \
    MAGIC_ARRAY_FUNC_INIT_DEF(type);                                                                                        \
    MAGIC_ARRAY_FUNC_ADD_DEF(type, element_type);                                                                           \
    MAGIC_ARRAY_FUNC_REMOVE_DEF(type);                                                                                      \
    MAGIC_ARRAY_FUNC_GET_DEF(type, element_type);                                                                           \
    MAGIC_ARRAY_FUNC_SET_DEF(type, element_type);                                                                           \
    MAGIC_ARRAY_FUNC_CLEAR_DEF(type);                                                                                       \
    MAGIC_ARRAY_FUNC_IS_EMPTY_DEF(type);                                                                                    \
    MAGIC_ARRAY_FUNC_IS_FULL_DEF(type);                                                                                     \
    MAGIC_ARRAY_FUNC_SIZE_DEF(type);                                                                                        \
    MAGIC_ARRAY_FUNC_MAX_SIZE_DEF(type);                                                                                    \
    MAGIC_ARRAY_FUNC_CONTAINS_DEF(type, element_type);                                                                      \
    MAGIC_ARRAY_FUNC_INDEX_OF_DEF(type, element_type);                                                                      \
    MAGIC_ARRAY_FUNC_LAST_INDEX_OF_DEF(type, element_type);                                                                 \
    MAGIC_ARRAY_FUNC_COPY_DEF(type);                                                                                        \
    MAGIC_ARRAY_FUNC_RESET_DEF(type);

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* QS_MAGIC_ARRAY_H */