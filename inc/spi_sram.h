#ifndef _SPI_SRAM_H_
#define _SPI_SRAM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "eoss3_hal_spi.h"

HAL_StatusTypeDef spi0_sram_init(void);
HAL_StatusTypeDef spi0_sram_trans(uint8_t *cmd, uint32_t cmd_len) ;



#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* _SPI_SRAM_H_ */