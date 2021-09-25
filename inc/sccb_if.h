#ifndef _SCCB_IF_H_
#define _SCCB_IF_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "eoss3_hal_i2c.h"

#if 1
static I2C_Config i2c0config =
{
  .eI2CFreq = I2C_400KHZ,    // 400kHz
  .eI2CInt = I2C_DISABLE,    // enabled interrupt
  .ucI2Cn = 0
};
#endif

void sccb_init(void) ;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _SCCB_IF_H_ */