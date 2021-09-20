#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "uart_api.h"
#include "stm32l4xx_hal.h"


#define INCOMING_BUFF_LENGTH    64
#define COMMANDS_COUNT          4

extern UART_HandleTypeDef huart1;

typedef struct
{
    const char *command_name;
    int (*run_func)(int);
} Command_t;


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
 * @brief Send char thru UART1 using registers only. The function is executed in RAM
 */
static __RAM_FUNC void UART1_send_char(char c)
{
    /* Load the Data */
    USART1->TDR = c;
    /* Wait for TC to SET. This indicates that the data has been transmitted */
    while (!(USART1->ISR & USART_ISR_TC_Msk))
    {
        __asm__("nop");
    }
}

/**
 * @brief Get incoming char from UART1 using registers only. The function is executed in RAM
 */
static __RAM_FUNC char UART1_get_char(void)
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

/**
 * @brief Send string thru UART1. The function is executed in RAM
 */
static __RAM_FUNC void UART1_send_string(char *c, int len)
{
    for(int i=0; i<len; i++)
    {
        UART1_send_char(*c++);
    }
}

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

    UART1_send_string(str_info, str_info_len);

    /* Handle incoming commands after erasing */
    while(1)
    {
        value = UART1_get_char();
        UART1_send_char(value);
        switch(value)
        {
            case '\r':
            case '\n':
                UART1_send_string(str_no_func, str_no_func_len);
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


