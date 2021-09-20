#include "uart_api.h"
#include "stm32l4xx_hal.h"

extern UART_HandleTypeDef huart1;

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

/**
 * @brief Mass erasing of FLASH from RAM
 */
__RAM_FUNC void mass_erase_from_ram(void)
{
    /* Authorize the FLASH Registers access */
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

    /* Wait for last operation to be completed */
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
    {
        __asm__("nop");
    }

    /* Disable instruction cache  */
     __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

     /* Disable data cache  */
    __HAL_FLASH_DATA_CACHE_DISABLE();

    /* Set the Mass Erase Bit for the bank 1 */
    SET_BIT(FLASH->CR, FLASH_CR_MER1);

    /* Set the Mass Erase Bit for the bank 2 */
    SET_BIT(FLASH->CR, FLASH_CR_MER2);

    /* Proceed to erase all sectors */
    SET_BIT(FLASH->CR, FLASH_CR_STRT);

    /* Wait for last operation to be completed */
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
    {
        __asm__("nop");
    }

    /* Set the LOCK Bit to lock the FLASH Registers access */
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);

    while(1)
    {
        __asm__("nop");
    }
}




