#include "uart_api.h"
#include "stm32l4xx_hal.h"



/**
 * @brief Mass erasing of FLASH from RAM
 */
RAMFUNC void mass_erase_from_ram(void)
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

    }
}

