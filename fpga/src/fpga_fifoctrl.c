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
#include "fpga_fifoctrl.h"

//------------- Pointer to registers ---------------------//
fpga_fifoctrl_t* fifoctrl_regs = (fpga_fifoctrl_t*)(FPGA_PERIPH_BASE);

//------------- Local functions -------------------------//


//------------- Local variables ------------------------//

void fpga_fifoctrl_init(void) {
    // Setup FPGA clocks
    S3x_Clk_Set_Rate(S3X_FB_21_CLK, 12000*1000);
    S3x_Clk_Set_Rate(S3X_FB_16_CLK, 12000*1000);
    S3x_Clk_Enable(S3X_FB_21_CLK);
	S3x_Clk_Enable(S3X_FB_16_CLK);
    
    // Confirm expected IP is loaded
	configASSERT(fifoctrl_regs->device_id == 0xF1F07E57);
}

void fpga_setgpio(uint32_t value) {
	fifoctrl_regs->gpio_out = value;
}

uint32_t fpga_getgpio(void) {
	return fifoctrl_regs->gpio_in;
}

void fpga_gpio_setdir(uint32_t value) {
	fifoctrl_regs->gpio_oe = value;
}

void fpga_setfifo(uint32_t value) {
	fifoctrl_regs->fifo1_acc = value;
}

uint32_t fpga_getfifo(void) {
	return fifoctrl_regs->fifo1_acc;
}

uint32_t fpga_getflag(void) {
	return fifoctrl_regs->fifo1_flags;
}


