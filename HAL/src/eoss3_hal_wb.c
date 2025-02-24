/**
 * @file    hal_wb.c
 * @author  Martin Anschuetz (martin.anschuetz@vert-tec.io)
 * @brief   
 * @version 0.1
 * @date    2025-01-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "eoss3_dev.h"
#include "eoss3_hal_ffe.h"
#include "eoss3_hal_wb.h"


#define CRU_CLK_CTRL_x_0_DIV_BY(x) ((x) < 2 ? 0x000 : (0x200 | ((x)-2)))


int HAL_WB_Transmit(uint8_t ucAddr, uint8_t ucData, uint8_t ucNodeSel)
{
    while (EXT_REGS_FFE->CSR & (WB_CSR_BUSY | WB_CSR_MASTER_START))
        ;

    EXT_REGS_FFE->ADDR = ucNodeSel | ucAddr;
    EXT_REGS_FFE->WDATA = ucData;

    if (ucNodeSel == WB_ADDR_SPI0_NODE_SEL)
        EXT_REGS_FFE->CSR = WB_CSR_SPI0MUX_SEL_WBMASTER | WB_CSR_MASTER_WR_EN | WB_CSR_MASTER_START | WB_CSR_MUX_SEL_WB;

    else if (ucNodeSel == WB_ADDR_I2C1_NODE_SEL)
        EXT_REGS_FFE->CSR = WB_CSR_I2C1MUX_SEL_WBMASTER | WB_CSR_MASTER_WR_EN | WB_CSR_MASTER_START | WB_CSR_MUX_SEL_WB;

    else if (ucNodeSel == WB_ADDR_I2C0_NODE_SEL)
        EXT_REGS_FFE->CSR = WB_CSR_I2C0MUX_SEL_WBMASTER | WB_CSR_MASTER_WR_EN | WB_CSR_MASTER_START | WB_CSR_MUX_SEL_WB;

    else
        return -1;
    
    return 0;
}

int HAL_WB_Receive(uint8_t ucAddr, uint8_t *pucData, uint8_t ucNodeSel)
{
    while (EXT_REGS_FFE->CSR & (WB_CSR_BUSY | WB_CSR_MASTER_START))
        ;

    EXT_REGS_FFE->ADDR = ucNodeSel | ucAddr;

    if (ucNodeSel == WB_ADDR_SPI0_NODE_SEL)
        EXT_REGS_FFE->CSR = WB_CSR_SPI0MUX_SEL_WBMASTER | WB_CSR_MASTER_START | WB_CSR_MUX_SEL_WB;

    else if (ucNodeSel == WB_ADDR_I2C1_NODE_SEL)
        EXT_REGS_FFE->CSR = WB_CSR_I2C1MUX_SEL_WBMASTER | WB_CSR_MASTER_START | WB_CSR_MUX_SEL_WB;

    else if (ucNodeSel == WB_ADDR_I2C0_NODE_SEL)
        EXT_REGS_FFE->CSR = WB_CSR_I2C0MUX_SEL_WBMASTER | WB_CSR_MASTER_START | WB_CSR_MUX_SEL_WB;
    
    else
        return -1;

    while (EXT_REGS_FFE->CSR & (WB_CSR_BUSY | WB_CSR_MASTER_START))
        ;
    *pucData = EXT_REGS_FFE->RDATA;
    return 0;
}


int HAL_WB_Init(uint8_t ucNodeSel)
{
    // ensure FFE power up
    PMU->FFE_FB_PF_SW_WU |= PMU_FFE_FB_PF_SW_WU_FFE_WU;
    while (!(PMU->FFE_STATUS & 1))
        ;

    if (ucNodeSel == WB_ADDR_SPI0_NODE_SEL)
    {
        
        CRU->CLK_CTRL_B_0 = CRU_CLK_CTRL_x_0_DIV_BY(2); // Good Call!
        
        // Start clock C08X4 = 36 MHz and C08X1 = 9 MHz
        // Assuming main clock is 72MHz
        CRU->CLK_CTRL_C_0 = CRU_CLK_CTRL_x_0_DIV_BY(1);
        CRU->C01_CLK_GATE |= C01_CLK_GATE_PATH_3_ON | C01_CLK_GATE_PATH_9_ON;            // = FFE Gate Path
        CRU->C08_X1_CLK_GATE |= C08_X1_CLK_GATE_PATH_0_ON;
        CRU->C08_X4_CLK_GATE |= C08_X4_CLK_GATE_PATH_0_ON;
        
    }

    return 0;
}


int HAL_WB_DeInit(uint8_t ucNodeSel)
{

    return 0;
}