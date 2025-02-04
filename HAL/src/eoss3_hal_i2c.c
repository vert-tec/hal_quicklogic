/* Copyright 2021 Stefan Holst

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stdint.h>

#include "eoss3_hal_wb.h"
#include "eoss3_hal_i2c.h"
// #include "regs/pmu.h"
// #include "regs/cru.h"
// #include "regs/iomux.h"
// #include "regs/ffe.h"


/*
* Note:  12-14-2018
* The Datasheet seems to be not updated to the correct RTL implementation.
* The macro HAL_I2C_CLK_PRESCALE is incorrect due to this.
*      I2C_FREQ ? (SYS_FREQ/(I2C_FREQ*5))-1
*
* The computation should be according to the following formula.
* For pre-scale  = 0:
*
*  Default I2C frequency  (with control register bit [4] = bit[5] = 0)  =  Sys Freq. / 12
* 
* For pre-scale  > 0:
*
*  I2C SCL Freq  = Sys Freq / (5 * (pre-scale +1) + X ,       
*        1. Where X  is added because of the clock stretching support, filtering of the feedback path 
*           to know whether clock is being stretched or not. 
*        2. X varies according to the pre-scale value.
*           For pre-scale (1-3 ), X = 5
*           For pre-scale (4-7 ), X = 7
*           For pre-scale (8-11 ), X = 9 
*           For pre-scale (12-15 ), X =11 and So On
*
* So, the values are precomputed for a lookup in a Table
*/

#define CRU_CLK_CTRL_x_0_DIV_BY(x) ((x) < 2 ? 0x000 : (0x200 | ((x)-2)))

#define CHECK_DIV_ARRAY_SIZE  16
//static int divArray[CHECK_DIV_ARRAY_SIZE]      = {12,15,20,25,32,37,42,47,54,59,64,69,76,81,86,91};
static int checkDivArray[CHECK_DIV_ARRAY_SIZE] = {13,17,22,29,34,39,44,50,56,61,66,72,78,83,88,91};
//int checkDivArraySize = 16;
static int CalculatePreScale(float divFactor)
{
    int i;
    for (i = 0; i < CHECK_DIV_ARRAY_SIZE; i++)
    {
        if (divFactor <= checkDivArray[i])
        {
            return i;
        }
    }
    return CHECK_DIV_ARRAY_SIZE-1;
}


int wb_write(uint8_t ucAddr, uint8_t ucData)
{
    HAL_WB_Transmit(ucAddr, ucData, WB_ADDR_I2C0_NODE_SEL);
    return 0;
}


int wb_read(uint8_t ucAddr, uint8_t *pucData)
{
    HAL_WB_Receive(ucAddr, pucData, WB_ADDR_I2C0_NODE_SEL);
    return 0;
}


int HAL_I2C_WriteByte(I2C_Config *cfg, uint8_t ucData, uint8_t ucFlags)
{
    uint8_t ucStatus = 0;

    if (HAL_WB_Transmit(I2C_TXRX_DR, ucData, cfg->ucI2Cn))
        return -1;
    if (HAL_WB_Transmit(I2C_CMD_SR, I2C_CMD_WRITE_NODE_BIT | ucFlags, cfg->ucI2Cn))
        return -2;
    do
    {
        HAL_WB_Receive(I2C_CMD_SR, &ucStatus, cfg->ucI2Cn);
    } while (ucStatus & I2C_SR_TIP_BIT);
    if (ucStatus & I2C_SR_RXACK_BIT)
        return -3;


    return 0;
}


int HAL_I2C_ReadByte(I2C_Config *cfg, uint8_t *pucData, uint8_t ucFlags)
{
    uint8_t ucStatus = 0;

    if (HAL_WB_Transmit(I2C_CMD_SR, I2C_CMD_READ_NODE_BIT | ucFlags, cfg->ucI2Cn))
    {
        return -1;
    }

    do 
    {
        HAL_WB_Receive(I2C_CMD_SR, &ucStatus, cfg->ucI2Cn);
    } while (ucStatus & I2C_SR_TIP_BIT);
    HAL_WB_Receive(I2C_TXRX_DR, pucData, cfg->ucI2Cn);

    return 0;
}


int HAL_I2C_WriteRegs(I2C_Config *cfg, uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucData, uint32_t uiLength)
{
    if (!ucDevAddr || !pucData || !uiLength)
    {
        return -1;
    }

    if (HAL_I2C_WriteByte(cfg, ucDevAddr << 1, I2C_CMD_START_BIT))
    {
        return -2;
    }

    if (HAL_I2C_WriteByte(cfg, ucRegAddr, 0))
    {
        return -3;
    }

    while (--uiLength)
    {
        if (HAL_I2C_WriteByte(cfg, *pucData++, 0) != 0)
        {
            return -3;
        }
    }
    if (HAL_I2C_WriteByte(cfg, *pucData++, I2C_CMD_STOP_BIT))
    {
        return -3;
    }


    return 0;
}


int HAL_I2C_WriteReg(I2C_Config *cfg, uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t ucData)
{
    return HAL_I2C_WriteRegs(cfg, ucDevAddr, ucRegAddr, &ucData, 1);
}


int HAL_I2C_ReadRegs(I2C_Config *cfg, uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucData, uint32_t uiLength)
{
    if (!ucDevAddr || !pucData || !uiLength)
    {
        return -1;
    }

    if (HAL_I2C_WriteByte(cfg, ucDevAddr << 1, I2C_CMD_START_BIT))
    {
        return -2;
    }

    if (HAL_I2C_WriteByte(cfg, ucRegAddr, 0))
    {
        return -3;
    }

    if (HAL_I2C_WriteByte(cfg, (ucDevAddr << 1) | 1, I2C_CMD_START_BIT))
    {
        return -4;
    }

    while(--uiLength)
    {
        if (HAL_I2C_ReadByte(cfg, pucData++, 0))
        {
            return -5;
        }
    }

    if (HAL_I2C_ReadByte(cfg, pucData, I2C_CMD_STOP_BIT | I2C_CMD_NACK_BIT))
    {
        return -6;
    }

    return 0;
}

// int HAL_I2C_StartTransfer(I2C_Config *cfg, uint8_t ucDevAddress, uint8_t flags)
// {
//     if (cfg->eI2CState != I2C_READY)
//     {
//         return HAL_BUSY;  // Device is Busy
//     }

//     if (!ucDevAddress)
//     {
//         return HAL_ERROR;
//     }

//     uint8_t ucI2CStatus = 0;
//     cfg->eI2CState = I2C_BUSY;

//     /* Request for write */
//     if(HAL_WB_Transmit(I2C_TXRX_DR, ((ucDevAddress << 1) & (~1)), cfg->ucI2Cn) != HAL_OK)
//     {
//         return HAL_ERROR;
//     }

//     /* Start Write Cycle */
//     if (HAL_WB_Transmit(I2C_CMD_SR, I2C_CMD_START_BIT | flags, cfg->ucI2Cn) != HAL_OK )
//     {
//         return HAL_ERROR;
//     }

//     /* Wait for Transfer Complete */
//     do 
//     {
//         HAL_WB_Receive(I2C_CMD_SR, &ucI2CStatus, cfg->ucI2Cn);
//     } while(ucI2CStatus & I2C_SR_TIP_BIT);

//     /* Read ACK */
//     if((ucI2CStatus & I2C_SR_RXACK_BIT))
//     {
//         return HAL_ERROR;
//     }

//     return HAL_OK;
// }

// void HAL_I2C_EndTransfer(I2C_Config *cfg)
// {
//     cfg->eI2CState = I2C_READY;
// }


// int HAL_I2C_Write_Reg(I2C_Config *cfg, uint8_t ucDevAddress, uint8_t ucAddress, uint8_t *pucData)
// {
//     uint8_t ucI2CStatus = 0;

//     if (HAL_I2C_StartTransfer(cfg, ucDevAddress, I2C_CMD_WRITE_NODE_BIT) != HAL_OK)
//     {
//         HAL_I2C_EndTransfer(cfg);
//         return HAL_ERROR;
//     }

//     /* Write Address */
//     if(HAL_WB_Transmit(I2C_TXRX_DR, ucAddress, cfg->ucI2Cn) != HAL_OK)
//     {
//         HAL_I2C_EndTransfer(cfg);
//         return HAL_ERROR;
//     }

//     /* Stop Write Cycle */
//     if(HAL_WB_Transmit(I2C_CMD_SR, I2C_CMD_WRITE_NODE_BIT, cfg->ucI2Cn) != HAL_OK)
//     {
//         HAL_I2C_EndTransfer(cfg);
//         return HAL_ERROR;
//     }

//     HAL_I2C_EndTransfer(cfg);

//     return 0;
// }

// int i2c_init()
// {
//     // ensure FFE power up
//     PMU->FFE_FB_PF_SW_WU |= PMU_FFE_FB_PF_SW_WU_FFE_WU;
//     while (!(PMU->FFE_STATUS & 1))
//         ;

//     // Start clock C08X4 = 36 MHz and C08X1 = 9 MHz
//     // Assuming main clock is 72MHz
//     CRU->CLK_CTRL_C_0 = CRU_CLK_CTRL_x_0_DIV_BY(2);
//     CRU->C01_CLK_GATE |= CRU_C01_CLK_GATE_PATH_3_FFE;
//     CRU->C08_X1_CLK_GATE |= CRU_C08_X1_CLK_GATE_PATH_0_Msk;
//     CRU->C08_X4_CLK_GATE |= CRU_C08_X4_CLK_GATE_PATH_0_Msk;

//     // Route I2C to Pad0 and Pad1
//     IOMUX->PAD[0] = IOMUX_PAD_CSEL_OTHER | IOMUX_PAD_P_PULLUP | IOMUX_PAD_E_4MA | IOMUX_PAD_REN_ENABLE;
//     IOMUX->PAD[1] = IOMUX_PAD_CSEL_OTHER | IOMUX_PAD_P_PULLUP | IOMUX_PAD_E_4MA | IOMUX_PAD_REN_ENABLE;
//     IOMUX->SDA0_SEL = IOMUX_SCL0_SEL_PAD0;
//     IOMUX->SCL0_SEL = IOMUX_SDA0_SEL_PAD1;

//     // Configure I2C frequency scaling factor s
//     // I2Cfreq = C08X1Hz / (5*(s+1)+(int(s/4)*2+5))
//     // Here: 9 MHz / (5*(15+1)+(int(15/4)*2+5)) = 99kHz
//     wb_write(WB_I2C_PRELO, 15);
//     wb_write(WB_I2C_MCR, WB_I2C_MCR_EN_Msk);

//     return 0;
// }

int i2c_write_byte(uint8_t ucData, uint8_t ucFlags)
{
    uint8_t ucStatus = 0;

    if (wb_write(I2C_TXRX_DR, ucData))
        return -1;
    if (wb_write(I2C_CMD_SR, I2C_CMD_WRITE_NODE_BIT | ucFlags))
        return -2;
    do
    {
        wb_read(I2C_CMD_SR, &ucStatus);
    } while (ucStatus & I2C_SR_TIP_BIT);
    if (ucStatus & I2C_SR_RXACK_BIT)
        return -3;

    return 0;
}

int i2c_read_byte(uint8_t *pucData, uint8_t ucFlags)
{
    uint8_t ucStatus = 0;

    if (wb_write(I2C_CMD_SR, I2C_CMD_READ_NODE_BIT | ucFlags))
        return -1;
    do
    {
        wb_read(I2C_CMD_SR, &ucStatus);
    } while (ucStatus & I2C_SR_TIP_BIT);
    wb_read(I2C_TXRX_DR, pucData);

    return 0;
}

int i2c_write_regs(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucData, uint32_t uiLength)
{
    if (!ucDevAddr || !pucData || !uiLength)
        return -1;
    if (i2c_write_byte(ucDevAddr << 1, I2C_CMD_START_BIT))
        return -2;
    if (i2c_write_byte(ucRegAddr, 0))
        return -3;
    while (--uiLength)
    {
        if (i2c_write_byte(*pucData++, 0))
            return -3;
    }
    if (i2c_write_byte(*pucData++, I2C_CMD_STOP_BIT))
        return -3;

    return 0;
}

int i2c_write_reg(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t ucData)
{
    return i2c_write_regs(ucDevAddr, ucRegAddr, &ucData, 1);
}

int i2c_read_regs(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucData, uint32_t uiLength)
{
    if (!ucDevAddr || !pucData || !uiLength)
        return -1;
    if (i2c_write_byte(ucDevAddr << 1, I2C_CMD_START_BIT))
        return -2;
    if (i2c_write_byte(ucRegAddr, 0))
        return -3;
    if (i2c_write_byte((ucDevAddr << 1) | 1, I2C_CMD_START_BIT))
        return -4;
    while (--uiLength)
    {
        if (i2c_read_byte(pucData++, 0))
            return -5;
    }
    if (i2c_read_byte(pucData, I2C_CMD_STOP_BIT | I2C_CMD_NACK_BIT))
        return -6;

    return 0;
}

static I2C_Config i2c_cfg;

int i2c_accel_init()
{
    
	i2c_cfg.ucI2Cn = WB_I2C0;
	HAL_I2C_Init(&i2c_cfg);

    int ret = 0;
    ret = HAL_I2C_WriteReg(&i2c_cfg, 0x18, 0x23, 0x08);
    printf("1: %d\n", ret);
    ret = HAL_I2C_WriteReg(&i2c_cfg, 0x18, 0x20, 0x57);
    printf("1: %d\n", ret);
    ret = HAL_I2C_WriteReg(&i2c_cfg, 0x18, 0x1f, 0xC0);
    printf("1: %d\n", ret);

    uint8_t data[4];
    HAL_I2C_ReadRegs(&i2c_cfg, 0x18, 0x23, data, 3);


    printf("%02x %02x %02x\n", data[0], data[1], data[2]);

    //i2c_init();
    //i2c_write_reg(0x18, 0x23, 0x80);
    //i2c_write_reg(0x18, 0x20, 0x57);
    //i2c_write_reg(0x18, 0x1f, 0xC0);

    return 0;

}

int i2c_accel_read(int32_t *piX, int32_t *piY, int32_t *piZ)
{
    uint8_t b[6];
    HAL_I2C_ReadRegs(&i2c_cfg, 0x18, 0x80 | 0x28, b, 6);
    *piX = ((b[0] >> 6) & 0x3) | ((int32_t)((int8_t)b[1])) << 2;
    *piY = ((b[2] >> 6) & 0x3) | ((int32_t)((int8_t)b[3])) << 2;
    *piZ = ((b[4] >> 6) & 0x3) | ((int32_t)((int8_t)b[5])) << 2;

    return 0;
}


int HAL_I2C0_Select(void)
{
    return 0;
}

int HAL_I2C1_Select(void)
{
    return 0;
}



int HAL_I2C0_Default_Pin_Init(I2C_Config xI2CConfig)
{


    return 0;
}


int HAL_I2C1_Default_Pin_Init(I2C_Config xI2CConfig)
{


    return 0;
}


int HAL_I2C_SetClockFreq(I2C_Config *xI2CConfig)
{
    uint32_t uiClock, uiPrescale;
    uint8_t val;
    uint32_t uiClkFreq = xI2CConfig->eI2CFreq;

    //Set the frequency
    /** TODO: Implement the Clock HAL  */
    //uiClock = S3x_Clk_Get_Rate(S3X_FFE_X1_CLK);
    uiClock = 71000u;

    // Configure I2C frequency scaling factor s
    // I2Cfreq = C08X1Hz / (5*(s+1)+(int(s/4)*2+5))
    // Here: 9 MHz / (5*(15+1)+(int(15/4)*2+5)) = 99kHz
    uiPrescale = 15;
    val = (uint8_t) uiPrescale;

    // Program prescale value
// #if 1 //use new formula to compute    
//     uiPrescale = CalculatePreScale(uiClock*1.0/(uiClkFreq*100*1000));
// #else  //this is incorrect according to RTL   
//     uiPrescale = HAL_I2C_CLK_PRESCALE(uiClock, (uiClkFreq*100*1000));
// #endif    
//    HAL_I2C_WRITE_PRESCALE(uiPrescale);

    // HAL_WB_Transmit(I2C_PRELO, uiPrescale&0xFF, xI2CConfig.ucI2Cn);
    // HAL_WB_Transmit(I2C_PREHI, (uiPrescale>>8)&0xFF, xI2CConfig.ucI2Cn);

    HAL_WB_Transmit(I2C_PRELO, val,  xI2CConfig->ucI2Cn);

    uint8_t reg = 0x00;

    HAL_WB_Receive(I2C_PRELO, &reg, xI2CConfig->ucI2Cn);

    printf("w %02x / r %02x\n", val, reg);

    return 0;
}


int HAL_I2C_Init(I2C_Config *xI2CConfig)
{
    if (! ((xI2CConfig->ucI2Cn == WB_I2C0) || (xI2CConfig->ucI2Cn == WB_I2C1)))
    {
        return -1;
    }

    HAL_WB_Init(xI2CConfig->ucI2Cn);

    // Start clock C08X4 = 36 MHz and C08X1 = 9 MHz
    // Assuming main clock is 72MHz
    CRU->CLK_CTRL_C_0 = CRU_CLK_CTRL_x_0_DIV_BY(2);
    CRU->C01_CLK_GATE |= C01_CLK_GATE_PATH_3_ON;            // = FFE Gate Path
    CRU->C08_X1_CLK_GATE |= C08_X1_CLK_GATE_PATH_0_ON;
    CRU->C08_X4_CLK_GATE |= C08_X4_CLK_GATE_PATH_0_ON;

    // Route I2C to Pad0 and Pad1
    IO_MUX->PAD_0_CTRL = PAD_CTRL_SEL_OTHER | PAD_P_PULLUP | PAD_E_4MA | PAD_REN_ENABLE;
    IO_MUX->PAD_1_CTRL = PAD_CTRL_SEL_OTHER | PAD_P_PULLUP | PAD_E_4MA | PAD_REN_ENABLE;
    IO_MUX->SDA0_SEL_REG = SDA0_SEL_PAD1;
    IO_MUX->SCL0_SEL_REG = SCL0_SEL_PAD0;

    HAL_WB_Transmit(I2C_MCR, (uint8_t) ~I2C_CR_EN_BIT, xI2CConfig->ucI2Cn);

    /* Set I2C clock frequency */
    HAL_I2C_SetClockFreq(xI2CConfig);

    /* Check if Interrupt needs to be enable */
    if(xI2CConfig->eI2CInt)
    {
        HAL_WB_Transmit(I2C_MCR, I2C_CR_IEN_BIT, xI2CConfig->ucI2Cn);
    }

    HAL_WB_Transmit(I2C_MCR, I2C_CR_EN_BIT, xI2CConfig->ucI2Cn);

	xI2CConfig->eI2CState = I2C_READY;

    return 0;
}
