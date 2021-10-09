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

#include <stdint.h>
#include <stdio.h>

#include "Fw_global_config.h"
#include "fpga_modctrl.h"

//------------- Pointer to registers ---------------------//
//fpga_modctrl_t* modctrl = (fpga_modctrl_t*)(FPGA_PERIPH_BASE);

//------------- Local functions -------------------------//
// memory maps on FPGA
volatile uint32_t **fb_devid	= 0x40020000;
volatile uint32_t **fb_revnum 	= 0x40020004;
volatile uint32_t **fb_gpioin 	= 0x40020008;
volatile uint32_t **fb_gpioout	= 0x4002000C;
volatile uint32_t **fb_gpiooe 	= 0x40020010;
volatile uint32_t **fb_ram0		= 0x40022000;
volatile uint32_t **fb_ram1		= 0x40024000; 
volatile uint32_t **fb_ram2		= 0x40026000; 
volatile uint32_t **fb_ram3		= 0x40028000; 
volatile uint32_t **fb_status 	= 0x4002a000;


//------------- Local variables ------------------------//
void fpga_modctrl_init(void) {

	// see the TRM p.340
	S3x_Clk_Set_Rate(S3X_FB_02_CLK, F_10MHZ); 	// C02:Sys_Pclk(up to 40MHz) -> WISHBONE Clock
	S3x_Clk_Set_Rate(S3X_FB_21_CLK, F_8MHZ);	// C21:Sys_Clk1(up to 72MHz)
	S3x_Clk_Set_Rate(S3X_FB_16_CLK, F_24MHZ);	// C16:Sys_Clk0(up to 72MHz) 
	
	S3x_Clk_Enable(S3X_FB_16_CLK);
	S3x_Clk_Enable(S3X_FB_21_CLK);
	S3x_Clk_Enable(S3X_FB_02_CLK);
	
	#if 1
	S3x_Register_Qos_Node(S3X_FB_16_CLK);
  	S3x_Set_Qos_Req(S3X_FB_16_CLK, MIN_HSOSC_FREQ, HSOSC_72MHZ);
	#endif
    // Confirm expected IP is loaded
	configASSERT(*(volatile uint32_t *)(fb_devid) == 0x56A37E57); //RAM:0x56A37E57, FIFO:0xF1F07E57
}

void fpga_setgpio(uint32_t value) {
	*(volatile uint32_t *)(fb_gpioout) = value ;
}

uint32_t fpga_getgpio(void) {
	return (*(volatile uint32_t *)(fb_gpioin)) ;
}

void fpga_gpio_setdir(uint32_t value) {
	*(volatile uint32_t *)(fb_gpiooe) = value;
}



