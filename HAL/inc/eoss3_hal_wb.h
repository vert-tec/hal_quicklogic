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
 *    File      : eoss3_hal_wb.h
 *    Purpose :  
 *                                                          
 * ===========================================================
 *
 *
 *     Edit by M. Anschuetz
 *     martin.anschuetz@vert-tec.io
 * 
 */

#ifndef HAL_INC_EOSS3_HAL_WB_H_
#define HAL_INC_EOSS3_HAL_WB_H_

#include <stdint.h>
#include <stddef.h>

#include "test_types.h"
#include "eoss3_hal_def.h"

/*!
 * \brief WB_ADDR register definition
 */
#define WB_ADDR_SPI0_NODE_SEL		((uint8_t)(0x2 << BYTE_IDX_6))
#define WB_ADDR_I2C1_NODE_SEL		((uint8_t)(0x1 << BYTE_IDX_6))
#define WB_ADDR_I2C0_NODE_SEL		((uint8_t)(0x0 << BYTE_IDX_6))

/*!
 *\brief Wishbone Control and status register definition
 */
#define WB_CSR_SPI0MUX_SEL_WBMASTER		((uint8_t)(1 << BYTE_IDX_7))
#define WB_CSR_SPI0MUX_SEL_SM1          ((uint8_t)(0 << BYTE_IDX_7))        

#define WB_CSR_I2C1MUX_SEL_WBMASTER     ((uint8_t)(1 << BYTE_IDX_6))
#define WB_CSR_I2C1MUX_SEL_SM1			((uint8_t)(0 << BYTE_IDX_6))

#define WB_CSR_I2C0MUX_SEL_WBMASTER     ((uint8_t)(1 << BYTE_IDX_5))
#define WB_CSR_I2C0MUX_SEL_SM0			((uint8_t)(0 << BYTE_IDX_5))

#define WB_CSR_OVFL				        ((uint8_t)(1 << BYTE_IDX_4))
#define WB_CSR_BUSY				        ((uint8_t)(1 << BYTE_IDX_3))

#define WB_CSR_MUX_SEL_WB               ((uint8_t)(1 << BYTE_IDX_2))
#define WB_CSR_MUX_SEL_SM               ((uint8_t)(0 << BYTE_IDX_2))

#define WB_CSR_MASTER_WR_EN			    ((uint8_t)(1 << BYTE_IDX_1))
#define WB_CSR_MASTER_START			    ((uint8_t)(1 << BYTE_IDX_0))

/*!
 * \fn		int HAL_WB_Transmit(uint8_t ucOffset, uint8_t ucVal, uint8_t ucNodeSel)
 * \brief 	Function to send data over Wishbone interface
 * \param	ucOffset        --- Wishbone register offset
 * \param       ucVal           --- Data
 * \param       ucNodeSel      --- Node Select (I2C1 or I2C0 or SPI)
 * \return      HAL status
 */
int HAL_WB_Transmit(uint8_t ucOffset, uint8_t ucVal, uint8_t ucNodeSel);
/*!
 * \fn		int HAL_WB_Receive(uint8_t ucOffset, uint8_t *buf, uint8_t ucNodeSel)
 * \brief 	Function to read data over Wishbone interface
 * \param	ucOffset        --- Wishbone register offset
 * \param       ucVal           --- Data
 * \param       ucNodeSel      --- Node Select (I2C1 or I2C0 or SPI)
 * \return      HAL status
 */
int HAL_WB_Receive(uint8_t ucOffset, uint8_t *buf, uint8_t ucNodeSel);
/*!
 * \fn		int HAL_WB_Init(uint8_t ucNodeSel)
 * \brief 	Function to initialize Wishbone interface
 * \param       ucNodeSel      --- Node Select (I2C1 or I2C0 or SPI)
 * \return      HAL status
 */
int HAL_WB_Init(uint8_t ucNodeSel);
/*!
 * \fn		int HAL_WB_DeInit(uint8_t ucNodeSel)
 * \brief 	Function to De-initialize Wishbone interface
 * \param       ucNodeSel      --- Node Select (I2C1 or I2C0 or SPI)
 * \return      HAL status
 */
int HAL_WB_DeInit(uint8_t ucNodeSel);

#endif /* HAL_INC_EOSS3_HAL_WB_H_ */
