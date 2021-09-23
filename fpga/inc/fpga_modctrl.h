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

#ifndef __FPGA_MODCTRL_H_
#define __FPGA_MODCTRL_H_

#include <stdbool.h>

#include <FreeRTOS.h>
#include <queue.h>

#include <eoss3_dev.h>
#include <eoss3_hal_def.h>
#include "s3x_clock.h"

// memory maps on FPGA
volatile uint32_t **devid_regs ;
volatile uint32_t **revnum_regs;
volatile uint32_t **gpioin_regs;
volatile uint32_t **gpioout_regs;
volatile uint32_t **gpiooe_regs;
volatile uint32_t **ram0_regs;
volatile uint32_t **ram1_regs; 
volatile uint32_t **ram2_regs; 
volatile uint32_t **ram3_regs; 
volatile uint32_t **status_regs;


// init API
void    	fpga_modctrl_init(void);

// GPIO APIs
uint32_t    fpga_getgpio(void);
void        fpga_setgpio(uint32_t value);
void        fpga_gpio_setdir(uint32_t value); // 0:tri-state 1:Drive output

#endif // __FPGA_MODCTRL_H_