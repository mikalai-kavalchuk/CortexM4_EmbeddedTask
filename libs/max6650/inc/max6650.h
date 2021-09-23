#ifndef __MAX6650_INC_MAX6650_H
#define __MAX6650_INC_MAX6650_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


/**
 * @brief I2C External Interface
 */
struct MAX6650_I2C_ExtInterface
{
    bool (*i2c_setup)(bool fast_speed);
    bool (*i2c_read)(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);
    bool (*i2c_write)(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);
};


/**
 * @brief MAX6650 ADD line connection
 */
typedef enum
{
    ADD_Line_GND,
    ADD_Line_Vcc,
    ADD_Line_NotConnected,
    ADD_Line_Res10K

} MAX6650_ADDLineConn_t;

/**
 * @brief MAX6650 Operating Mode
 */
typedef enum
{
    OperatingMode_Software_FullOn = 0,
    OperatingMode_Software_Off,
    OperatingMode_Closed_Loop,
    OperatingMode_Open_loop
} MAX6650_OperatingMode_t;

/**
 * @brief MAX6650 Fan Voltage
 */
typedef enum
{
    FanVoltage_5V = 0,
    FanVoltage_12V,

} MAX6650_FanVoltage_t;

/**
 * @brief MAX6650 Kscale
 */
typedef enum
{
    KScale_1 = 0,
    KScale_2,
    KScale_4,
    KScale_8,
    KScale_16
} MAX6650_KScale_t;

/**
 * @brief MAX6650 Configuration structure
 */
typedef struct
{
    MAX6650_ADDLineConn_t add_line_connection;
    MAX6650_OperatingMode_t operating_mode;
    MAX6650_FanVoltage_t fan_lovtage;
    MAX6650_KScale_t k_scale;
    uint16_t rpm_max;
} MAX6650_Config_t;

/**
  * @brief MAX6650 Initialization Function
  * @param[in] MAX6650 configuration
  * @param[in] External I2C Interface
  * @retval true if initialized, otherwise false
  */
bool MAX6650_Init(MAX6650_Config_t *max6650_config, const struct MAX6650_I2C_ExtInterface *ext_i2c_interface);

/**
 * @brief MAX6650 Set Speed
 * @param[in] speed_set (0..100%)
 * @param[out] speed_actual (0..100%)
 * @retval true if speed has been set
 */
bool MAX6650_SetSpeed(uint8_t speed_set, uint8_t *speed_actual);

/**
 * @brief MAX6650 Get Speed
 * @param[out] speed (0..100)
 * @retval true if speed has been read
 */
bool MAX6650_GetSpeed(uint8_t *speed);


#ifdef __cplusplus
}
#endif

#endif /* __MAX6650_INC_MAX6650_H */
