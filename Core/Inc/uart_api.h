/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_API_H
#define __UART_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "termcolor.h"


void UartAPI_PrintMenu(void);

void UartAPI_WaitForCommand(void);


#ifdef __cplusplus
}
#endif

#endif /* __UART_API_H */

