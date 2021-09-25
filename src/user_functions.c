#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "user_functions.h"
#include "uart_api.h"
#include "i2c_api.h"
#include "stm32l4xx_hal.h"

#include "max6650.h"

#define COMMANDS_COUNT          4

static MAX6650_Config_t *max6650_config = NULL;

/* MAX6650 I2C external interface configuration */
static const struct MAX6650_I2C_ExtInterface max6650_i2c_ext_interface =
{
    .i2c_setup = I2C_API_Init,
    .i2c_read = I2C_API_ReadMultiple,
    .i2c_write = I2C_API_WriteMultiple
};


/* Prototypes for console commands */
static bool set_fan_speed(int var);
static bool get_fan_speed(int var);
static bool self_erase(int var);
static bool help(int var);


/**
 * List of commands with their names
 */
static Command_t commands_list[COMMANDS_COUNT] = {
    {set_fan_speed,     "set_fan_speed",    ",speed<0..100>"},
    {get_fan_speed,     "get_fan_speed",    ""},
    {self_erase,        "self_erase",       " "TC_RED"*Warning: this operation is irreversible"TC_RESET},
    {help,              "help",             ""}
};


/**
 * Get string for status name
 */
static const char* get_status(bool status)
{
    const char* ok = "OK";
    const char* err = "ERROR";

    return status == true ? ok : err;
}


/**
 * @brief Mass erasing of FLASH from RAM
 */
static __RAM_FUNC void mass_erase_from_ram(void)
{
    char value;
    char * str_no_func = "\r\n"TC_RED"No functional\r\n"TC_RESET;
    int str_no_func_len = strlen(str_no_func);
    char str_no_func_ram[str_no_func_len];
    /* Copy string to RAM before flash erasing */
    memcpy(str_no_func_ram, str_no_func, str_no_func_len);
    
    char * str_info = "\r\n"TC_RED"MCU FLASH was erased. Device is not functional now. It will not start after reset!\r\n"TC_RESET;
    int str_info_len = strlen(str_info);
    char str_info_ram[str_info_len];
    /* Copy string to RAM before flash erasing */
    memcpy(str_info_ram, str_info, str_info_len);

    /* Authorize the FLASH Registers access */
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

    /* Wait for last operation to be completed */
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
    {
        __asm__("nop");
    }

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
 * @param[in] desired speed <0..100%>
 */
static bool set_fan_speed(int set_speed)
{
    uint8_t speed_actual = 0;
    bool res;

    res = MAX6650_SetSpeed(set_speed, &speed_actual);
    printf(TC_RESET"Status: %s\r\n", get_status(res));

    if(res!=false)
    {
        printf(TC_RESET"Set    speed: %d%%\r\n", set_speed);
        printf(TC_RESET"Actual speed: %d%%\r\n", speed_actual);
    }

    return res;
}

/**
 * @brief Handler for "get_fan_speed" command
 * @param[in] not used
 */
static bool get_fan_speed(int var)
{
    uint8_t speed_actual = 0;
    bool res;

    res = MAX6650_GetSpeed(&speed_actual);
    printf(TC_RESET"Status: %s\r\n", get_status(res));

    if(res!=false)
    {
        printf(TC_RESET"Actual speed: %d%%\r\n", speed_actual);
    }

    return res;
}


/**
 * @brief Handler for "self_erase" command
 * @param[in] not used
 */
static bool self_erase(int var)
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

    return true;
}

static bool help(int var)
{
    UartAPI_PrintMenu();
    return true;
}

static bool max6650_init()
{
    bool res;

    max6650_config = (MAX6650_Config_t *) malloc(sizeof(MAX6650_Config_t));
    if(max6650_config == NULL)
    {
        printf(TC_RED"MAX6650 config: no memory\r\n");
        return false;
    }

    /* MAX6650/fan configuration */
    max6650_config->add_line_connection = ADD_Line_GND;
    max6650_config->rpm_max = 10500U;
    max6650_config->fan_lovtage = FanVoltage_12V;
    max6650_config->operating_mode = OperatingMode_Closed_Loop;
    /* Select the KScale value so the fan’s full speed is achieved with a speed register value of approximately 64
     * Fan-Speed Register value (KTACH) may be calculated as:
     * tTACH = 1 / (2 x Fan Speed[RPS])
     * KTACH = [tTACH x KSCALE x (fCLK / 128)] - 1
     *
     * KSCALE=11.5 for 10500 RPM, so choosing scale=KScale_16, in this case KTACH=90 at max speed (should be less than 128)*/
    max6650_config->k_scale = KScale_16;

    res = MAX6650_Init(max6650_config, &max6650_i2c_ext_interface);

    if(res != true)
    {
        printf(TC_RED"MAX6650 initialization error!\r\n");
        return false;
    }

    return true;
}


bool UserFunctions_Init(void)
{
    bool res;

    res = max6650_init();

    return res;
}


inline uint8_t UserFunctions_GetFuncCount()
{
    return COMMANDS_COUNT;
}


Command_t* UserFunctions_GetFunc(uint8_t item)
{
    if(item < COMMANDS_COUNT)
    {
        return commands_list+item;
    }
    else
    {
        return NULL;
    }
}
