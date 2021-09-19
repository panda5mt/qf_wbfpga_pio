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
fpga_modctrl_t* modctrl_regs = (fpga_modctrl_t*)(FPGA_PERIPH_BASE);

//------------- Local functions -------------------------//


//------------- Local variables ------------------------//
void fpga_modctrl_init(void) {
    // Setup FPGA clocks
	S3x_Clk_Set_Rate(S3X_FB_16_CLK, F_10MHZ);
	S3x_Clk_Set_Rate(S3X_FB_21_CLK, F_24MHZ);
	S3x_Clk_Enable(S3X_FB_16_CLK);
	S3x_Clk_Enable(S3X_FB_21_CLK);
	#if 1
	S3x_Register_Qos_Node(S3X_FB_16_CLK);
  	S3x_Set_Qos_Req(S3X_FB_16_CLK, MIN_HSOSC_FREQ, HSOSC_72MHZ);
	#endif
    // Confirm expected IP is loaded
	configASSERT(modctrl_regs->device_id ==0x56A37E57); //RAM:0x56A37E57, FIFO:0xF1F07E57
}

void fpga_setgpio(uint32_t value) {
	modctrl_regs->gpio_out = value;
}

uint32_t fpga_getgpio(void) {
	return modctrl_regs->gpio_in;
}

void fpga_gpio_setdir(uint32_t value) {
	modctrl_regs->gpio_oe = value;
}
/*
void fpga_setfifo(uint8_t ch, uint32_t value) {
	switch(ch){
	case 1:
		modctrl_regs->fifo1_acc = value;
		break;
	case 2:
		modctrl_regs->fifo2_acc = value;
		break;
	case 3:
		modctrl_regs->fifo3_acc = value;
		break;
	default:
		modctrl_regs->fifo1_acc = value;
	}
}

uint32_t fpga_get_wrch(void) {
	return modctrl_regs->fifo_write_ch;
}

uint32_t fpga_getfifo(uint8_t ch) {
	switch(ch) {
	case 1:
		return modctrl_regs->fifo1_acc;
	case 2:
		return modctrl_regs->fifo2_acc;
	case 3:
		return modctrl_regs->fifo3_acc;
	default:
		return (0xDEADBEEF);
	}
}

uint32_t fpga_getflag(uint8_t ch) {
	switch(ch){
	case 1:
		return modctrl_regs->fifo1_flags;
	case 2:
		return modctrl_regs->fifo2_flags;
	case 3:
		return modctrl_regs->fifo3_flags;
	default:
		return (0xDEADBEEF);
	}
}
*/

