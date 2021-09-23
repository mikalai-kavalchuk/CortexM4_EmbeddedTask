#ifndef __UART_API_H
#define __UART_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "termcolor.h"


/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void UartAPI_Init(void);

/**
 * @brief Send char thru UART using registers only. The function is executed in RAM after "self_erase"
 */
void UartAPI_SendChar(char c);

/**
 * @brief Get incoming char from UART using registers only. The function is executed in RAM after "self_erase"
 */
char UartAPI_GetChar(void);

/**
 * @brief Send string thru UART. The function is executed in RAM after "self_erase"
 */
void UartAPI_SendString(char *c, int len);

/**
 * @brief Print menu with information about commands
 */
void UartAPI_PrintMenu(void);

/**
 * @brief Waits for incoming command and execute it
 */
void UartAPI_WaitForCommandAndExecute(void);


#ifdef __cplusplus
}
#endif

#endif /* __UART_API_H */

