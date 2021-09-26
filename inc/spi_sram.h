#ifndef _SPI_SRAM_H_
#define _SPI_SRAM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "eoss3_dev.h"
#include "eoss3_hal_pad_config.h"
#include "eoss3_hal_wb.h"


HAL_StatusTypeDef spi0_sram_init(void);
HAL_StatusTypeDef spi0_sram_trans(uint8_t *cmd, uint32_t cmd_len) ;

// See TRM p.245~p.252 "SPI_0_Master registers"

// SPI_0_Master Offsets
#define SPI0_BAUD_LSB   (0x00)
#define SPI0_BAUD_MSB   (0x01)
#define SPI0_CONFIG     (0x02)
#define SPI0_TRAN_RECV  (0x03)
#define SPI0_CSR        (0x04)
#define SPI0_SLAVE_SEL  (0x05)
#define SPI0_CLK_CTRL   (0x06)
#define SPI0_CLK_DEACT  (0x07)




#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* _SPI_SRAM_H_ */