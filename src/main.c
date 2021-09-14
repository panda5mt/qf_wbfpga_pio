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



#define FIFO_CH1 (1)
#define FIFO_CH2 (2)
#define FIFO_CH3 (3)

extern const struct cli_cmd_entry my_main_menu[];

volatile uint32_t **fifo1_regs = 0x40020100; 
volatile uint32_t **fifo2_regs = 0x40020200; 
volatile uint32_t **fifo3_regs = 0x40020400; 


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
    fpga_fifoctrl_init();

    HAL_Delay_Init();
    // dbg_str("\n\n");
    // dbg_str( "##########################\n");
    // dbg_str( "Quicklogic QuickFeather FPGA Example\n");
    // dbg_str( "SW Version: ");
    // dbg_str( SOFTWARE_VERSION_STR );
    // dbg_str( "\n" );
    // dbg_str( __DATE__ " " __TIME__ "\n" );
    // dbg_str( "##########################\n\n");

	//   dbg_str( "\n\nfpga test...\n\n");	// <<<<<<<<<<<<<<<<<<<<<  Change me!



    // GPIO init
    fpga_gpio_setdir(0xff);
    
    // init ov5642
    sccb_init();
volatile uint32_t a[512*6];
volatile uint32_t flg, lp;

// test for ringbuffer in FPGA
uint32_t ch = 2;
uint32_t a_ptr = 0; 
while(1) {
while(1) {
    
    
    flg = fpga_getflag(ch) & 0x000f;
    //if((flg > 0x03) || (!flg)) {
    if((!flg)) {

        for(uint32_t i=0 ; i<512 ;i++) {
            switch(ch){
                case 1:
                    a[a_ptr + i] = *(volatile uint32_t *)fifo1_regs;
                    break;
                case 2:
                    a[a_ptr + i] = *(volatile uint32_t *)fifo2_regs;
                    break;
                case 3:
                    a[a_ptr + i] = *(volatile uint32_t *)fifo3_regs;
                    break;
                default:
                    break;
            }

                
            
        }
        a_ptr = a_ptr + 512;
    }    

    ch = ch + 1;
    if (ch > 3) ch = 1;

    if(a_ptr >= (512*6)-1)break;
}

    a_ptr = 0;
    for(uint32_t i=0 ; i<512*6 ; i++) {
        dbg_str("0x");dbg_hex32(a[i]);dbg_str("\r\n");
    }
    
}
    // dbg_str("\r\nsta = 0x"); dbg_hex32(flgx); dbg_str("\r\n");
    // dbg_str("\r\nfin = 0x"); dbg_hex32(flg2); dbg_str("\r\n"); dbg_str("\r\n");

 
    
    // init task
    xTaskCreate(vTask1,"Task1", 100, NULL, 1, NULL);
    xTaskCreate(vTask2,"Task2", 100, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1) ;
}


void vTask1(void *pvParameters){
  while(1){
      fpga_setgpio(0x05);
      vTaskDelay(400);
      fpga_setgpio(0x0a);
      vTaskDelay(400);
    
  }
}
void vTask2(void *pvParameters){
  while(1){
    vTaskDelay(500);
    //dbg_str("\r\n\r\nLED Blink Test!\r\nRead data from FPGA=0x");
    //dbg_hex32(fpga_getgpio());
  }
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
