/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_API_H
#define __UART_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "termcolor.h"

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

