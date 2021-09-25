#include "Fw_global_config.h"
#include <stdio.h>
#include "eoss3_hal_spi.h"
#include "spi_sram.h"
SPI_HandleTypeDef spiSramHandle; 

HAL_StatusTypeDef spi0_sram_init(void) {
    //SPI master init for SPI flash
    spiSramHandle.Init.ucFreq       = SPI_BAUDRATE_20MHZ; 
    spiSramHandle.Init.ucSPIInf     = SPI_4_WIRE_MODE;
    spiSramHandle.Init.ucSSn        = SPI_SLAVE_3_SELECT;
    spiSramHandle.Init.ulCLKPhase   = SPI_PHASE_1EDGE;
    spiSramHandle.Init.ulCLKPolarity = SPI_POLARITY_LOW;
    spiSramHandle.Init.ulDataSize   = SPI_DATASIZE_8BIT;
    spiSramHandle.Init.ulFirstBit   = SPI_FIRSTBIT_MSB;
    spiSramHandle.Init.ucCmdType    = CMD_NoResponse;
    spiSramHandle.ucSPIx            = SPI0_MASTER_SEL;  // must select SPI0 not SPI1
    
    HAL_StatusTypeDef ret = HAL_ERROR;
    
    ret = HAL_SPI_Init(&spiSramHandle);
    if(ret != HAL_OK) {
        printf("HAL_SPI0_Init failed\r\n");
    }
    return ret;

}

  HAL_StatusTypeDef spi0_sram_trans(uint8_t *cmd, uint32_t cmd_len) {
    
    spiSramHandle.Init.ucCmdType  = CMD_NoResponse; // transmit only.
    HAL_StatusTypeDef ret = HAL_ERROR;

    ret = HAL_SPI_Transmit(&spiSramHandle,cmd,cmd_len,NULL);
    if(ret != HAL_OK) {
        printf("Failed to send command %02x: %d\n",*cmd,ret);
        ret = FlashCmdFailed;
    }
    return ret;
  }