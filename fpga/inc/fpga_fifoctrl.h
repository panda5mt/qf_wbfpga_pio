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

typedef struct fpga_fifoctrl {
    uint32_t    device_id;			// 0x00
    uint32_t    rev_num;			// 0x04
    uint32_t    gpio_in;			// 0x08
    uint32_t    gpio_out;		    // 0x0C
    uint32_t    gpio_oe;			// 0x10
    uint32_t    reserved1[60-1];
    uint32_t    fifo1_acc;          // 0x100
    uint32_t    fifo1_flags;    	// 0x104
    uint32_t    reserved2[63-1];
    uint32_t    fifo2_acc; 		    // 0x200
	uint32_t	fifo2_flags;    	// 0x204
    uint32_t    reserved3[127-1];
	uint32_t	fifo3_acc;			// 0x400
	uint32_t	fifo3_flags;        // 0x404
} fpga_fifoctrl_t;

// init API
void    	fpga_fifoctrl_init(void);

// GPIO APIs
uint32_t    fpga_getgpio(void);
void        fpga_setgpio(uint32_t value);
void        fpga_gpio_setdir(uint32_t value); // 0:tri-state 1:Drive output

// FIFO APIs
uint32_t    fpga_getflag(uint8_t ch);
uint32_t    fpga_getfifo(uint8_t ch); 
void        fpga_setfifo(uint8_t ch, uint32_t value);

#endif // __FPGA_LEDCTLR_H_