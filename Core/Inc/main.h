/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"

#define ST_LINK_USART1_TX_Pin           GPIO_PIN_6
#define ST_LINK_USART1_TX_GPIO_Port     GPIOB
#define ST_LINK_USART1_RX_Pin           GPIO_PIN_7
#define ST_LINK_USART1_RX_GPIO_Port     GPIOB

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
