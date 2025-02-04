/*
 * ==========================================================
 *
 *    Copyright (C) 2020 QuickLogic Corporation             
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 		http://www.apache.org/licenses/LICENSE-2.0
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *
 *    File      : eoss3_hal_i2c.h
 *    Purpose 	: This file contains API declaration for I2C read/write
 *                                                          
 * ===========================================================
 *
 *
 *     Edit by M. Anschuetz
 *     martin.anschuetz@vert-tec.io
 * 
 */


#ifndef __EOSS3_HAL_I2C_H_
#define __EOSS3_HAL_I2C_H_
#include <stdint.h>
#include <stddef.h>
#include "test_types.h"
#include "eoss3_dev.h"
#include "eoss3_hal_def.h"
#include "eoss3_hal_wb.h"

   
/* I2C internal states */
typedef enum
{
    I2C_RESET=0,
    I2C_READY,
    I2C_BUSY,
    I2C_TIMEOUT
}I2C_State;

/* I2C clock frequency */
typedef enum
{
    I2C_100KHZ = 1,
    I2C_200KHZ,
    I2C_300Khz,
    I2C_400KHZ,
    I2C_INVALID
}I2C_FREQ;
   
/* interrupt mode */
typedef enum
{
    I2C_DISABLE = 0,
    I2C_ENABLE
} I2C_IntMode;

#define  I2C_PRELO                                  0   // Pre-Scaler Reg Low Byte
#define  I2C_PREHI                                  1   // Pre-Scaler Reg High Byte
#define  I2C_MCR                                    2   // Control Register
#define  I2C_TXRX_DR                                3   // Rx/Tx Data Register
#define  I2C_CMD_SR                                 4   // Command and Status Register
  
  
/* Clock prescale macros */
//Note: 12-14-2018. This is incorrect. Use the Table lookup computation 
//#define HAL_I2C_CLK_PRESCALE(SYS_FREQ, I2C_FREQ)    (I2C_FREQ?(SYS_FREQ/(I2C_FREQ*5))-1:0)
#define HAL_I2C_WRITE_PRESCALE(PRESCALE)            (I2C->I2C_PRELO = PRESCALE&0xFF); (I2C->I2C_PREHI = (PRESCALE >> 8)&0xFF)

/* Control register macros */

#define I2C_CR_EN_BIT_Pos                           7
#define I2C_CR_IEN_BIT_Pos                          6

#define I2C_CR_EN_BIT                               ( 1 << I2C_CR_EN_BIT_Pos )
#define I2C_CR_IEN_BIT                              ( 1 << I2C_CR_IEN_BIT_Pos )


#define HAL_I2C_ENABLE()                            (I2C->I2C_MCR |= 1<<I2C_CR_EN_BIT_Pos)
#define HAL_I2C_DISABLE()                           (I2C->I2C_MCR &= ~(1<<I2C_CR_EN_BIT_Pos))

#define HAL_I2C_INT_ENABLE()                        (I2C->I2C_MCR |= 1<<I2C_CR_IEN_BIT_Pos)
#define HAL_I2C_INT_DISABLE()                       (I2C->I2C_MCR &= ~(1<<I2C_CR_IEN_BIT_Pos))

/* Data register macros */
#define HAL_I2C_WRITE_REQ(DEV_ADR)                  (I2C->I2C_TXRX_DR = (uint8_t)((DEV_ADR<<1) & (~1)))
#define HAL_I2C_READ_REQ(DEV_ADR)                   (I2C->I2C_TXRX_DR = (uint8_t)((DEV_ADR<<1) | 1))  

/* Command register macros */

#define I2C_CMD_START_BIT                           0x80
#define I2C_CMD_STOP_BIT                            0x40
#define I2C_CMD_READ_NODE_BIT                       0x20
#define I2C_CMD_WRITE_NODE_BIT                      0x10
#define I2C_CMD_NACK_BIT                            0x08
#define I2C_CMD_IACK_BIT                            0x01
  
#define HAL_I2C_SET_CMD(CMD_VAL)                    (I2C->I2C_CMD_SR = (uint8_t)CMD_VAL)

/* Status register macros */

#define SR_RXACK_BIT_Pos                            7
#define SR_BUSY_BIT_Pos                             6
#define SR_AL_BIT_Pos                               5
#define SR_TIP_BIT_Pos                              1
#define SR_IF_BIT_Pos                               0

#define I2C_SR_RXACK_BIT                            (1<<SR_RXACK_BIT_Pos)
#define I2C_SR_BUSY_BIT                             (1<<SR_BUSY_BIT_Pos)
#define I2C_SR_AL_BIT                               (1<<SR_AL_BIT_Pos)
#define I2C_SR_TIP_BIT                              (1<<SR_TIP_BIT_Pos)
#define I2C_SR_IF_BIT                               (1<<SR_IF_BIT_Pos)

  
// #define HAL_I2C_IS_RXACK_SET()                      (I2C->I2C_CMD_SR & (1<<SR_RXACK_BIT))
// #define HAL_I2C_IS_BUSY_SET()                       (I2C->I2C_CMD_SR & (1<<SR_BUSY_BIT))
// #define HAL_I2C_IS_AL_SET()                         (I2C->I2C_CMD_SR & (1<<SR_AL_BIT))
// #define HAL_I2C_IS_TIP_SET()                        (I2C->I2C_CMD_SR & (1<<SR_TIP_BIT))
// #define HAL_I2C_IS_IF_SET()                         (I2C->I2C_CMD_SR & (1<<SR_IF_BIT))

typedef enum
{
    WB_I2C0 = WB_ADDR_I2C0_NODE_SEL,
    WB_I2C1 = WB_ADDR_I2C1_NODE_SEL
} I2C_Inst;


/*! \struct FIFO_IntConfig eoss3_hal_i2c.h "inc/eoss3_hal_i2c.h"
 * 	\brief I2C clock and interrupt configuration structure.
 */
typedef struct
{
	I2C_FREQ     eI2CFreq;                       /*!< I2C Frequency */
	I2C_IntMode  eI2CInt;                        /*!< Interrupt enable */
	I2C_Inst	 ucI2Cn;                         /*!< I2C Instance */
    I2C_State    eI2CState;
}I2C_Config;

// /*! \fn int HAL_I2C0_Select(void)
//  *  \brief Select I2C0 device to use for all I2C init/read/write operation.
//  *
//  *  \return int      status of device.
//  */
// int HAL_I2C0_Select(void);

// /*! \fn int HAL_I2C1_Select(void)
//  *  \brief Select I2C1 device to use for all I2C init/read/write operation.
//  *
//  *  \return int      status of device.
//  */
// int HAL_I2C1_Select(void);

/*! \fn int HAL_I2C_Init(I2C_Config xI2CConfig)
 *  \brief Select I2C1 device to use for all I2C init/read/write operation.
 *
 *  \param xI2CConfig           I2C configuration structure
 *  \return int   status of device Init operation.
 */
int HAL_I2C_Init(I2C_Config *xI2CConfig);

/*! \fn int HAL_I2C_SetClockFreq(uint32_t uiClkFreq)
 *  \brief Set clock frequency for I2C device. It may not be exact frequency when set.
 *
 *  \param xI2CConfig           I2C configuration structure
 *  \return int   status of frequency set operation.
 */
int HAL_I2C_SetClockFreq(I2C_Config *xI2CConfig);


int HAL_I2C_StartTransfer(I2C_Config *cfg, uint8_t ucDevAddress, uint8_t flags);
void HAL_I2C_EndTransfer(I2C_Config *cfg);


int HAL_I2C_Write_Reg(I2C_Config *cfg, uint8_t ucDevAddress, uint8_t ucAddress, uint8_t *pucData);

/*! \fn int HAL_I2C_Write(uint8_t ucDevAddress, uint8_t ucAddress, uint8_t *pucDataBuf, uint32_t uiLength)
 *  \brief Write data to I2C device.
 *
 *  \param ucDevAddress         I2C device address.
 *  \param ucAddress            offset address in device to write.
 *  \param pucDataBuf           pointer to data array to write.
 *  \param uiLength             Length of data to write (in bytes)
 *  \return int   status of I2C write operation.
 */
int HAL_I2C_Write(I2C_Config *cfg, uint8_t ucDevAddress, uint8_t ucAddress, uint8_t *pucDataBuf, uint32_t uiLength);

/*! \fn int HAL_I2C_Read(uint8_t ucDevAddress, uint8_t ucAddress, uint8_t *pucDataBuf, uint32_t uiLength)
 *  \brief Read data from I2C device.
 *
 *  \param ucDevAddress         I2C device address.
 *  \param ucAddress            offset address in device to read.
 *  \param pucDataBuf           pointer to data array to read.
 *  \param uiLength             Length of data to read (in bytes)
 *  \return int   status of I2C read operation.
 */
int HAL_I2C_Read(I2C_Config *cfg, uint8_t ucDevAddress, uint8_t ucAddress, uint8_t *pucDataBuf, uint32_t uiLength);

int HAL_I2C_Read16(I2C_Config *cfg, uint8_t ucDevAddress, uint16_t ucAddress, uint8_t *pucDataBuf, uint32_t uiLength);
int HAL_I2C_Write16(I2C_Config *cfg, uint8_t ucDevAddress, uint16_t ucAddress, uint8_t *pucDataBuf, uint32_t uiLength);
#endif /* !__EOSS3_HAL_I2C_H_ */
