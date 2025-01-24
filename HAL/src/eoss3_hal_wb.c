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


int HAL_WB_Transmit(uint8_t ucAddr, uint8_t ucData, uint8_t ucNodeSel)
{
    while (EXT_REGS_FFE->CSR & (WB_CSR_BUSY | WB_CSR_MASTER_START))
        ;

    EXT_REGS_FFE->ADDR = ucAddr;
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

    EXT_REGS_FFE->ADDR = ucAddr;

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

    return 0;
}


int HAL_WB_DeInit(uint8_t ucNodeSel)
{

    return 0;
}