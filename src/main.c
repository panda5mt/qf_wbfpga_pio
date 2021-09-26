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

/*==========================================================
 *
 *    File   : main.c
 *    Purpose: main for QuickFeather CAMERA IF test
 *
 *=========================================================*/

#include "Fw_global_config.h"   // This defines application specific charactersitics

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "RtosTask.h"

/*    Include the generic headers required for QORC */
#include "eoss3_hal_gpio.h"
#include "eoss3_hal_rtc.h"
#include "eoss3_hal_sdma.h"
#include "eoss3_hal_timer.h"
#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"
#include "cli.h"

#include "fpga_loader.h"    // API for loading FPGA
#include "AL4S3B_FPGA_top_bit.h"   // FPGA bitstream to load into FPGA

#include "gateware.h"           // FPGA bitstream to load into FPGA
//#include "sensor_config.h"
#include "datablk_mgr.h"
#include "process_ids.h"
#include "datablk_processor.h"
#include "process_imu.h"
#include "sccb_if.h"
#include "fpga_modctrl.h"
#include "spi0_comm.h"

extern const struct cli_cmd_entry my_main_menu[];

uint32_t a[512*8];
volatile int32_t cntr=0;
#if DBG_FLAGS_ENABLE
uint32_t DBG_flags = DBG_flags_default;
#endif

const char *SOFTWARE_VERSION_STR;

uint8_t DeviceClassUUID[UUID_TOTAL_BYTES] =
{ 0x4d, 0x36, 0xe9, 0x78,
  0xe3, 0x25, 0x11, 0xce,
  0xbf, 0xc1, 0x08, 0x00,
  0x2b, 0xe1, 0x03, 0x18
};

char *pSupportedPaths[NUM_SUPPORTED_PATHS] = {"/default/"};

/*
 * Global variable definition
 */
extern void qf_hardwareSetup();
static void nvic_init(void);
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

int main(void)
{
    uint32_t flg;
  
    //SOFTWARE_VERSION_STR = "qorc-sdk/qf_apps/qf_wbfpga_pio";
#if S3AI_FIRMWARE_IS_COLLECTION
    SOFTWARE_VERSION_STR = "C Jun-2020";
#endif
#if S3AI_FIRMWARE_IS_RECOGNITION
    SOFTWARE_VERSION_STR = "R Jun-2020";
#endif

    qf_hardwareSetup();
    nvic_init();

    S3x_Clk_Disable(S3X_FB_21_CLK);
    S3x_Clk_Disable(S3X_FB_16_CLK);
    S3x_Clk_Enable(S3X_A1_CLK);
    S3x_Clk_Enable(S3X_CFG_DMA_A1_CLK);
    
    load_fpga(sizeof(axFPGABitStream), axFPGABitStream);     // Load bitstrem into FPGA
    fpga_modctrl_init();
    
    HAL_Delay_Init();


    // GPIO init
    fpga_gpio_setdir(0xff);
    *(volatile uint32_t *)fb_gpioout = 0x0f;
    
    // init task
    xTaskCreate(vTask1,"Task1", 100, NULL, 1, NULL);
    xTaskCreate(vTask2,"Task2", 100, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1) ;
}


void vTask1(void *pvParameters){
  while(1){
    cntr = 0;
    // init ov5642
    sccb_init();

    while(1){
        while(0x10 == *(volatile uint32_t *)fb_status);
        memcpy(&a[0], fb_ram0, (512 * sizeof(uint32_t)));   // ram0 -> a
        while(0x11 == *(volatile uint32_t *)fb_status);
        memcpy(&a[512], fb_ram1, (512 * sizeof(uint32_t))); // ram1 -> a
        
        cntr+=2;
        
        while(0x12 == *(volatile uint32_t *)fb_status);
        memcpy(&a[512*2], fb_ram2, (512 * sizeof(uint32_t))); // ram2 -> a
        while(0x13 == *(volatile uint32_t *)fb_status);
        memcpy(&a[512*3], fb_ram3, (512 * sizeof(uint32_t))); // ram3 -> a
        
        cntr+=2;

        while(0x10 == *(volatile uint32_t *)fb_status);
        memcpy(&a[512*4], fb_ram0, (512 * sizeof(uint32_t))); // ram0 -> a
        while(0x11 == *(volatile uint32_t *)fb_status);
        memcpy(&a[512*5], fb_ram1, (512 * sizeof(uint32_t))); // ram1 -> a
        
        cntr+=2;
        
        while(0x12 == *(volatile uint32_t *)fb_status);
        memcpy(&a[512*6], fb_ram2, (512 * sizeof(uint32_t))); // ram2 -> a
        while(0x13 == *(volatile uint32_t *)fb_status);
        memcpy(&a[512*7], fb_ram3, (512 * sizeof(uint32_t))); // ram3 -> a
        
        cntr+=2;
    }

    while(1);
    
  }
}

void vTask2(void *pvParameters) {

    int32_t j = 0;
    uint32_t nowptr = 0;    
    HAL_StatusTypeDef ret_data;

    
    // ret_data = spi0_init();
    // if(HAL_OK == ret_data) {
    //     //dbg_str("SPI Init OK\r\n");   
    // }

    // ret_data = spi0_trans(0x55);
    // if(HAL_OK == ret_data) {
    //     //dbg_str("SPI Trans OK\r\n");
    // }

    while(1) {
        if(cntr > 3) {
            for(uint32_t i = 0 ; i < 512 * 8 ; i+=32) {
                // dbg_ch_raw((a[i])& 0xff);
                // dbg_ch_raw((a[i] >> 8)& 0xff);
                // dbg_ch_raw((a[i] >> 16)& 0xff);
                // dbg_ch_raw((a[i] >> 24) & 0xff);
                dbg_hex32(a[i]);
                dbg_str("\n");
            }
            cntr -= 8;
        }
    }


    while(1);
}


static void nvic_init(void)
 {
    // To initialize system, this interrupt should be triggered at main.
    // So, we will set its priority just before calling vTaskStartScheduler(), not the time of enabling each irq.
    NVIC_SetPriority(Ffe0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SpiMs_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CfgDma_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(Uart_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(FbMsg_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
 }

//needed for startup_EOSS3b.s asm file
void SystemInit(void)
{

}

//missing functions for S3 project
void wait_ffe_fpga_load(void){ return; };

