#include "Fw_global_config.h"
#include <stdio.h>
#include "eoss3_hal_i2c.h"
#include "sccb_if.h"

// camera init
void sccb_init(void) {

  uint8_t i2c_read_data = 0x0;
  uint8_t i2c_write_data = 0x0;
  uint8_t val = 0;
  HAL_I2C_Init(i2c0config);

  // SCCB: write:0x78/read:0x79
  // PLL
  uint8_t sccb_dat[] = {0x31, 0x03, 0x93};
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // Reset system
  sccb_dat[0]=0x30;sccb_dat[1]=0x08,sccb_dat[2]=0x82;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  
  // output enable(1)
  sccb_dat[0]=0x30;sccb_dat[1]=0x17,sccb_dat[2]=0x7f;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // output enable(2)
  sccb_dat[0]=0x30;sccb_dat[1]=0x18,sccb_dat[2]=0xfc;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // HV offset setting
  sccb_dat[0]=0x38; sccb_dat[1]=0x10 ; sccb_dat[2]=0xc2; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // analog setting
  sccb_dat[0]=0x36; sccb_dat[1]=0x15 ; sccb_dat[2]=0xf0; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // block init
  sccb_dat[0]=0x30; sccb_dat[1]=0x00 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x30; sccb_dat[1]=0x01 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x02 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x03 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  sccb_dat[0]=0x30; sccb_dat[1]=0x00 ; sccb_dat[2]=0xf8 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x01 ; sccb_dat[2]=0x48 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x02 ; sccb_dat[2]=0x5c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x03 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // block clock enable
  sccb_dat[0]=0x30; sccb_dat[1]=0x04 ; sccb_dat[2]=0x07 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x05 ; sccb_dat[2]=0xb7 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x06 ; sccb_dat[2]=0x43 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x07 ; sccb_dat[2]=0x37 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  //PLL
  sccb_dat[0]=0x30; sccb_dat[1]=0x11 ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x30; sccb_dat[1]=0x10 ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  // VFIFO
  sccb_dat[0]=0x46; sccb_dat[1]=0x0c ; sccb_dat[2]=0x22 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
 
 
  // unknown settings
  sccb_dat[0]=0x38; sccb_dat[1]=0x15 ; sccb_dat[2]=0x04 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // array control
  sccb_dat[0]=0x37; sccb_dat[1]=0x0d ; sccb_dat[2]=0x06 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  
  // analog settings
  sccb_dat[0]=0x37; sccb_dat[1]=0x0c ; sccb_dat[2]=0xa0 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x02 ; sccb_dat[2]=0xfc ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x12 ; sccb_dat[2]=0xff ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x34 ; sccb_dat[2]=0xc0 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x13 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x05 ; sccb_dat[2]=0x7c ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // array control 
  sccb_dat[0]=0x36; sccb_dat[1]=0x21 ; sccb_dat[2]=0x09 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // analog settings
  sccb_dat[0]=0x36; sccb_dat[1]=0x22 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x04 ; sccb_dat[2]=0x40 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x03 ; sccb_dat[2]=0xa7 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x03 ; sccb_dat[2]=0x27 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // black color level 
  sccb_dat[0]=0x40; sccb_dat[1]=0x00 ; sccb_dat[2]=0x21 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x40; sccb_dat[1]=0x1d ; sccb_dat[2]=0x02 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // analog settings
  sccb_dat[0]=0x36; sccb_dat[1]=0x00 ; sccb_dat[2]=0x54 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x05 ; sccb_dat[2]=0x04 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x06 ; sccb_dat[2]=0x3f ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // flicker
  sccb_dat[0]=0x3c; sccb_dat[1]=0x01 ; sccb_dat[2]=0x80 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // ISP
  sccb_dat[0]=0x50; sccb_dat[1]=0x00 ; sccb_dat[2]=0x4f ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // unknown 
  sccb_dat[0]=0x50; sccb_dat[1]=0x20 ; sccb_dat[2]=0x04 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // AWB
  sccb_dat[0]=0x51; sccb_dat[1]=0x81 ; sccb_dat[2]=0x79 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x82 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x85 ; sccb_dat[2]=0x22 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x97 ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // ISP
  sccb_dat[0]=0x50; sccb_dat[1]=0x01 ; sccb_dat[2]=0xff ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // UV adjust
  sccb_dat[0]=0x55; sccb_dat[1]=0x00 ; sccb_dat[2]=0x0a ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x55; sccb_dat[1]=0x04 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x55; sccb_dat[1]=0x05 ; sccb_dat[2]=0x7f ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // ISP
  sccb_dat[0]=0x50; sccb_dat[1]=0x80 ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // MIPI
  sccb_dat[0]=0x30; sccb_dat[1]=0x0e ; sccb_dat[2]=0x18 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // unknown 
  sccb_dat[0]=0x46; sccb_dat[1]=0x10 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // DVP output
  sccb_dat[0]=0x47; sccb_dat[1]=0x1d ; sccb_dat[2]=0x05 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x47; sccb_dat[1]=0x08 ; sccb_dat[2]=0x06 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // analog setting register 
  sccb_dat[0]=0x37; sccb_dat[1]=0x10 ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x32 ; sccb_dat[2]=0x41 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x37; sccb_dat[1]=0x02 ; sccb_dat[2]=0x40 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x20 ; sccb_dat[2]=0x37 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x36; sccb_dat[1]=0x31 ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // output setting
  sccb_dat[0]=0x38; sccb_dat[1]=0x08 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);   
  sccb_dat[0]=0x38; sccb_dat[1]=0x09 ; sccb_dat[2]=0x80 ; // H-size:0x0280 = 640
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x38; sccb_dat[1]=0x0a ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);   
  sccb_dat[0]=0x38; sccb_dat[1]=0x0b ; sccb_dat[2]=0xe0 ; // V-size:0x01e0 = 480
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x38; sccb_dat[1]=0x0e ; sccb_dat[2]=0x07 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x38; sccb_dat[1]=0x0f ; sccb_dat[2]=0xd0 ; // V-pixel:0x07d0 = 2000
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // select output format
  sccb_dat[0]=0x50; sccb_dat[1]=0x1f ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // ISP Settings
  sccb_dat[0]=0x50; sccb_dat[1]=0x00 ; sccb_dat[2]=0x4f ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // output format settings
  sccb_dat[0]=0x51; sccb_dat[1]=0x1e ; sccb_dat[2]=0x2a ;   
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  sccb_dat[0]=0x50; sccb_dat[1]=0x02 ; sccb_dat[2]=0x78 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  sccb_dat[0]=0x50; sccb_dat[1]=0x1f ; sccb_dat[2]=0x01 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  sccb_dat[0]=0x43; sccb_dat[1]=0x00 ; sccb_dat[2]=0x61 ;   // RGB565
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // AEC Settings         
  sccb_dat[0]=0x35; sccb_dat[1]=0x03 ; sccb_dat[2]=0x07 ;   // VTS Manual 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x01 ; sccb_dat[2]=0x73 ;   // shutter speed
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x02 ; sccb_dat[2]=0x80 ;   // shutter speed
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x0b ; sccb_dat[2]=0x00 ;   // AGC Gain
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x03 ; sccb_dat[2]=0x07 ;   // VTS manual  
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // unknown
  sccb_dat[0]=0x38; sccb_dat[1]=0x24 ; sccb_dat[2]=0x11 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // AEC Settings
  sccb_dat[0]=0x35; sccb_dat[1]=0x01 ; sccb_dat[2]=0x1e ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x02 ; sccb_dat[2]=0x80 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // AGC Settings 
  sccb_dat[0]=0x35; sccb_dat[1]=0x0b ; sccb_dat[2]=0x7f ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // output timing settings
  sccb_dat[0]=0x38; sccb_dat[1]=0x0c ; sccb_dat[2]=0x0c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x38; sccb_dat[1]=0x0d ; sccb_dat[2]=0x80 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x38; sccb_dat[1]=0x0e ; sccb_dat[2]=0x03 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x38; sccb_dat[1]=0x0f ; sccb_dat[2]=0xe8 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // flicker-less settings
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0d ; sccb_dat[2]=0x04 ;   // 60Hz
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0e ; sccb_dat[2]=0x03 ;   // 50Hz
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // timing settings
  sccb_dat[0]=0x38; sccb_dat[1]=0x18 ; sccb_dat[2]=0xc1 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // analog settings register 
  sccb_dat[0]=0x37; sccb_dat[1]=0x05 ; sccb_dat[2]=0xdb ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x37; sccb_dat[1]=0x0a ; sccb_dat[2]=0x81 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // array control 
  sccb_dat[0]=0x36; sccb_dat[1]=0x21 ; sccb_dat[2]=0xc7 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // output timing
  sccb_dat[0]=0x38; sccb_dat[1]=0x01 ; sccb_dat[2]=0x50 ; // H-Start:80
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x38; sccb_dat[1]=0x03 ; sccb_dat[2]=0x08 ; // V-Start:8
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // unknown
  sccb_dat[0]=0x38; sccb_dat[1]=0x27 ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // HV offset settings
  sccb_dat[0]=0x38; sccb_dat[1]=0x10 ; sccb_dat[2]=0xc0 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // output timing
  sccb_dat[0]=0x38; sccb_dat[1]=0x04 ; sccb_dat[2]=0x05 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x38; sccb_dat[1]=0x05 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  // Statistics Settings
  sccb_dat[0]=0x56; sccb_dat[1]=0x82 ; sccb_dat[2]=0x05 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x56; sccb_dat[1]=0x83 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // output timing
  sccb_dat[0]=0x38; sccb_dat[1]=0x06 ; sccb_dat[2]=0x03 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x38; sccb_dat[1]=0x07 ; sccb_dat[2]=0xc0 ; // V-pixel:960
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  // Statistics Settings
  sccb_dat[0]=0x56; sccb_dat[1]=0x86 ; sccb_dat[2]=0x03 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x56; sccb_dat[1]=0x87 ; sccb_dat[2]=0xc0 ; // V-pixel:960
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #102:AEC Settings
  sccb_dat[0]=0x3a; sccb_dat[1]=0x00 ; sccb_dat[2]=0x78 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x1a ; sccb_dat[2]=0x04 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x13 ; sccb_dat[2]=0x30 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x18 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x19 ; sccb_dat[2]=0x7c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #107: flicker-less settings
  sccb_dat[0]=0x3a; sccb_dat[1]=0x08 ; sccb_dat[2]=0x12 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x09 ; sccb_dat[2]=0xc0 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0a ; sccb_dat[2]=0x0f ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0b ; sccb_dat[2]=0xa0 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #111: block clock enable
  sccb_dat[0]=0x30; sccb_dat[1]=0x04 ; sccb_dat[2]=0xff ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);  

  // #112: AEC Settings
  sccb_dat[0]=0x35; sccb_dat[1]=0x0c ; sccb_dat[2]=0x07 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);  
  sccb_dat[0]=0x35; sccb_dat[1]=0x0d ; sccb_dat[2]=0xd0 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);  
  sccb_dat[0]=0x35; sccb_dat[1]=0x00 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x01 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x02 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #117: AGC/AEC Settings
  sccb_dat[0]=0x35; sccb_dat[1]=0x0a ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x0b ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x35; sccb_dat[1]=0x03 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #120: De-Noise Settings
  sccb_dat[0]=0x52; sccb_dat[1]=0x8a ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x8b ; sccb_dat[2]=0x04 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x8c ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x8d ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x8e ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x8f ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x90 ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x92 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x93 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x94 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x95 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x96 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x97 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x98 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x99 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x9a ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x9b ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x9c ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x9d ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x9e ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x9f ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #141: AEC Settings
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0f ; sccb_dat[2]=0x3c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x10 ; sccb_dat[2]=0x30 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x1b ; sccb_dat[2]=0x3c ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x1e ; sccb_dat[2]=0x30 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x3a; sccb_dat[1]=0x11 ; sccb_dat[2]=0x70 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x1f ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #147: system settings 
  sccb_dat[0]=0x30; sccb_dat[1]=0x30 ; sccb_dat[2]=0x0b ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #148: AEC Settings
  sccb_dat[0]=0x3a; sccb_dat[1]=0x02 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x03 ; sccb_dat[2]=0x7d ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x04 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x14 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x15 ; sccb_dat[2]=0x7d ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x16 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x00 ; sccb_dat[2]=0x7c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #155: flicker-less settings
  sccb_dat[0]=0x3a; sccb_dat[1]=0x08 ; sccb_dat[2]=0x09 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x3a; sccb_dat[1]=0x09 ; sccb_dat[2]=0x60 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0a ; sccb_dat[2]=0x07 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0b ; sccb_dat[2]=0xd0 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0d ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0e ; sccb_dat[2]=0x06 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #161: AWB settings
  sccb_dat[0]=0x51; sccb_dat[1]=0x93 ; sccb_dat[2]=0x70 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #162: analog settings
  sccb_dat[0]=0x36; sccb_dat[1]=0x20 ; sccb_dat[2]=0x57 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x37; sccb_dat[1]=0x03 ; sccb_dat[2]=0x98 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x37; sccb_dat[1]=0x04 ; sccb_dat[2]=0x1c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #165: unknown settings
  sccb_dat[0]=0x58; sccb_dat[1]=0x9b ; sccb_dat[2]=0x04 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x58; sccb_dat[1]=0x9a ; sccb_dat[2]=0xc5 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #167: De-Noise Settings
  sccb_dat[0]=0x52; sccb_dat[1]=0x8a ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x8b ; sccb_dat[2]=0x02 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x8c ; sccb_dat[2]=0x08 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x8d ; sccb_dat[2]=0x10 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x8e ; sccb_dat[2]=0x20 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x8f ; sccb_dat[2]=0x28 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x90 ; sccb_dat[2]=0x30 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x92 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x93 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x94 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x95 ; sccb_dat[2]=0x02 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x96 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x97 ; sccb_dat[2]=0x08 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x98 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x99 ; sccb_dat[2]=0x10 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x9a ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x9b ; sccb_dat[2]=0x20 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x9c ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x9d ; sccb_dat[2]=0x28 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  sccb_dat[0]=0x52; sccb_dat[1]=0x9e ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);  
  sccb_dat[0]=0x52; sccb_dat[1]=0x9f ; sccb_dat[2]=0x30 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x52; sccb_dat[1]=0x82 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #189: CIP
  sccb_dat[0]=0x53; sccb_dat[1]=0x00 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x01 ; sccb_dat[2]=0x20 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x02 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x03 ; sccb_dat[2]=0x7c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x0c ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x0d ; sccb_dat[2]=0x0c ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x0e ; sccb_dat[2]=0x20 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x0f ; sccb_dat[2]=0x80 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x10 ; sccb_dat[2]=0x20 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x11 ; sccb_dat[2]=0x80 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x08 ; sccb_dat[2]=0x20 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x09 ; sccb_dat[2]=0x40 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x04 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x05 ; sccb_dat[2]=0x30 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x06 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x07 ; sccb_dat[2]=0x80 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x14 ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x15 ; sccb_dat[2]=0x20 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x19 ; sccb_dat[2]=0x30 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x16 ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x17 ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x18 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #211: Color Matrix Settings
  sccb_dat[0]=0x53; sccb_dat[1]=0x80 ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x81 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x82 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x83 ; sccb_dat[2]=0x4e ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x84 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x85 ; sccb_dat[2]=0x0f ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x86 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x87 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x88 ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x89 ; sccb_dat[2]=0x15 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x8a ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x8b ; sccb_dat[2]=0x31 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x8c ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x8d ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x8e ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x8f ; sccb_dat[2]=0x0f ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x90 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x91 ; sccb_dat[2]=0xab ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x92 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x93 ; sccb_dat[2]=0xa2 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x53; sccb_dat[1]=0x94 ; sccb_dat[2]=0x08 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #232: Gamma Setings 
  sccb_dat[0]=0x54; sccb_dat[1]=0x80 ; sccb_dat[2]=0x14 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x81 ; sccb_dat[2]=0x21 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x82 ; sccb_dat[2]=0x36 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x83 ; sccb_dat[2]=0x57 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x84 ; sccb_dat[2]=0x65 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x85 ; sccb_dat[2]=0x71 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x86 ; sccb_dat[2]=0x7d ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x87 ; sccb_dat[2]=0x87 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x88 ; sccb_dat[2]=0x91 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x89 ; sccb_dat[2]=0x9a ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x8a ; sccb_dat[2]=0xaa ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x8b ; sccb_dat[2]=0xb8 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x8c ; sccb_dat[2]=0xcd ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x8d ; sccb_dat[2]=0xdd ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x8e ; sccb_dat[2]=0xea ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x8f ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x90 ; sccb_dat[2]=0x05 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x91 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x92 ; sccb_dat[2]=0x04 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x93 ; sccb_dat[2]=0x20 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x94 ; sccb_dat[2]=0x03 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x95 ; sccb_dat[2]=0x60 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x96 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x97 ; sccb_dat[2]=0xb8 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x98 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x99 ; sccb_dat[2]=0x86 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x9a ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x9b ; sccb_dat[2]=0x5b ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x9c ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x9d ; sccb_dat[2]=0x3b ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x9e ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0x9f ; sccb_dat[2]=0x1c ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa0 ; sccb_dat[2]=0x02 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa1 ; sccb_dat[2]=0x04 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa2 ; sccb_dat[2]=0x01 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa3 ; sccb_dat[2]=0xed ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa4 ; sccb_dat[2]=0x01 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa5 ; sccb_dat[2]=0xc5 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa6 ; sccb_dat[2]=0x01 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa7 ; sccb_dat[2]=0xa5 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa8 ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xa9 ; sccb_dat[2]=0x6c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xaa ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xab ; sccb_dat[2]=0x41 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xac ; sccb_dat[2]=0x01 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xad ; sccb_dat[2]=0x20 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xae ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x54; sccb_dat[1]=0xaf ; sccb_dat[2]=0x16 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #280: AWB Settings 
  sccb_dat[0]=0x34; sccb_dat[1]=0x06 ; sccb_dat[2]=0x00 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x92 ; sccb_dat[2]=0x04 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x91 ; sccb_dat[2]=0xf8 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x93 ; sccb_dat[2]=0x70 ; //R
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x94 ; sccb_dat[2]=0xf0 ; //G
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x95 ; sccb_dat[2]=0xf0 ; //B
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x8d ; sccb_dat[2]=0x3d ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x8f ; sccb_dat[2]=0x54 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x8e ; sccb_dat[2]=0x3d ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x90 ; sccb_dat[2]=0x54 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x8b ; sccb_dat[2]=0xc0 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x8c ; sccb_dat[2]=0xbd ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x87 ; sccb_dat[2]=0x18 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x88 ; sccb_dat[2]=0x18 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x89 ; sccb_dat[2]=0x6e ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x8a ; sccb_dat[2]=0x68 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x86 ; sccb_dat[2]=0x1c ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x81 ; sccb_dat[2]=0x50 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #298: AWB Settings
  sccb_dat[0]=0x51; sccb_dat[1]=0x84 ; sccb_dat[2]=0x25 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x82 ; sccb_dat[2]=0x11 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x83 ; sccb_dat[2]=0x14 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x84 ; sccb_dat[2]=0x25 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x51; sccb_dat[1]=0x85 ; sccb_dat[2]=0x24 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // ISP Settings
  sccb_dat[0]=0x50; sccb_dat[1]=0x25 ; sccb_dat[2]=0x82 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  // #304: AEC Settings
  sccb_dat[0]=0x3a; sccb_dat[1]=0x0f ; sccb_dat[2]=0x7e ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x10 ; sccb_dat[2]=0x72 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x1b ; sccb_dat[2]=0x80 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x1e ; sccb_dat[2]=0x70 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x11 ; sccb_dat[2]=0xd0 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x3a; sccb_dat[1]=0x1f ; sccb_dat[2]=0x40 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);
  
  // #310: Digital effect 
  sccb_dat[0]=0x55; sccb_dat[1]=0x83 ; sccb_dat[2]=0x40 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x55; sccb_dat[1]=0x84 ; sccb_dat[2]=0x40 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x55; sccb_dat[1]=0x80 ; sccb_dat[2]=0x02 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #313: Analog settings register
  sccb_dat[0]=0x36; sccb_dat[1]=0x33 ; sccb_dat[2]=0x07 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x37; sccb_dat[1]=0x02 ; sccb_dat[2]=0x10 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x37; sccb_dat[1]=0x03 ; sccb_dat[2]=0xb2 ; 
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x37; sccb_dat[1]=0x04 ; sccb_dat[2]=0x18 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  sccb_dat[0]=0x37; sccb_dat[1]=0x0b ; sccb_dat[2]=0x40 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // array control 
  sccb_dat[0]=0x37; sccb_dat[1]=0x0d ; sccb_dat[2]=0x02 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  // #319: analog settings register 
  sccb_dat[0]=0x36; sccb_dat[1]=0x20 ; sccb_dat[2]=0x52 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  
  #if 1 // pclk=6MHz
  sccb_dat[0]=0x30; sccb_dat[1]=0x11 ; sccb_dat[2]=0x08 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1);

  sccb_dat[0]=0x30; sccb_dat[1]=0x12 ; sccb_dat[2]=0x00 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x30; sccb_dat[1]=0x10 ; sccb_dat[2]=0x70 ;  
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x46; sccb_dat[1]=0x0c ; sccb_dat[2]=0x22 ;  
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  
  sccb_dat[0]=0x38; sccb_dat[1]=0x0c ; sccb_dat[2]=0x0c ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x38; sccb_dat[1]=0x0d ; sccb_dat[2]=0x80 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x3a; sccb_dat[1]=0x00 ; sccb_dat[2]=0x78 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x3a; sccb_dat[1]=0x08 ; sccb_dat[2]=0x09 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x3a; sccb_dat[1]=0x09 ; sccb_dat[2]=0x60 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x3a; sccb_dat[1]=0x0a ; sccb_dat[2]=0x07 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x3a; sccb_dat[1]=0x0b ; sccb_dat[2]=0xd0 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x3a; sccb_dat[1]=0x0d ; sccb_dat[2]=0x08 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 

  sccb_dat[0]=0x3a; sccb_dat[1]=0x0e ; sccb_dat[2]=0x06 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  #endif

  //VSYNC Active-Low
  sccb_dat[0]=0x47; sccb_dat[1]=0x40 ; sccb_dat[2]=0x01 ;
  HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  // sccb_dat[0]=0x3a; sccb_dat[1]=0x00 ; sccb_dat[2]=0x78 ;

}