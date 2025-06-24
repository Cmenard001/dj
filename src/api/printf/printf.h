/**
 * @file printf.h
 * @brief Printf API
 * @author Cyprien Ménard
 * @date 24/06/2025
 * @see printf.c
 */

#ifndef __PRINTF_H__
#define __PRINTF_H__

/* ******************************************************* Includes ****************************************************** */

#include <stdio.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initializes the printf functionality.
 * This function should be called before using printf
 */
void printfInit();

/* ******************************************* Public callback functions declarations ************************************ */

int __io_putchar(int ch);
int fputc(int ch, FILE *f);
int _write(int file, char *ptr, int len);

#endif /* __PRINTF_H__ */
