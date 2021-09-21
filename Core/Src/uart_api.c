#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "stm32l4xx_hal.h"
#include "user_functions.h"
#include "uart_api.h"

#define INCOMING_BUFF_LENGTH    64

extern UART_HandleTypeDef huart1;

UART_HandleTypeDef huart1;

/**
 * @brief Custom implementation of WEAK __io_putchar() function from syscallc.c
 */
int __io_putchar(int ch)
{
    if(HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100) != HAL_OK)
    {
        return -1;
    }
    return ch;
}

/**
 * @brief Custom implementation of WEAK __io_getchar() function from syscallc.c
 */
int __io_getchar(void)
{
    char data[4];
    uint8_t ch, len = 1;

    while(HAL_UART_Receive(&huart1, &ch, 1, 0x10) != HAL_OK)
    {
        __asm__("nop");
    }

    memset(data, 0x00, 4);
    switch(ch)
    {
        case '\r':
        case '\n':
            len = 2;
            sprintf(data, "\r\n");
            break;

        case '\b':
        case 0x7F:
            len = 3;
            sprintf(data, "\b \b");
            break;

        default:
            data[0] = ch;
            break;
    }

    HAL_UART_Transmit(&huart1, (uint8_t *)&data, len, 100);
    return ch;
}


void UartAPI_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}


__RAM_FUNC void UartAPI_SendChar(char c)
{
    /* Load the Data */
    USART1->TDR = c;
    /* Wait for TC to SET. This indicates that the data has been transmitted */
    while (!(USART1->ISR & USART_ISR_TC_Msk))
    {
        __asm__("nop");
    }
}


__RAM_FUNC char UartAPI_GetChar(void)
{
    char temp;
    /* Wait for RXNE to SET. This indicates that the data has been Received */
    while (!(USART1->ISR & USART_ISR_RXNE_Msk))
    {
        __asm__("nop");
    }
    /* Read the data.*/
    temp = (char)USART1->RDR;
    return temp;
}


__RAM_FUNC void UartAPI_SendString(char *c, int len)
{
    for(int i=0; i<len; i++)
    {
        UartAPI_SendChar(*c++);
    }
}


void UartAPI_PrintMenu(void)
{
    printf(TC_YELLOW"\r\n\r\nUse next commands to control peripheral devices:\r\n");
    printf(TC_YELLOW"- %s,speed,<0..100>\r\n", commands_list[0].command_name);
    printf(TC_YELLOW"- %s\r\n", commands_list[1].command_name);
    printf(TC_YELLOW"- %s\r\n", commands_list[2].command_name);
    printf(TC_YELLOW"- %s "TC_RED"*Warning: this operation is irreversible\r\n\r\n", commands_list[3].command_name);
}


void UartAPI_WaitForCommandAndExecute(void)
{
    char incom[INCOMING_BUFF_LENGTH];
    int value = -1;
    int res = -1;
    char *p;
    bool command_found = false;

    printf(TC_RESET"Waiting for commands..\r\n\r\n");

    memset(incom, 0 ,INCOMING_BUFF_LENGTH);
    scanf("%s", incom);

    for(int i=0; i<COMMANDS_COUNT; i++)
    {
        p = strstr(incom, commands_list[i].command_name);

        /* Command found */
        if( p != NULL )
        {
            command_found = true;

            p = strstr(incom,",");

            /* Value found */
            if( p != NULL )
            {
                value = atoi(p+1);
            }
            res = commands_list[i].run_func(value);
            if(res != 1)
            {

            }
            else
            {

            }
            break;
        }
    }

    if(!command_found)
    {
        printf(TC_YELLOW"\r\nCommand \"%s\" is not found..\r\n\r\n", incom);
    }
}


