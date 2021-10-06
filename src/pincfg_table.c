/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

#include "Fw_global_config.h"
#include "eoss3_hal_pad_config.h"
#include "eoss3_hal_gpio.h"

PadConfig pincfg_table[] = 
{
  { // setup UART TX
    .ucPin = PAD_44,
    .ucFunc = PAD44_FUNC_SEL_UART_TXD,
    .ucCtrl = PAD_CTRL_SRC_A0,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // setup UART RX
    .ucPin = PAD_45,                     // Options: 14, 16, 25, or 45
    .ucFunc = PAD45_FUNC_SEL_UART_RXD,
    .ucCtrl = PAD_CTRL_SRC_A0,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucPull = PAD_NOPULL,
  },



  { // setup debug input pins Pad 14 - S3_SW_DP_CLK
    .ucMode = PAD_MODE_INPUT_EN,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
    .ucPin = PAD_14,
    .ucFunc = PAD14_FUNC_SEL_SW_DP_CLK,
    .ucPull = PAD_NOPULL,
  },

  { //setup debug input pins Pad 15 - S3_SW_DP_IO
    .ucMode = PAD_MODE_INPUT_EN,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
    .ucPin = PAD_15,
    .ucFunc = PAD15_FUNC_SEL_SW_DP_IO,
    .ucPull = PAD_NOPULL
  },

  {
    //Currently Pad 9 is not configured and has an internal pull-down and is 
    // connected to SWD_DATA that has an external pull-up
    //Pad 9 - remove default pull-down configuration, ensure pin is configured as input
    .ucPin = PAD_9,
    .ucFunc = PAD9_FUNC_SEL_GPIO_1,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS
  },
#if 0 
  {  // configure SCL0
    .ucPin = PAD_0,
    .ucFunc = PAD0_FUNC_SEL_SCL_0,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucPull = PAD_PULLUP,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },

  {  // configure SDA0
    .ucPin = PAD_1,
    .ucFunc = PAD1_FUNC_SEL_SDA_0,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucPull = PAD_PULLUP,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
#endif
// { // Pad 3 -- GPIO
//     .ucPin = PAD_3,
//     .ucFunc = PAD3_FUNC_SEL_FBIO_3,
//     .ucCtrl = PAD_CTRL_SRC_FPGA,
//     .ucMode = PAD_MODE_OUTPUT_EN,
//     .ucPull = PAD_NOPULL,
//     .ucDrv = PAD_DRV_STRENGHT_4MA,
//     .ucSpeed = PAD_SLEW_RATE_FAST,
//     .ucSmtTrg = PAD_SMT_TRIG_EN,
//   },
 { // Pad 16 -- GPIO
    .ucPin = PAD_16,
    .ucFunc = PAD16_FUNC_SEL_FBIO_16,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
  },

   { // Pad 23 -- GPIO(CCLKO)
    .ucPin = PAD_23,
    .ucFunc = PAD23_FUNC_SEL_FBIO_23,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucSmtTrg = PAD_SMT_TRIG_EN,
  },
    { // Pad 24 -- GPIO(PCLKI)
    .ucPin = PAD_24,
    .ucFunc = PAD24_FUNC_SEL_FBIO_24,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 31 -- GPIO (QUAD_CLK)
    .ucPin = PAD_31,
    .ucFunc = PAD31_FUNC_SEL_FBIO_31,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
   { // Pad 13 -- GPIO(HREFI)
    .ucPin = PAD_13,
    .ucFunc = PAD13_FUNC_SEL_FBIO_13,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
 { // Pad 25 -- GPIO(VSYNCI)
    .ucPin = PAD_25,
    .ucFunc = PAD25_FUNC_SEL_FBIO_25,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
   { // Pad 11 -- GPIO
    .ucPin = PAD_11,
    .ucFunc = PAD11_FUNC_SEL_FBIO_11,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 12 -- GPIO
    .ucPin = PAD_12,
    .ucFunc = PAD12_FUNC_SEL_FBIO_12,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 4 -- GPIO
    .ucPin = PAD_4,
    .ucFunc = PAD4_FUNC_SEL_FBIO_4,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
   { // Pad 5 -- GPIO
    .ucPin = PAD_5,
    .ucFunc = PAD5_FUNC_SEL_FBIO_5,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 7 -- GPIO
    .ucPin = PAD_7,
    .ucFunc = PAD7_FUNC_SEL_FBIO_7,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
    { // Pad 8 -- GPIO
    .ucPin = PAD_8,
    .ucFunc = PAD8_FUNC_SEL_FBIO_8,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 10 -- GPIO
    .ucPin = PAD_10,
    .ucFunc = PAD10_FUNC_SEL_FBIO_10,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 3 -- GPIO
    .ucPin = PAD_3,
    .ucFunc = PAD3_FUNC_SEL_FBIO_3,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucPull = PAD_NOPULL,
  },
  // LEDS
   { // setup blue LED
    .ucPin = PAD_18,
    .ucFunc = PAD18_FUNC_SEL_GPIO_4,
    .ucCtrl = PAD_CTRL_SRC_A0,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // setup green LED
    .ucPin = PAD_21,
    .ucFunc = PAD21_FUNC_SEL_GPIO_5,
    .ucCtrl = PAD_CTRL_SRC_A0,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // setup red LED
    .ucPin = PAD_22,
    .ucFunc = PAD22_FUNC_SEL_GPIO_6,
    .ucCtrl = PAD_CTRL_SRC_A0,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // Pad 30 -- QSPI SRAM (SIO(0))
    .ucPin = PAD_30,
    .ucFunc = PAD30_FUNC_SEL_FBIO_30,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucSmtTrg = PAD_SMT_TRIG_EN,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 27 -- QSPI SRAM (SIO(1))
    .ucPin = PAD_27,
    .ucFunc = PAD27_FUNC_SEL_FBIO_27,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucSmtTrg = PAD_SMT_TRIG_EN,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucPull = PAD_NOPULL,
  },
   { // Pad 20 -- QSPI SRAM (SIO(2))
    .ucPin = PAD_20,
    .ucFunc = PAD20_FUNC_SEL_FBIO_20,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucSmtTrg = PAD_SMT_TRIG_EN,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 17 -- QSPI SRAM (SIO(3))
    .ucPin = PAD_17,
    .ucFunc = PAD17_FUNC_SEL_FBIO_17,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucSmtTrg = PAD_SMT_TRIG_EN,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucPull = PAD_NOPULL,
  },
  { // Pad 28 -- QSPI SRAM (nCE)
    .ucPin = PAD_28,
    .ucFunc = PAD28_FUNC_SEL_FBIO_28,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_FAST,
    .ucSmtTrg = PAD_SMT_TRIG_EN,
  },
#if 0
   {
    // Pad6 is the user button
    .ucPin = PAD_6,
    .ucFunc = PAD6_FUNC_SEL_GPIO_0,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucPull = PAD_PULLUP,
    .ucDrv = PAD_DRV_STRENGTH_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS
  },
#endif
  /*
    // SPI0 Master
   { // Pad 31 -- SPI0 Master CLK
    .ucPin = PAD_31,
    .ucFunc = PAD31_FUNC_SEL_SPI_SENSOR_CLK,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // Pad 28 -- SPI0 Master MOSI
    .ucPin = PAD_28,
    .ucFunc = PAD28_FUNC_SEL_SPI_SENSOR_MOSI,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // Pad 30 -- SPI0 Master SSn1
    .ucPin = PAD_30,
    .ucFunc = PAD30_FUNC_SEL_SPI_SENSOR_SSn_1,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
   { // Pad 29 -- SPI0 Master MISO
    .ucPin = PAD_29,
    .ucFunc = PAD29_FUNC_SEL_SPI_SENSOR_MISO,
    .ucCtrl = PAD_CTRL_SRC_OTHER,
    .ucMode = PAD_MODE_INPUT_EN,
    .ucPull = PAD_NOPULL, //PAD_PULLUP,//PAD_NOPULL,
   },
   */
  
  //------------- USB ---------------//
   { // Pad 37 -- USB Pullup control
    .ucPin = PAD_37,
    .ucFunc = PAD37_FUNC_SEL_FBIO_37,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // Pad 41 -- USB D-
    .ucPin = PAD_41,
    .ucFunc = PAD41_FUNC_SEL_FBIO_41,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },
  { // Pad 41 -- USB D+
    .ucPin = PAD_42,
    .ucFunc = PAD42_FUNC_SEL_FBIO_42,
    .ucCtrl = PAD_CTRL_SRC_FPGA,
    .ucMode = PAD_MODE_OUTPUT_EN,
    .ucPull = PAD_NOPULL,
    .ucDrv = PAD_DRV_STRENGHT_4MA,
    .ucSpeed = PAD_SLEW_RATE_SLOW,
    .ucSmtTrg = PAD_SMT_TRIG_DIS,
  },


  // { // Pad 12 -- FPGA MOSI
  //   .ucPin = PAD_12,
  //   .ucFunc = PAD12_FUNC_SEL_FBIO_12,
  //   .ucCtrl = PAD_CTRL_SRC_FPGA,
  //   .ucMode = PAD_MODE_OUTPUT_EN,
  //   .ucPull = PAD_NOPULL,
  //   .ucDrv = PAD_DRV_STRENGHT_4MA,
  //   .ucSpeed = PAD_SLEW_RATE_SLOW,
  //   .ucSmtTrg = PAD_SMT_TRIG_EN,
  // }

};

GPIOCfgTypeDef  gpiocfg_table[] =
{
  // {
  //   //vm1010 dout setup as hardware interrupt pin on PAD_41 (sensor interrupt 6)  //Chandalar jumper J3.4
  //   .usPadNum = PAD_3,
  //   .ucGpioNum = GPIO_0,
  //   .ucFunc = PAD3_FUNC_SEL_SENS_INT_0,
  //   .intr_type = LEVEL_TRIGGERED,
  //   .pol_type = FALL_LOW,
  //   .ucPull = PAD_NOPULL,
  // },

};

int sizeof_pincfg_table = sizeof(pincfg_table)/sizeof(pincfg_table[0]);
int sizeof_gpiocfg_table = sizeof(gpiocfg_table)/sizeof(gpiocfg_table[0]);
