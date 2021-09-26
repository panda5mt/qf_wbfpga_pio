#include "Fw_global_config.h"
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <test_types.h>
#include "s3x_clock_hal.h"
#include "spi_sram.h"


uint8_t ucSPI0SS = WB_ADDR_SPI0_SLAVE_SEL;

HAL_StatusTypeDef spi0_sram_init(void) {

    //SPI0 master init 
    HAL_StatusTypeDef ret = HAL_OK;
    //HAL_WB_Init(WB_ADDR_SPI0_SLAVE_SEL);
    //PadConfig  padcfg;

    //enable FFE power & clock domain
    PMU->FFE_PWR_MODE_CFG = 0x0;
    PMU->FFE_PD_SRC_MASK_N = 0x0;
    PMU->FFE_WU_SRC_MASK_N = 0x0;

    //wake up FFE
    PMU->FFE_FB_PF_SW_WU = 0x1;
    //check if FFE is in Active mode
    while(!(PMU->FFE_STATUS & 0x1));

    S3x_Clk_Set_Rate(S3X_FFE_X1_CLK, F_10MHZ); 
    S3x_Clk_Set_Rate(S3X_FFE_CLK, F_10MHZ); 
    S3x_Clk_Enable(S3X_FFE_X1_CLK);
    S3x_Clk_Enable(S3X_FFE_CLK);
    
    return ret;

}

  HAL_StatusTypeDef spi0_sram_trans(uint8_t data) {
    
    HAL_StatusTypeDef ret = HAL_OK;
    uint32_t spi_status = 0;
    // 1: set divisor(offset0x00 and 0x01)
    HAL_WB_Transmit(SPI0_BAUD_LSB,  0x01, ucSPI0SS);
    HAL_WB_Transmit(SPI0_BAUD_MSB,  0x00, ucSPI0SS);
    // 2: write SPI config(offset 0x02)
    HAL_WB_Transmit(SPI0_CONFIG,    0x80, ucSPI0SS);
    // 3: select SPI slave(offset 0x05)
    HAL_WB_Transmit(SPI0_SLAVE_SEL, 0x01, ucSPI0SS); // 0x01 = CSn1
    // 4: Write Transmit register (offset 0x03)
    HAL_WB_Transmit(SPI0_TX_RX,     data, ucSPI0SS);
    // 5: Start transfer
    HAL_WB_Transmit(SPI0_CSR, 0x05, ucSPI0SS);
    // 6: check if transmit complete
    while(1) {
      HAL_WB_Receive(SPI0_CSR, &spi_status, ucSPI0SS);
      if(0 != (0x02 & spi_status))break;
    }
    // 7: stop transaction(CS-> High)
    ret = HAL_WB_Transmit(SPI0_CSR,  0x02, ucSPI0SS);
    return ret;
  }