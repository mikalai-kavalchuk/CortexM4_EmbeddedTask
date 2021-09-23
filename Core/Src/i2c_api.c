#include "i2c_api.h"
#include "error.h"

I2C_HandleTypeDef hi2c2;


/** @defgroup I2C LOW LEVEL Private Function Prototypes
  * @{
  */
static void I2Cx_Init(I2C_HandleTypeDef *i2c_handler);
static void I2Cx_DeInit(I2C_HandleTypeDef *i2c_handler);
static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_IsDeviceReady(I2C_HandleTypeDef *i2c_handler, uint16_t DevAddress, uint32_t Trials);
static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr);
/**
  * @}
  */


/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void I2Cx_Init(I2C_HandleTypeDef *i2c_handler)
{
  i2c_handler->Instance = I2C2;
  i2c_handler->Init.Timing = 0x10909CEC;
  i2c_handler->Init.OwnAddress1 = 0;
  i2c_handler->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  i2c_handler->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  i2c_handler->Init.OwnAddress2 = 0;
  i2c_handler->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  i2c_handler->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c_handler->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(i2c_handler) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(i2c_handler, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(i2c_handler, 0) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  DeInitializes I2C HAL.
  * @param  i2c_handler : I2C handler
  * @retval None
  */
static void I2Cx_DeInit(I2C_HandleTypeDef *i2c_handler)
{
    /* DeInit the I2C */
  HAL_I2C_DeInit(i2c_handler);
}


/**
  * @brief  Reads multiple data.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  MemAddress: memory address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval HAL status
  */
static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* I2C error occured */
    I2Cx_Error(i2c_handler, Addr);
  }
  return status;
}


/**
  * @brief  Writes a value in a register of the device through BUS in using DMA mode.
  * @param  i2c_handler : I2C handler
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @param  MemAddress: memory address
  * @param  Buffer: The target register value to be written
  * @param  Length: buffer size to be written
  * @retval HAL status
  */
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the I2C Bus */
    I2Cx_Error(i2c_handler, Addr);
  }
  return status;
}


/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  i2c_handler : I2C handler
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @retval HAL status
  */
static HAL_StatusTypeDef I2Cx_IsDeviceReady(I2C_HandleTypeDef *i2c_handler, uint16_t DevAddress, uint32_t Trials)
{
  return (HAL_I2C_IsDeviceReady(i2c_handler, DevAddress, Trials, 1000));
}


/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr)
{
  /* De-initialize the I2C communication bus */
  HAL_I2C_DeInit(i2c_handler);

  /* Re-Initialize the I2C communication bus */
  I2Cx_Init(i2c_handler);
}


/*******************************************************************************
                            Exported functions
*******************************************************************************/

bool I2CAPI_Init(bool fast_speed)
{
    I2Cx_Init(&hi2c2);

    if(fast_speed)
    {
        HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C2);
    }
    else
    {
        HAL_I2CEx_DisableFastModePlus(I2C_FASTMODEPLUS_I2C2);
    }

    return true;
}


void I2CAPI_DeInit(void)
{
    I2Cx_DeInit(&hi2c2);
}


void I2CAPI_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
    I2Cx_WriteMultiple(&hi2c2, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT,(uint8_t*)&Value, 1);
}


uint8_t I2CAPI_Read(uint8_t Addr, uint8_t Reg)
{
    uint8_t read_value = 0;
    I2Cx_ReadMultiple(&hi2c2, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&read_value, 1);
    return read_value;
}


bool I2CAPI_ReadMultiple(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length)
{
    HAL_StatusTypeDef res;
    res = I2Cx_ReadMultiple(&hi2c2, addr, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, buffer, length);
    return res == HAL_OK ? true : false;
}


bool I2CAPI_WriteMultiple(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length)
{
    HAL_StatusTypeDef res;
    res = I2Cx_WriteMultiple(&hi2c2, addr, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, buffer, length);
    return res == HAL_OK ? true : false;
}


HAL_StatusTypeDef I2CAPI_IsDeviceReady(uint16_t dev_address, uint32_t trials)
{
    return (I2Cx_IsDeviceReady(&hi2c2, dev_address, trials));
}

