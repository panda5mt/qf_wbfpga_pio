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

#ifndef __FPGA_LEDCTLR_H_
#define __FPGA_LEDCTLR_H_

#include <stdbool.h>

#include <FreeRTOS.h>
#include <queue.h>

#include <eoss3_dev.h>
#include <eoss3_hal_def.h>
#include "s3x_clock.h"

//#define FPGA_LEDCTLR_ID_VALUE         0xA5BD
//#define FPGA_LEDCTLR_REV_NUM          0x0100

typedef struct fpga_ledctlr_regs {
    uint32_t    device_id;			// 0x00
    uint32_t    rev_num;			// 0x04
    uint16_t    scratch_reg;		// 0x08
    uint16_t    reserved1;			// 0x0A
    uint32_t    reserved2;			// 0x0C
    uint8_t    	color0; 			// 0x10
    uint8_t    	color1; 			// 0x11
	uint8_t    	color2; 			// 0x12
	uint8_t    	color3; 			// 0x13
    uint32_t    reserved7[3];		// 0x14
	uint32_t	duration0;			// 0x20
	uint32_t	duration1;			// 0x24
	uint32_t	duration2;			// 0x28
	uint32_t	duration3;			// 0x2C
} fpga_ledctlr_regs_t;

typedef struct fpga_ledctlr_regs2 {
    uint32_t    device_id;			// 0x00
    uint32_t    rev_num;			// 0x04
    uint16_t    scratch_reg;		// 0x08
    uint16_t    reserved1;			// 0x0A
    uint32_t    reserved2;			// 0x0C
    uint32_t    colors;				// 0x10
    uint32_t    reserved7[3];		// 0x14
	uint32_t	duration0;			// 0x20
	uint32_t	duration1;			// 0x24
	uint32_t	duration2;			// 0x28
	uint32_t	duration3;			// 0x2C
} fpga_ledctlr_regs2_t;

typedef struct fpga_fifoctrl {
    uint32_t    device_id;			// 0x00
    uint32_t    rev_num;			// 0x04
    uint32_t    gpio_in;			// 0x08
    uint32_t    gpio_out;		    // 0x0C
    uint32_t    gpio_oe;			// 0x10
    uint32_t    reserved1[60];
    uint32_t    fifo1_acc;          // 0x100
    uint32_t    fifo1_flags;    	// 0x104
    uint32_t    reserved2[60];
    uint32_t    fifo2_acc; 		    // 0x200
	uint32_t	fifo2_flags;    	// 0x204
    uint32_t    reserved3[120];
	uint32_t	fifo3_acc;			// 0x400
	uint32_t	fifo3_flags;        // 0x404

} fpga_fifoctrl_t;

uint32_t    fpga_fifoctrl_getgpio(void);
void    	fpga_fifoctrl_init(void);
void        fpga_fifoctrl_setgpio(uint32_t value);
void        fpga_fifoctrl_gpio_setdir(uint32_t value); // 0:tri-state 1:Drive output


uint32_t fpga_ledctlr_getcolors(void);
void    	fpga_ledctlr_setcolor(uint8_t ucColorValue, uint8_t ucTimerIndex);
uint8_t 	fpga_ledctlr_getcolor(uint8_t ucTimerIndex);
void    	fpga_ledctlr_setduration(uint16_t uhDuration, uint8_t ucTimerIndex);
uint16_t	fpga_ledctlr_getduration(uint8_t ucTimerIndex);

#endif // __FPGA_LEDCTLR_H_