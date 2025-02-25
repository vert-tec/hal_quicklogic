/**
 * @file    eoss3_hal_spi.c
 * @author  Martin Anschuetz (martin.anschuetz@vert-tec.io)
 * @brief   
 * @version 0.1
 * @date    2025-02-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include "eoss3_dev.h"
#include "eoss3_hal_wb.h"
#include "eoss3_hal_spi.h"



int HAL_SM_SPI_ReadCfgRegs(uint8_t ucRegOffset, uint8_t *pucData, uint32_t uiLength)
{
    if ((ucRegOffset + uiLength) > 0x08)
        return -1;

    while (uiLength-- > 0)
    {
        if (HAL_WB_Receive(ucRegOffset++, pucData++, WB_ADDR_SPI0_NODE_SEL) != 0)
        {
            return -1;
        }
    }

    return 0;
}


int HAL_SM_SPI_ReadCfgReg(uint8_t ucRegOffset, uint8_t *ucValue)
{
    return HAL_SM_SPI_ReadCfgRegs(ucRegOffset, ucValue, 1);
}


int HAL_SM_SPI_WriteCfgRegs(uint8_t ucRegOffset, uint8_t *pucData, uint32_t uiLength)
{
    if ((ucRegOffset + uiLength) > 0x08)
        return -1;

    while (uiLength-- > 0)
    {
        if (HAL_WB_Transmit(ucRegOffset++, *pucData, WB_ADDR_SPI0_NODE_SEL) != 0)
        {
            return -1;
        }

        pucData++;
    }

    return 0; 
}


int HAL_SM_SPI_WriteCfgReg(uint8_t ucRegOffset, uint8_t ucValue)
{
    return HAL_SM_SPI_WriteCfgRegs(ucRegOffset, &ucValue, 1);
}


int HAL_SM_SPI_PinConfig(SPI_HandleTypeDef *hspi)
{

    uint8_t cs_bits = hspi->Init.ucSSn;

    if (hspi->Init.ucSPIPadSel == 0)
    {
        // CS PINs
        if ((cs_bits & (1 << 0)) > 0) // CS #1
        {
            IO_MUX->PAD_9_CTRL = 0x01 | PAD_E_4MA;
        }

        if ((cs_bits & (1 << 1)) > 0) // CS #2
        {
            IO_MUX->PAD_2_CTRL = 0x01 | PAD_E_4MA;
        }   

        if ((cs_bits & (1 << 2)) > 0) // CS #3
        {
            IO_MUX->PAD_4_CTRL = 0x01 | PAD_E_4MA;
        }

        if ((cs_bits & (1 << 3)) > 0) // CS #4
        {
            IO_MUX->PAD_5_CTRL = 0x01 | PAD_E_4MA;
        }

        if ((cs_bits & (1 << 4)) > 0) // CS #5
        {
            IO_MUX->PAD_7_CTRL = 0x01 | PAD_E_4MA;
        }

        if ((cs_bits & (1 << 5)) > 0) // CS #6
        {
            IO_MUX->PAD_11_CTRL = 0x01 | PAD_E_4MA;
        }

        if ((cs_bits & (1 << 6)) > 0) // CS #7
        {
            IO_MUX->PAD_12_CTRL = 0x01 | PAD_E_4MA;
        }

        if ((cs_bits & (1 << 7)) > 0) // CS #8
        {
            IO_MUX->PAD_13_CTRL = 0x01 | PAD_E_4MA;
        }

        
        //! CLK
        IO_MUX->PAD_10_CTRL = 0x01 | PAD_E_4MA;
        
        //! COPI
        IO_MUX->PAD_6_CTRL = 0x01 | PAD_E_4MA;
        IO_MUX->SPI_SENSOR_MOSI_SEL = 0x01;    // 1 - pad #6

        //! CIPO
        IO_MUX->PAD_8_CTRL = 0x00 | PAD_REN_ENABLE | PAD_OEN_DISABLE;
        IO_MUX->SPI_SENSOR_MISO_SEL = 0x01;    // 1 - pad #8
        
    }
    else if(hspi->Init.ucSPIPadSel == 1)
    {
        // CS PINs
        if ((cs_bits & (1 << 0)) > 0) // CS #1
            IO_MUX->PAD_30_CTRL = 0x01 | PAD_E_4MA;

        if ((cs_bits & (1 << 1)) > 0) // CS #2
            IO_MUX->PAD_36_CTRL = 0x02 | PAD_E_4MA;

        if ((cs_bits & (1 << 2)) > 0) // CS #3
            IO_MUX->PAD_4_CTRL = 0x01 | PAD_E_4MA;

        if ((cs_bits & (1 << 3)) > 0) // CS #4
            IO_MUX->PAD_26_CTRL = 0x01 | PAD_E_4MA;

        if ((cs_bits & (1 << 4)) > 0) // CS #5
            IO_MUX->PAD_27_CTRL = 0x01 | PAD_E_4MA;

        if ((cs_bits & (1 << 5)) > 0) // CS #6
            IO_MUX->PAD_33_CTRL = 0x01 | PAD_E_4MA;

        if ((cs_bits & (1 << 6)) > 0) // CS #7
            IO_MUX->PAD_35_CTRL = 0x02 | PAD_E_4MA;

        if ((cs_bits & (1 << 7)) > 0) // CS #8
            IO_MUX->PAD_37_CTRL = 0x02 | PAD_E_4MA;

        //! CLK
        IO_MUX->PAD_31_CTRL = 0x01 | PAD_E_4MA;
        
        //! COPI
        IO_MUX->PAD_28_CTRL = 0x01 | PAD_E_4MA;
        IO_MUX->SPI_SENSOR_MOSI_SEL = 0x02;    // 2 - pad #28

        //! CIPO
        IO_MUX->PAD_29_CTRL = 0x00 | PAD_REN_ENABLE | PAD_OEN_DISABLE;
        IO_MUX->SPI_SENSOR_MISO_SEL = 0x02;    // 2 - pad #29

    }
    else
    {
        return -1;
    }   

    return 0;
}


int HAL_SM_SPI_StartTransfer(SPI_HandleTypeDef *hspi, uint8_t ucCsMsk)
{
    // TODO: Bi-Directional Mode Init

    // Set Baud Registers (div)
    // f_bus = 8 MHz
    // baud = f_bus / ( 2 * div )
    if (hspi->Init.ucFreq == 0)
    {
        hspi->Init.ucFreq = SPI_CLK_FREQ_4MHZ;
    }
    HAL_SM_SPI_WriteCfgReg(SPI0_BR_LSB_REG, hspi->Init.ucFreq);
    HAL_SM_SPI_WriteCfgReg(SPI0_BR_MSB_REG, 0x00);

    // Enable SPI and Set Config
    uint8_t cfg = 0x00 | SPI_SYSTEM_EN | SPI_INTR_EN;

    if (hspi->Init.ulFirstBit == SPI_FIRSTBIT_MSB)
    {
        cfg |= (SPI_FIRSTBIT_MSB << 0);
    }
    else
    {
        cfg |= (SPI_FIRSTBIT_LSB << 0);
    }

    if (hspi->Init.ulCLKPhase == SPI_PHASE_1EDGE)
    {
        // Sample on Leading (first) Clock Edge
        cfg |= (SPI_PHASE_1EDGE << 2);
    }
    else
    {
        // Sample on trailing (second) Clock Edge
        cfg |= (SPI_PHASE_2EDGE << 2);
    }

    if (hspi->Init.ulCLKPolarity == SPI_POLARITY_LOW)
    {
        // Clock is Idle LOW
        cfg |= (SPI_POLARITY_LOW << 3);
    }
    else
    {
        // Clock is Idle HIGH
        cfg |= (SPI_POLARITY_HIGH << 3);
    }
    HAL_SM_SPI_WriteCfgReg(SPI0_CFG_REG, cfg);


    // Set CS Register
    HAL_SM_SPI_WriteCfgReg(SPI0_SS_REG, ucCsMsk);

    hspi->State = HAL_SPI_STATE_TX_RX_BUSY;

    return 0;
}


int HAL_SM_SPI_EndTransaction(SPI_HandleTypeDef *hspi)
{
    HAL_SM_SPI_WriteCfgReg(SPI0_CMD_STS_REG, SPI_CMD_STOP);
    HAL_SM_SPI_WriteCfgReg(SPI0_CFG_REG, 0x00);
    hspi->State = HAL_SPI_STATE_READY;
    return 0;
}


int HAL_SM_SPI_ReadByte(uint8_t *pData)
{
    uint8_t val = 0;

    HAL_SM_SPI_WriteCfgReg(SPI0_CMD_STS_REG, SPI_CMD_READ | SPI_CMD_START | SPI_CMD_IACK);
    
    // Wait for Transfer Complete
    HAL_SM_SPI_ReadCfgReg(SPI0_CMD_STS_REG, &val);
    while (val & SPI_STAT_TIP)
    {
        HAL_SM_SPI_ReadCfgReg(SPI0_CMD_STS_REG, &val);
    }

    // Check for Write Int. Flag
    if ( !(val & SPI_INTR_IR) ) 
        return -1;

    HAL_SM_SPI_ReadCfgReg(SPI0_TX_RX_REG, pData);

    return 0;
}

int HAL_SM_SPI_WriteByte(uint8_t *pData)
{
    uint8_t val = 0;

    HAL_SM_SPI_WriteCfgReg(SPI0_TX_RX_REG, *pData);
    HAL_SM_SPI_WriteCfgReg(SPI0_CMD_STS_REG, SPI_CMD_WRITE | SPI_CMD_START | SPI_CMD_IACK);

    // Wait for Transfer Complete
    HAL_SM_SPI_ReadCfgReg(SPI0_CMD_STS_REG, &val);
    while (val & SPI_STAT_TIP)
    {
        HAL_SM_SPI_ReadCfgReg(SPI0_CMD_STS_REG, &val);
    }

    // Check for Write Int. Flag
    if ( !(val & SPI_INTR_IW) ) 
        return -1;


    return 0;
}


// int HAL_SM_SPI_Transmit(uint8_t *pData, size_t uiLen)
// {
//     if (ucCsMsk == 0x00) 
//         return -1;


//     HAL_SM_SPI_WriteByte(pData);

//     HAL_SM_SPI_ReadByte(pData);

//     return 0;
// }


int HAL_SM_SPI_Init(SPI_HandleTypeDef *hspi)
{
    // Initialize WB and Enable CLK and PWR
    if (HAL_WB_Init(WB_ADDR_SPI0_NODE_SEL) != 0)
        return -1;

    // Set Pin Config
    if (HAL_SM_SPI_PinConfig(hspi) != 0)
        return -2;

    return 0;
}


int HAL_SPI_StartTransfer(SPI_HandleTypeDef *hspi, uint8_t ucCsMsk)
{
    if (hspi->ucSPIx == SPI0_MASTER_SEL)
    {
        // Sensor SPI0 Master (Belongs to FFE)
        // accesible by Wishbone Bus
        return HAL_SM_SPI_StartTransfer(hspi, ucCsMsk);

    }
    else if (hspi->ucSPIx == SPI1_MASTER_SEL)
    {
        // SPI1 Master
        // TODO: Include the SPI1 Master        
    }

    return 1;
}

int HAL_SPI_EndTransfer(SPI_HandleTypeDef *hspi)
{    if (hspi->ucSPIx == SPI0_MASTER_SEL)
    {
        // Sensor SPI0 Master (Belongs to FFE)
        // accesible by Wishbone Bus
        return HAL_SM_SPI_EndTransaction(hspi);

    }
    else if (hspi->ucSPIx == SPI1_MASTER_SEL)
    {
        // SPI1 Master
        // TODO: Include the SPI1 Master        
    }

    return -1;
}

int HAL_SPI_Write(SPI_HandleTypeDef *hspi, uint8_t *pData, size_t uiLen)
{
    if (hspi->State == HAL_SPI_STATE_ERROR)
        return -1;

    if (hspi->ucSPIx == SPI0_MASTER_SEL)
    {
        // Sensor SPI0 Master (Belongs to FFE)
        // accesible by Wishbone Bus

        for (size_t n = 0; n < uiLen; n++)
        {
            if (HAL_SM_SPI_WriteByte(pData++) != 0)
                return -2;

            hspi->usTxXferCount++;
        }

    }
    else if (hspi->ucSPIx == SPI1_MASTER_SEL)
    {
        // SPI1 Master
        // TODO: Include the SPI1 Master        
    }

    return 0;
}


int HAL_SPI_Read(SPI_HandleTypeDef *hspi, uint8_t *pData, size_t uiLen)
{
    if (hspi->State == HAL_SPI_STATE_ERROR)
        return -1;

    if (hspi->ucSPIx == SPI0_MASTER_SEL)
    {
        // Sensor SPI0 Master (Belongs to FFE)
        // accesible by Wishbone Bus

        for (size_t n = 0; n < uiLen; n++)
        {
            if (HAL_SM_SPI_ReadByte(pData++) != 0)
                return -2;

            hspi->usTxXferCount++;
        }

    }
    else if (hspi->ucSPIx == SPI1_MASTER_SEL)
    {
        // SPI1 Master
        // TODO: Include the SPI1 Master        
    }

    return 0;
}


int HAL_SPI_Init(SPI_HandleTypeDef  *hspi)
{

    if (hspi->ucSPIx == SPI0_MASTER_SEL)
    {
        // Sensor SPI0 Master (Belongs to FFE)
        // accesible by Wishbone Bus

        if (HAL_SM_SPI_Init(hspi) != 0)
            return -1;

    }
    else if (hspi->ucSPIx == SPI1_MASTER_SEL)
    {
        // SPI1 Master
        // TODO: Include the SPI1 Master        
    }

    return 0;
}


