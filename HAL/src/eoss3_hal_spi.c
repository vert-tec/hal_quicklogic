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


 #include "eoss3_hal_spi.h"



int HAL_SM_SPI_PinConig(SPI_HandleTypeDef *hspi)
{

    uint8_t cs_bits = hspi->Init.ucSSn;

    if (hspi->Init.ucSPIPadSel == 0)
    {
        // CS PINs
        if (cs_bits & (1 << 0)) // CS #1
            IO_MUX->PAD_9_CTRL = 0x01 | PAD_E_4MA;

        if (cs_bits & (1 << 1)) // CS #2
            IO_MUX->PAD_2_CTRL = 0x01 | PAD_E_4MA;

        if (cs_bits & (1 << 2)) // CS #3
            IO_MUX->PAD_4_CTRL = 0x01 | PAD_E_4MA;

        if (cs_bits & (1 << 3)) // CS #4
            IO_MUX->PAD_5_CTRL = 0x01 | PAD_E_4MA;

        if (cs_bits & (1 << 4)) // CS #5
            IO_MUX->PAD_7_CTRL = 0x01 | PAD_E_4MA;

        if (cs_bits & (1 << 5)) // CS #6
            IO_MUX->PAD_11_CTRL = 0x01 | PAD_E_4MA;

        if (cs_bits & (1 << 6)) // CS #7
            IO_MUX->PAD_12_CTRL = 0x01 | PAD_E_4MA;

        if (cs_bits & (1 << 7)) // CS #8
            IO_MUX->PAD_13_CTRL = 0x01 | PAD_E_4MA;

        
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
        // CS PIN
        IO_MUX->PAD_7_CTRL = 0x01 | PAD_E_4MA;

        //! CLK
        IO_MUX->PAD_10_CTRL = 0x01 | PAD_E_4MA;
        
        //! COPI
        IO_MUX->PAD_6_CTRL = 0x01 | PAD_E_4MA;
        IO_MUX->SPI_SENSOR_MOSI_SEL = 0x01;    // 1 - pad #6

        //! CIPO
        IO_MUX->PAD_8_CTRL = 0x00 | PAD_REN_ENABLE | PAD_OEN_DI

    }
    else
    {
        return -1;
    }   

    return 0;
}



int HAL_SM_SPI_Init(SPI_HandleTypeDef *hspi)
{




    return 0;
}





int HAL_SPI_Init(SPI_HandleTypeDef  *hspi)
{


    if (hspi->ucSPIx == SPI0_MASTER_SEL)
    {
        // Sensor SPI0 Master (Belongs to FFE)
        // accesible by Wishbone Bus

        HAL_SM_SPI_PinConfig(hspi);
        HAL_SM_SPI_Init(hspi);

    }
    else if (hspi->ucSPIx == SPI1_MASTER_SEL)
    {
        // SPI1 Master
        // TODO: Include the SPI1 Master        
    }


}


