#ifndef __I2C_API_H
#define __I2C_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


/**
  * @brief  Initializes I2C low level.
  * @retval None
  */
void I2cAPI_Init(void);

/**
  * @brief  DeInitializes I2C low level.
  * @retval None
  */
void I2cAPI_DeInit(void);

/**
  * @brief  I2C writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
void I2cAPI_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);

/**
  * @brief  I2C reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t I2cAPI_Read(uint8_t Addr, uint8_t Reg);

/**
  * @brief  I2C reads multiple data with I2C communication
  *         channel from TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval HAL status
  */
uint16_t I2cAPI_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);

/**
  * @brief  I2C writes multiple data with I2C communication
  *         channel from MCU to TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval None
  */
void I2cAPI_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
/**
  * @brief  I2C checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @retval HAL status
  */
HAL_StatusTypeDef I2cAPI_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_API_H */

