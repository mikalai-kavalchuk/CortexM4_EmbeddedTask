#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "user_functions.h"
#include "uart_api.h"
#include "stm32l4xx_hal.h"


/* Prototypes for console commands */
static int set_fan_speed(int var);
static int get_fan_speed(int var);
static int get_temperature(int var);
static int self_erase(int var);


/**
 * List of commands with their names
 */
Command_t commands_list[COMMANDS_COUNT] = {
    {"set_fan_speed", set_fan_speed},
    {"get_fan_speed", get_fan_speed},
    {"get_temperature", get_temperature},
    {"self_erase", self_erase}
};


/**
 * @brief Mass erasing of FLASH from RAM
 */
static __RAM_FUNC void mass_erase_from_ram(void)
{
    char value;
    char * str_no_func = "\r\n"TC_RED"No functional\r\n"TC_RESET;
    int str_no_func_len = strlen(str_no_func);
    char * str_info = "\r\n"TC_RED"MCU FLASH was erased. Device is not functional now. It will not start after reset!\r\n"TC_RESET;
    int str_info_len = strlen(str_info);

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

    UartAPI_SendString(str_info, str_info_len);

    /* Handle incoming commands after erasing */
    while(1)
    {
        value = UartAPI_GetChar();
        UartAPI_SendChar(value);
        switch(value)
        {
            case '\r':
            case '\n':
                UartAPI_SendString(str_no_func, str_no_func_len);
                break;
            default:
                break;
        }
    }
}

/**
 * @brief Handler for "set_fan_speed" command
 * @param[in] desired speed <0..100>
 */
static int set_fan_speed(int var)
{
    printf(TC_RESET"FUNC: %s, var=%d\r\n", __FUNCTION__, var);
    return 0;
}

/**
 * @brief Handler for "get_fan_speed" command
 * @param[in] not used
 */
static int get_fan_speed(int var)
{
    printf(TC_RESET"FUNC: %s, var=%d\r\n", __FUNCTION__, var);
    return 0;
}

/**
 * @brief Handler for "get_temperature" command
 * @param[in] not used
 */
static int get_temperature(int var)
{
    printf(TC_RESET"FUNC: %s, var=%d\r\n", __FUNCTION__, var);
    return 0;
}

/**
 * @brief Handler for "self_erase" command
 * @param[in] not used
 */
static int self_erase(int var)
{
    char value;
    bool wait_for_op = true;

    printf(TC_RED"*WARNING: this operation is irreversible!\r\n");


    while(wait_for_op)
    {
        printf(TC_YELLOW"\r\nPlease, type [Y] to confirm or [N] to reject the ERASE operation: "TC_RESET);
        fflush(stdout);
        fflush(stdin);
        scanf("%c", &value);
        switch(value)
        {
            case 'y':
            case 'Y':
                printf(TC_YELLOW"\r\nOperation accepted\r\n");
                __disable_irq();
                mass_erase_from_ram();
                break;

            case 'n':
            case 'N':
                printf(TC_YELLOW"\r\nOperation declined\r\n");
                wait_for_op = false;
                break;

            default:
                wait_for_op = true;
                break;
        }
    }

    return 0;
}
