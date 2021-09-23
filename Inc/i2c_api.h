#ifndef __I2C_API_H
#define __I2C_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "main.h"



/**
  * @brief  Initializes I2C low level.
  * @param  fast_speed: fast speed mode
  * @retval None
  */
bool I2C_API_Init(bool fast_speed);

/**
  * @brief  DeInitializes I2C low level.
  * @retval None
  */
void I2C_API_DeInit(void);

/**
  * @brief  Set I2C speed.
  * @retval None
  */
void I2C_API_SetSpeed(uint32_t i2c_speed);

/**
  * @brief  I2C writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
void I2C_API_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);

/**
  * @brief  I2C reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t I2C_API_Read(uint8_t Addr, uint8_t Reg);

/**
  * @brief  I2C reads multiple data with I2C communication
  *         channel from TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval HAL status
  */
bool I2C_API_ReadMultiple(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);


/**
  * @brief  I2C writes multiple data with I2C communication
  *         channel from MCU to TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval None
  */

bool I2C_API_WriteMultiple(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);
/**
  * @brief  I2C checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @retval HAL status
  */
HAL_StatusTypeDef I2C_API_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_API_H */

