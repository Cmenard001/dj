/**
 * @file macros.h
 * @brief Definition of macros for all robot code
 * @author Axel Voitier (et un peu savon aussi)
 * @author Revision 2008-2009 : Jacen
 * @date 2006 - 2010
 * @see macros.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Macro to get the high byte of a 16 bits integer
 * @param x The 16 bits integer
 * @return The high byte of x
 */
#define HIGHINT(x)            (((x) >> 8) & 0xFF)

/**
 * @brief Macro to get the low byte of a 16 bits integer
 * @param x The 16 bits integer
 * @return The low byte of x
 */
#define LOWINT(x)             ((x) & 0xFF)

/**
 * @brief Macro to construct a 16 bits integer from two 8 bits integers
 * @param high The high byte
 * @param low The low byte
 * @return The constructed 16 bits integer
 */
#define U16FROMU8(high, low)  ((((uint16_t)(high)) << 8) | (uint16_t)(low))

/**
 * @brief Macro to construct a 32 bits integer from two 16 bits integers
 * @param high The high word
 * @param low The low word
 * @return The constructed 32 bits integer
 */
#define U32FROMU16(high, low) ((((uint32_t)(high)) << 16) | (uint32_t)(low))

/**
 * @brief Macro to construct a 32 bits integer from four 8 bits integers
 * @param higher The highest byte
 * @param high The high byte
 * @param low The low byte
 * @param lower The lowest byte
 * @return The constructed 32 bits integer
 */
#define U32FROMU8(higher, high, low, lower)                                                        \
    ((((uint32_t)(higher)) << 24) | (((uint32_t)(high)) << 16) | (((uint32_t)(low)) << 8) |        \
     (uint32_t)(lower))

/**
 * @brief Met à 1 les bits de 'var' spécifiés par 'mask'.
 * @param var Variable à modifier
 * @param mask Masque des bits à mettre à 1
 */
#define BITS_ON(var, mask)    ((var) |= (mask))

/* ~0 est le complement à 1 de 0, donc pour 16 bits OxFFFF) */
/* ~0 ^ mask permet d'etre indépendant de la taille (en bits) de ~mask */
/**
 * @brief Met à 0 les bits de 'var' spécifiés par 'mask'.
 * @param var Variable à modifier
 * @param mask Masque des bits à mettre à 0
 */
#define BITS_OFF(var, mask)   ((var) &= ~0 ^ (mask))

/**
 * @brief Met à 1 le bit numéro 'bitno' de 'var'.
 * @param var Variable à modifier
 * @param bitno Numéro du bit à mettre à 1
 */
#define BIT_SET(var, bitno)   ((var) |= (1 << (bitno)))

/**
 * @brief Met à 0 le bit numéro 'bitno' de 'var'.
 * @param var Variable à modifier
 * @param bitno Numéro du bit à mettre à 0
 */
#define BIT_CLR(var, bitno)   ((var) &= ~(1 << (bitno)))

/**
 * @brief Teste si le bit numéro 'bitno' de 'data' est à 1.
 * @param data Donnée à tester
 * @param bitno Numéro du bit à tester
 * @return 1 si le bit est à 1, 0 sinon
 */
#define BIT_TEST(data, bitno) (((data) >> (bitno)) & 0x01)

/**
 * @brief Met à 1 les bits de 'var' spécifiés par 'bits'.
 * @param var Variable à modifier
 * @param bits Bits à mettre à 1
 */
#define BITS_SET(var, bits)   ((var) |= (bits))

/**
 * @brief Met à 0 les bits de 'var' spécifiés par 'bits'.
 * @param var Variable à modifier
 * @param bits Bits à mettre à 0
 */
#define BITS_CLR(var, bits)   ((var) &= ~(bits))

/**
 * @brief Teste si au moins un des bits spécifiés par 'bits' dans 'var' est à 1.
 * @param var Variable à tester
 * @param bits Bits à tester
 * @return Résultat du test (non nul si au moins un bit est à 1)
 */
#define BITS_TEST(var, bits)  ((var) & (bits))

/**
 * @brief Retourne la valeur minimale entre 'a' et 'b'.
 * @param a Première valeur
 * @param b Deuxième valeur
 * @return La plus petite des deux valeurs
 */
#if !defined(MIN)
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif

/**
 * @brief Retourne la valeur maximale entre 'a' et 'b'.
 * @param a Première valeur
 * @param b Deuxième valeur
 * @return La plus grande des deux valeurs
 */
#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 * @brief Retourne le signe de 'a'.
 * @param a Valeur à tester
 * @return 1 si positif, -1 si négatif, 0 si nul
 */
#define SIGN(a)                      ((a > 0) ? 1 : ((a < 0) ? -1 : 0))

/**
 * @brief Arrondit 'a' à l'entier supérieur si ce n'est pas déjà un entier.
 * @param a Valeur à arrondir
 * @return Valeur arrondie
 */
#define AROUND_UP(a)                 ((MAX((a), (int32_t)(a)) == (int32_t)(a)) ? (a) : (a) + 1)

/**
 * @brief Limite 'value_' entre 'min_' et 'max_'.
 * @param value_ Valeur à limiter
 * @param min_ Borne inférieure
 * @param max_ Borne supérieure
 * @return Valeur limitée
 */
#define CLIPPING(value_, min_, max_) (MAX(MIN(value_, max_), min_))

/**
 * @brief Retourne le nombre d'éléments d'un tableau.
 * @param array Tableau
 * @return Nombre d'éléments
 */
#define ARRAY_SIZE(array)            (sizeof(array) / sizeof(array[0]))

/**
 * @brief Calcule le carré de 'x'.
 * @param x Valeur à mettre au carré
 * @return x^2
 */
#define SQUARE(x)                    ((x) * (x))

/**
 * @brief Calcule le cube de 'x'.
 * @param x Valeur à mettre au cube
 * @return x^3
 */
#define POWER3(x)                    ((x) * (x) * (x))

/**
 * @brief Calcule le vrai modulo, toujours positif.
 * @param x Valeur
 * @param N Modulo
 * @return x modulo N, toujours positif
 * @details Vrai modulo, avec un résultat non signé. Car l'opérateur % renvoit le reste signé de la division entière.
 */
#define MODULO(x, N)                 ((x % N + N) % N)

/**
 * @brief Exécute une instruction NOP (No Operation).
 */
#define nop()                        __asm__("nop")

/* la fonction valeur absolue pour des entiers */
/**
 * @brief Calcule la valeur absolue de 'x'.
 * @param x Valeur
 * @return Valeur absolue
 */
#define absolute(x)                  (((x) >= 0) ? (x) : (-(x)))

/**
 * @brief Vérifie si une valeur d'énumération appartient à une famille d'énumération.
 * @param value Valeur à vérifier
 * @param family Famille d'énumération
 */
#define check_enum(value, family)                                                                  \
    if ((value & MASK_ENUM_FAMILY) != family)                                                      \
    {                                                                                              \
        ENUM_notify(__FILE__, __LINE__, (uint32_t)__builtin_return_address(0));                    \
    }

/**
 * @brief Récupère le nom du fichier courant sans le chemin.
 * @note Compartible uniquement avec GCC et Clang. Pour les autres compilateurs, c'est
 * remappé sur le path complet.
 * @note On ESP-IDF platforms, this is defined by assert.h which we include here.
 */
#if defined(ESP_PLATFORM)
// On ESP32, include assert.h to get __FILENAME__ definition
#include <assert.h>
#elif !defined(__FILENAME__)
#if defined(__GNUC__) || defined(__clang__)
#define __FILENAME__                                                                               \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define __FILENAME__ (__FILE__)
#endif // defined(__GNUC__) || defined(__clang__)
#endif // defined(ESP_PLATFORM)

/**
 * @brief Reverse the byte order of a 64-bit integer.
 */
#define __REV64(x)                                                                                 \
    ((((x) & 0xFF00000000000000ULL) >> 56) | (((x) & 0x00FF000000000000ULL) >> 40) |               \
     (((x) & 0x0000FF0000000000ULL) >> 24) | (((x) & 0x000000FF00000000ULL) >> 8) |                \
     (((x) & 0x00000000FF000000ULL) << 8) | (((x) & 0x0000000000FF0000ULL) << 24) |                \
     (((x) & 0x000000000000FF00ULL) << 40) | (((x) & 0x00000000000000FFULL) << 56))

/**
 * @brief Macro pour éviter un warning sur une variable non utilisée.
 * @param x Variable non utilisée
 */
#undef UNUSED
#define UNUSED(x)                   (void)(x);

/**
 * @brief Macro pour éviter un warning sur une fonction non utilisée.
 * @param x Fonction non utilisée
 */
#define UNUSED_FUNCTION(x)          (void)(x);

/**
 * @brief Macro pour convertir un mot en chaîne de caractères.
 */
#define WORD_CONVERT_TO_STRING(val) #val

/**
 * @brief Macro pour retourner le nom d'une valeur d'énumération sous forme de chaîne dans un switch.
 */
#define SWITCH_RET_STRING(x)                                                                       \
    case x:                                                                                        \
        return #x

/**
 * @brief Crée une typedef enum de nom 'enum_name' et une tableau de chaine de caractère de nom 'string_name'
 * contenant toutes les valeurs qui suivent
 */
#define TYPEDEF_ENUM_WITH_STRING(enum_name, type_name, ...)                                        \
    typedef enum                                                                                   \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    } type_name;                                                                                   \
    static const char *enum_name##_names[] = {FOREACH(WORD_CONVERT_TO_STRING, __VA_ARGS__)};       \
    static inline const char *enum_name##_get_string(type_name value)                              \
    {                                                                                              \
        if ((int)value < 0 ||                                                                      \
            (int)value >= (int)(sizeof(enum_name##_names) / sizeof(enum_name##_names[0])))         \
        {                                                                                          \
            return "UNKNOWN";                                                                      \
        }                                                                                          \
        return enum_name##_names[(int)value];                                                      \
    }

/**
 * @brief Macro pour compter le nombre d'arguments.
 */
#define PP_NARG(...)  PP_NARG_(__VA_ARGS__, PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)

/**
 * @brief Macro helper for PP_NARG
 * @note This macro supports a maximum number of arguments. If you need more, just extend the lists.
 */
// clang-format off
#define PP_ARG_N( \
    _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,_64,_65,_66,_67,_68,_69,_70, \
    _71,_72,_73,_74,_75,_76,_77,_78,_79,_80, \
    _81,_82,_83,_84,_85,_86,_87,_88,_89,_90, \
    _91,_92,_93,_94,_95,_96,_97,_98,_99,_100, \
    _101,_102,_103,_104,_105,_106,_107,_108,_109,N,...) N
// clang-format on

/**
 * @brief Macro helper for PP_NARG
 * @note This macro supports a maximum number of arguments. If you need more, just extend the lists.
 */
#define PP_RSEQ_N()                                                                                \
    109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89,  \
        88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67,    \
        66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45,    \
        44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23,    \
        22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/**
 * @brief Macro helper for FOREACH
 */
#define FOREACH_1(macro, val)        macro(val)
#define FOREACH_2(macro, val, ...)   macro(val), FOREACH_1(macro, ##__VA_ARGS__)
#define FOREACH_3(macro, val, ...)   macro(val), FOREACH_2(macro, ##__VA_ARGS__)
#define FOREACH_4(macro, val, ...)   macro(val), FOREACH_3(macro, ##__VA_ARGS__)
#define FOREACH_5(macro, val, ...)   macro(val), FOREACH_4(macro, ##__VA_ARGS__)
#define FOREACH_6(macro, val, ...)   macro(val), FOREACH_5(macro, ##__VA_ARGS__)
#define FOREACH_7(macro, val, ...)   macro(val), FOREACH_6(macro, ##__VA_ARGS__)
#define FOREACH_8(macro, val, ...)   macro(val), FOREACH_7(macro, ##__VA_ARGS__)
#define FOREACH_9(macro, val, ...)   macro(val), FOREACH_8(macro, ##__VA_ARGS__)
#define FOREACH_10(macro, val, ...)  macro(val), FOREACH_9(macro, ##__VA_ARGS__)
#define FOREACH_11(macro, val, ...)  macro(val), FOREACH_10(macro, ##__VA_ARGS__)
#define FOREACH_12(macro, val, ...)  macro(val), FOREACH_11(macro, ##__VA_ARGS__)
#define FOREACH_13(macro, val, ...)  macro(val), FOREACH_12(macro, ##__VA_ARGS__)
#define FOREACH_14(macro, val, ...)  macro(val), FOREACH_13(macro, ##__VA_ARGS__)
#define FOREACH_15(macro, val, ...)  macro(val), FOREACH_14(macro, ##__VA_ARGS__)
#define FOREACH_16(macro, val, ...)  macro(val), FOREACH_15(macro, ##__VA_ARGS__)
#define FOREACH_17(macro, val, ...)  macro(val), FOREACH_16(macro, ##__VA_ARGS__)
#define FOREACH_18(macro, val, ...)  macro(val), FOREACH_17(macro, ##__VA_ARGS__)
#define FOREACH_19(macro, val, ...)  macro(val), FOREACH_18(macro, ##__VA_ARGS__)
#define FOREACH_20(macro, val, ...)  macro(val), FOREACH_19(macro, ##__VA_ARGS__)
#define FOREACH_21(macro, val, ...)  macro(val), FOREACH_20(macro, ##__VA_ARGS__)
#define FOREACH_22(macro, val, ...)  macro(val), FOREACH_21(macro, ##__VA_ARGS__)
#define FOREACH_23(macro, val, ...)  macro(val), FOREACH_22(macro, ##__VA_ARGS__)
#define FOREACH_24(macro, val, ...)  macro(val), FOREACH_23(macro, ##__VA_ARGS__)
#define FOREACH_25(macro, val, ...)  macro(val), FOREACH_24(macro, ##__VA_ARGS__)
#define FOREACH_26(macro, val, ...)  macro(val), FOREACH_25(macro, ##__VA_ARGS__)
#define FOREACH_27(macro, val, ...)  macro(val), FOREACH_26(macro, ##__VA_ARGS__)
#define FOREACH_28(macro, val, ...)  macro(val), FOREACH_27(macro, ##__VA_ARGS__)
#define FOREACH_29(macro, val, ...)  macro(val), FOREACH_28(macro, ##__VA_ARGS__)
#define FOREACH_30(macro, val, ...)  macro(val), FOREACH_29(macro, ##__VA_ARGS__)
#define FOREACH_31(macro, val, ...)  macro(val), FOREACH_30(macro, ##__VA_ARGS__)
#define FOREACH_32(macro, val, ...)  macro(val), FOREACH_31(macro, ##__VA_ARGS__)
#define FOREACH_33(macro, val, ...)  macro(val), FOREACH_32(macro, ##__VA_ARGS__)
#define FOREACH_34(macro, val, ...)  macro(val), FOREACH_33(macro, ##__VA_ARGS__)
#define FOREACH_35(macro, val, ...)  macro(val), FOREACH_34(macro, ##__VA_ARGS__)
#define FOREACH_36(macro, val, ...)  macro(val), FOREACH_35(macro, ##__VA_ARGS__)
#define FOREACH_37(macro, val, ...)  macro(val), FOREACH_36(macro, ##__VA_ARGS__)
#define FOREACH_38(macro, val, ...)  macro(val), FOREACH_37(macro, ##__VA_ARGS__)
#define FOREACH_39(macro, val, ...)  macro(val), FOREACH_38(macro, ##__VA_ARGS__)
#define FOREACH_40(macro, val, ...)  macro(val), FOREACH_39(macro, ##__VA_ARGS__)
#define FOREACH_41(macro, val, ...)  macro(val), FOREACH_40(macro, ##__VA_ARGS__)
#define FOREACH_42(macro, val, ...)  macro(val), FOREACH_41(macro, ##__VA_ARGS__)
#define FOREACH_43(macro, val, ...)  macro(val), FOREACH_42(macro, ##__VA_ARGS__)
#define FOREACH_44(macro, val, ...)  macro(val), FOREACH_43(macro, ##__VA_ARGS__)
#define FOREACH_45(macro, val, ...)  macro(val), FOREACH_44(macro, ##__VA_ARGS__)
#define FOREACH_46(macro, val, ...)  macro(val), FOREACH_45(macro, ##__VA_ARGS__)
#define FOREACH_47(macro, val, ...)  macro(val), FOREACH_46(macro, ##__VA_ARGS__)
#define FOREACH_48(macro, val, ...)  macro(val), FOREACH_47(macro, ##__VA_ARGS__)
#define FOREACH_49(macro, val, ...)  macro(val), FOREACH_48(macro, ##__VA_ARGS__)
#define FOREACH_50(macro, val, ...)  macro(val), FOREACH_49(macro, ##__VA_ARGS__)
#define FOREACH_51(macro, val, ...)  macro(val), FOREACH_50(macro, ##__VA_ARGS__)
#define FOREACH_52(macro, val, ...)  macro(val), FOREACH_51(macro, ##__VA_ARGS__)
#define FOREACH_53(macro, val, ...)  macro(val), FOREACH_52(macro, ##__VA_ARGS__)
#define FOREACH_54(macro, val, ...)  macro(val), FOREACH_53(macro, ##__VA_ARGS__)
#define FOREACH_55(macro, val, ...)  macro(val), FOREACH_54(macro, ##__VA_ARGS__)
#define FOREACH_56(macro, val, ...)  macro(val), FOREACH_55(macro, ##__VA_ARGS__)
#define FOREACH_57(macro, val, ...)  macro(val), FOREACH_56(macro, ##__VA_ARGS__)
#define FOREACH_58(macro, val, ...)  macro(val), FOREACH_57(macro, ##__VA_ARGS__)
#define FOREACH_59(macro, val, ...)  macro(val), FOREACH_58(macro, ##__VA_ARGS__)
#define FOREACH_60(macro, val, ...)  macro(val), FOREACH_59(macro, ##__VA_ARGS__)
#define FOREACH_61(macro, val, ...)  macro(val), FOREACH_60(macro, ##__VA_ARGS__)
#define FOREACH_62(macro, val, ...)  macro(val), FOREACH_61(macro, ##__VA_ARGS__)
#define FOREACH_63(macro, val, ...)  macro(val), FOREACH_62(macro, ##__VA_ARGS__)
#define FOREACH_64(macro, val, ...)  macro(val), FOREACH_63(macro, ##__VA_ARGS__)
#define FOREACH_65(macro, val, ...)  macro(val), FOREACH_64(macro, ##__VA_ARGS__)
#define FOREACH_66(macro, val, ...)  macro(val), FOREACH_65(macro, ##__VA_ARGS__)
#define FOREACH_67(macro, val, ...)  macro(val), FOREACH_66(macro, ##__VA_ARGS__)
#define FOREACH_68(macro, val, ...)  macro(val), FOREACH_67(macro, ##__VA_ARGS__)
#define FOREACH_69(macro, val, ...)  macro(val), FOREACH_68(macro, ##__VA_ARGS__)
#define FOREACH_70(macro, val, ...)  macro(val), FOREACH_69(macro, ##__VA_ARGS__)
#define FOREACH_71(macro, val, ...)  macro(val), FOREACH_70(macro, ##__VA_ARGS__)
#define FOREACH_72(macro, val, ...)  macro(val), FOREACH_71(macro, ##__VA_ARGS__)
#define FOREACH_73(macro, val, ...)  macro(val), FOREACH_72(macro, ##__VA_ARGS__)
#define FOREACH_74(macro, val, ...)  macro(val), FOREACH_73(macro, ##__VA_ARGS__)
#define FOREACH_75(macro, val, ...)  macro(val), FOREACH_74(macro, ##__VA_ARGS__)
#define FOREACH_76(macro, val, ...)  macro(val), FOREACH_75(macro, ##__VA_ARGS__)
#define FOREACH_77(macro, val, ...)  macro(val), FOREACH_76(macro, ##__VA_ARGS__)
#define FOREACH_78(macro, val, ...)  macro(val), FOREACH_77(macro, ##__VA_ARGS__)
#define FOREACH_79(macro, val, ...)  macro(val), FOREACH_78(macro, ##__VA_ARGS__)
#define FOREACH_80(macro, val, ...)  macro(val), FOREACH_79(macro, ##__VA_ARGS__)
#define FOREACH_81(macro, val, ...)  macro(val), FOREACH_80(macro, ##__VA_ARGS__)
#define FOREACH_82(macro, val, ...)  macro(val), FOREACH_81(macro, ##__VA_ARGS__)
#define FOREACH_83(macro, val, ...)  macro(val), FOREACH_82(macro, ##__VA_ARGS__)
#define FOREACH_84(macro, val, ...)  macro(val), FOREACH_83(macro, ##__VA_ARGS__)
#define FOREACH_85(macro, val, ...)  macro(val), FOREACH_84(macro, ##__VA_ARGS__)
#define FOREACH_86(macro, val, ...)  macro(val), FOREACH_85(macro, ##__VA_ARGS__)
#define FOREACH_87(macro, val, ...)  macro(val), FOREACH_86(macro, ##__VA_ARGS__)
#define FOREACH_88(macro, val, ...)  macro(val), FOREACH_87(macro, ##__VA_ARGS__)
#define FOREACH_89(macro, val, ...)  macro(val), FOREACH_88(macro, ##__VA_ARGS__)
#define FOREACH_90(macro, val, ...)  macro(val), FOREACH_89(macro, ##__VA_ARGS__)
#define FOREACH_91(macro, val, ...)  macro(val), FOREACH_90(macro, ##__VA_ARGS__)
#define FOREACH_92(macro, val, ...)  macro(val), FOREACH_91(macro, ##__VA_ARGS__)
#define FOREACH_93(macro, val, ...)  macro(val), FOREACH_92(macro, ##__VA_ARGS__)
#define FOREACH_94(macro, val, ...)  macro(val), FOREACH_93(macro, ##__VA_ARGS__)
#define FOREACH_95(macro, val, ...)  macro(val), FOREACH_94(macro, ##__VA_ARGS__)
#define FOREACH_96(macro, val, ...)  macro(val), FOREACH_95(macro, ##__VA_ARGS__)
#define FOREACH_97(macro, val, ...)  macro(val), FOREACH_96(macro, ##__VA_ARGS__)
#define FOREACH_98(macro, val, ...)  macro(val), FOREACH_97(macro, ##__VA_ARGS__)
#define FOREACH_99(macro, val, ...)  macro(val), FOREACH_98(macro, ##__VA_ARGS__)
#define FOREACH_100(macro, val, ...) macro(val), FOREACH_99(macro, ##__VA_ARGS__)
#define FOREACH_101(macro, val, ...) macro(val), FOREACH_100(macro, ##__VA_ARGS__)
#define FOREACH_102(macro, val, ...) macro(val), FOREACH_101(macro, ##__VA_ARGS__)
#define FOREACH_103(macro, val, ...) macro(val), FOREACH_102(macro, ##__VA_ARGS__)
#define FOREACH_104(macro, val, ...) macro(val), FOREACH_103(macro, ##__VA_ARGS__)
#define FOREACH_105(macro, val, ...) macro(val), FOREACH_104(macro, ##__VA_ARGS__)
#define FOREACH_106(macro, val, ...) macro(val), FOREACH_105(macro, ##__VA_ARGS__)
#define FOREACH_107(macro, val, ...) macro(val), FOREACH_106(macro, ##__VA_ARGS__)
#define FOREACH_108(macro, val, ...) macro(val), FOREACH_107(macro, ##__VA_ARGS__)
#define FOREACH_109(macro, val, ...) macro(val), FOREACH_108(macro, ##__VA_ARGS__)

/**
 * @brief Macro helper for string concatenation
 */
#define CONCATENATE(arg1, arg2)      CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)     CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)     arg1##arg2

/**
 * @brief Macro helper for FOREACH
 */
#define FOREACH_(N, macro, ...)      CONCATENATE(FOREACH_, N)(macro, __VA_ARGS__)

/**
 * @brief Macro to apply a macro to each argument
 */
#define FOREACH(macro, ...)          FOREACH_(PP_NARG(__VA_ARGS__), macro, __VA_ARGS__)

/**
 * @brief Macro helper for FOREACH_OR
 */
// clang-format off
#define FOREACH_OR_1(macro, val)         macro(val)
#define FOREACH_OR_2(macro, val, ...)    macro(val) | FOREACH_OR_1(macro, ##__VA_ARGS__)
#define FOREACH_OR_3(macro, val, ...)    macro(val) | FOREACH_OR_2(macro, ##__VA_ARGS__)
#define FOREACH_OR_4(macro, val, ...)    macro(val) | FOREACH_OR_3(macro, ##__VA_ARGS__)
#define FOREACH_OR_5(macro, val, ...)    macro(val) | FOREACH_OR_4(macro, ##__VA_ARGS__)
#define FOREACH_OR_6(macro, val, ...)    macro(val) | FOREACH_OR_5(macro, ##__VA_ARGS__)
#define FOREACH_OR_7(macro, val, ...)    macro(val) | FOREACH_OR_6(macro, ##__VA_ARGS__)
#define FOREACH_OR_8(macro, val, ...)    macro(val) | FOREACH_OR_7(macro, ##__VA_ARGS__)
#define FOREACH_OR_9(macro, val, ...)    macro(val) | FOREACH_OR_8(macro, ##__VA_ARGS__)
#define FOREACH_OR_10(macro, val, ...)   macro(val) | FOREACH_OR_9(macro, ##__VA_ARGS__)
#define FOREACH_OR_11(macro, val, ...)   macro(val) | FOREACH_OR_10(macro, ##__VA_ARGS__)
#define FOREACH_OR_12(macro, val, ...)   macro(val) | FOREACH_OR_11(macro, ##__VA_ARGS__)
#define FOREACH_OR_13(macro, val, ...)   macro(val) | FOREACH_OR_12(macro, ##__VA_ARGS__)
#define FOREACH_OR_14(macro, val, ...)   macro(val) | FOREACH_OR_13(macro, ##__VA_ARGS__)
#define FOREACH_OR_15(macro, val, ...)   macro(val) | FOREACH_OR_14(macro, ##__VA_ARGS__)
#define FOREACH_OR_16(macro, val, ...)   macro(val) | FOREACH_OR_15(macro, ##__VA_ARGS__)
#define FOREACH_OR_17(macro, val, ...)   macro(val) | FOREACH_OR_16(macro, ##__VA_ARGS__)
#define FOREACH_OR_18(macro, val, ...)   macro(val) | FOREACH_OR_17(macro, ##__VA_ARGS__)
#define FOREACH_OR_19(macro, val, ...)   macro(val) | FOREACH_OR_18(macro, ##__VA_ARGS__)
#define FOREACH_OR_20(macro, val, ...)   macro(val) | FOREACH_OR_19(macro, ##__VA_ARGS__)
#define FOREACH_OR_21(macro, val, ...)   macro(val) | FOREACH_OR_20(macro, ##__VA_ARGS__)
#define FOREACH_OR_22(macro, val, ...)   macro(val) | FOREACH_OR_21(macro, ##__VA_ARGS__)
#define FOREACH_OR_23(macro, val, ...)   macro(val) | FOREACH_OR_22(macro, ##__VA_ARGS__)
#define FOREACH_OR_24(macro, val, ...)   macro(val) | FOREACH_OR_23(macro, ##__VA_ARGS__)
#define FOREACH_OR_25(macro, val, ...)   macro(val) | FOREACH_OR_24(macro, ##__VA_ARGS__)
#define FOREACH_OR_26(macro, val, ...)   macro(val) | FOREACH_OR_25(macro, ##__VA_ARGS__)
#define FOREACH_OR_27(macro, val, ...)   macro(val) | FOREACH_OR_26(macro, ##__VA_ARGS__)
#define FOREACH_OR_28(macro, val, ...)   macro(val) | FOREACH_OR_27(macro, ##__VA_ARGS__)
#define FOREACH_OR_29(macro, val, ...)   macro(val) | FOREACH_OR_28(macro, ##__VA_ARGS__)
#define FOREACH_OR_30(macro, val, ...)   macro(val) | FOREACH_OR_29(macro, ##__VA_ARGS__)
#define FOREACH_OR_31(macro, val, ...)   macro(val) | FOREACH_OR_30(macro, ##__VA_ARGS__)
#define FOREACH_OR_32(macro, val, ...)   macro(val) | FOREACH_OR_31(macro, ##__VA_ARGS__)
// clang-format on

/**
 * @brief Macro helper for FOREACH_OR
 */
#define FOREACH_OR_(N, macro, ...)   CONCATENATE(FOREACH_OR_, N)(macro, __VA_ARGS__)

/**
 * @brief Macro to apply a macro to each argument and bitwise-OR the results
 */
#define FOREACH_OR(macro, ...)       FOREACH_OR_(PP_NARG(__VA_ARGS__), macro, __VA_ARGS__)

#ifndef __unused
#define __unused __attribute__((unused))
#endif

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */
