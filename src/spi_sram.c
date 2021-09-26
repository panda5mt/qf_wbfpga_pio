#include "Fw_global_config.h"
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <test_types.h>
#include "s3x_clock_hal.h"
#include "spi_sram.h"


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

  
    S3x_Clk_Enable(S3X_FFE_X1_CLK);
    S3x_Clk_Enable(S3X_FFE_CLK);
    
    return ret;

}

  HAL_StatusTypeDef spi0_sram_trans(uint8_t *cmd, uint32_t cmd_len) {
    
    HAL_StatusTypeDef ret = HAL_OK;

    return ret;
  }