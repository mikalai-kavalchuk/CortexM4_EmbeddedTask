/*
 Set the fan speed to the specified RPM (or read back the RPM setting).

 The MAX6650/1 will automatically control fan speed when in closed loop
 mode.

 Assumptions:

 1) The MAX6650/1 is running from its internal 254kHz clock (perhaps
    this should be made a module parameter).

 2) The prescaler (low three bits of the config register) has already
    been set to an appropriate value.

 The relevant equations are given on pages 21 and 22 of the datasheet.

 From the datasheet, the relevant equation when in regulation is:

    [fCLK / (128 x (KTACH + 1))] = 2 x FanSpeed / KSCALE

 where:

    fCLK is the oscillator frequency (either the 254kHz internal
         oscillator or the externally applied clock)

    KTACH is the value in the speed register

    FanSpeed is the speed of the fan in rps

    KSCALE is the prescaler value (1, 2, 4, 8, or 16)

 When reading, we need to solve for FanSpeed :

      FanSpeed = (tacho / 2) x count_t

      then multiply by 60 to give fanspeed in rpm

 When writing, we need to solve for KTACH, using the datasheet equation:

    Divide the required speed by 60 to get from rpm to rps

      KTACH = [(fCLK x KSCALE) / (256 x FanSpeed)] - 1

    If the internal oscillator is used, setting fCLK to 254kHz
    can further reduce the equations:

      Divide the required speed by 60 to get from rpm to rps

      KTACH = (992 x KSCALE / FanSpeed) - 1

 Note: this tachometer is completely separate from the tachometers
 used to measure the fan speeds. Only one fan's speed (fan1) is
 controlled.
*/

#include <stdio.h>
#include "max6650.h"

#define MAX6650_SPEED_REG               0b00000000     /* fan speed R/W */
#define MAX6650_CONFIG_REG              0b00000010     /* configuration R/W */
#define MAX6650_GPIODEF_REG             0b00000100     /* GPIO definition R/W */
#define MAX6650_DAC_REG                 0b00000110     /* DAC R/W */
#define MAX6650_ALARMENABLE_REG         0b00001000     /* alarm enable R/W */
#define MAX6650_ALARM_REG               0b00001010     /* alarm status R */
#define MAX6650_TACHO_0_REG             0b00001100     /* tachometer 0 count R */
#define MAX6650_GPIOSTAT_REG            0b00010100     /* GPIO status R */
#define MAX6650_COUNT_REG               0b00010110     /* tachometer count time R/W */

#define I2C_ADDRESS_GND                 0x90
#define I2C_ADDRESS_VCC                 0x96
#define I2C_ADDRESS_NOT_CONNECTED       0x36
#define I2C_ADDRESS_RES10K              0x3E

#define COUNTT                          2               /* default count time */

static const struct MAX6650_I2C_ExtInterface *i2c_ext_if = NULL;
static MAX6650_Config_t *config = NULL;
static uint8_t i2c_address;


static uint8_t get_scale(MAX6650_KScale_t k_scale)
{
    uint res = KScale_4;
    switch(k_scale)
    {
        case KScale_1: res = 1; break;
        case KScale_2: res = 2; break;
        case KScale_4: res = 4; break;
        case KScale_8: res = 8; break;
        case KScale_16: res = 16; break;
        default: break;
    }
    return res;
}

bool MAX6650_Init(MAX6650_Config_t *max6650_config, const struct MAX6650_I2C_ExtInterface *ext_i2c_interface)
{
    bool res;
    uint8_t config_byte;
    i2c_ext_if = ext_i2c_interface;
    config = max6650_config;

    if(i2c_ext_if == NULL)
    {
        printf("MAX6650: External I2C interface is not initialized!\r\n");
        return false;
    }

    if(config == NULL)
    {
        printf("MAX6650: no configuration available!\r\n");
        return false;
    }

    switch(config->add_line_connection)
    {
        case ADD_Line_GND:
            i2c_address = I2C_ADDRESS_GND;
            break;
        case ADD_Line_Vcc:
            i2c_address = I2C_ADDRESS_VCC;
            break;
        case ADD_Line_NotConnected:
            i2c_address = I2C_ADDRESS_NOT_CONNECTED;
            break;
        case ADD_Line_Res10K:
            i2c_address = I2C_ADDRESS_RES10K;
            break;
        default:
            printf("MAX6650: I2C address configuration failed!\r\n");
            return false;
    }

    config_byte = (config->operating_mode&0x03)<<4 | (config->fan_lovtage&0x01)<<3 | (config->k_scale&0x07);

    res = i2c_ext_if->i2c_write(i2c_address, MAX6650_CONFIG_REG, &config_byte, 1);

    if(res == true)
    {
        config_byte = COUNTT;
        res = i2c_ext_if->i2c_write(i2c_address, MAX6650_COUNT_REG, &config_byte, 1);
    }

    return res;
}


bool MAX6650_GetSpeed(uint8_t *speed)
{
    uint8_t rps;
    bool res;

    res = i2c_ext_if->i2c_read(i2c_address, MAX6650_TACHO_0_REG, &rps, 1);

    if(res == true)
    {
        rps = ((rps /2)/COUNTT);
        *speed =  (uint32_t)rps * 60 * 100 / config->rpm_max;
    }

   return true;
}


bool MAX6650_SetSpeed(uint8_t speed_set, uint8_t *speed_actual)
{
    uint8_t ktach;
    uint16_t rpm;
    bool res;

    if(speed_set > 100)
    {
        printf("MAX6650: Warning, speed should be in range: 0..100%%\r\n");
        speed_set = 100;
    }

    rpm = config->rpm_max/100 * speed_set;

    ktach=(((992 * get_scale(config->k_scale)) / (rpm/60) ) - 1);

    res = i2c_ext_if->i2c_write(i2c_address, MAX6650_SPEED_REG, &ktach, 1);

    if(res == true)
    {
        res = MAX6650_GetSpeed(speed_actual);
    }

    return res;
}
