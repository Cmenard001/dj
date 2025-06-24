/**
 * @file printfStm32f4.c
 * @brief Printf API for STM32F4 series using UART
 * @author Cyprien Ménard
 * @date 24/06/2025
 * @see printfStm32f4.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "printf.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

/* **************************************************** Private macros ****************(*********************************** */
// Use USART1 for printf redirection
#define PRINTF_USART         USART1
#define PRINTF_USART_CLK     RCC_APB2Periph_USART1
#define PRINTF_USART_GPIO    GPIOB
#define PRINTF_USART_GPIO_CLK RCC_AHB1Periph_GPIOB
#define PRINTF_USART_TX_PIN  GPIO_Pin_6
#define PRINTF_USART_RX_PIN  GPIO_Pin_7
#define PRINTF_USART_TX_AF   GPIO_AF_USART1
#define PRINTF_USART_RX_AF   GPIO_AF_USART1
#define PRINTF_USART_TX_SRC  GPIO_PinSource6
#define PRINTF_USART_RX_SRC  GPIO_PinSource7

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void printfInit()
{
    // Enable clocks
    RCC_AHB1PeriphClockCmd(PRINTF_USART_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(PRINTF_USART_CLK, ENABLE);

    // Configure TX pin as alternate function
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = PRINTF_USART_TX_PIN | PRINTF_USART_RX_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(PRINTF_USART_GPIO, &GPIO_InitStruct);

    // Connect pins to USART1 TX and RX
    GPIO_PinAFConfig(PRINTF_USART_GPIO, PRINTF_USART_TX_SRC, PRINTF_USART_TX_AF);
    GPIO_PinAFConfig(PRINTF_USART_GPIO, PRINTF_USART_RX_SRC, PRINTF_USART_RX_AF);

    // USART configuration
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(PRINTF_USART, &USART_InitStruct);

    USART_Cmd(PRINTF_USART, ENABLE);
}

/* ***************************************** Public callback functions definitions *************************************** */

// Redirect printf to USART1
int __io_putchar(int ch)
{
    // Wait until transmit data register is empty
    while (USART_GetFlagStatus(PRINTF_USART, USART_FLAG_TXE) == RESET);
    USART_SendData(PRINTF_USART, (uint8_t)ch);
    return ch;
}

// For GCC, also redirect fputc
int fputc(int ch, FILE *f)
{
    return __io_putchar(ch);
}

// Implement _write for newlib printf redirection
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        __io_putchar(ptr[i]);
    }
    return len;
}
