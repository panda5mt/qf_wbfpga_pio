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
 *    Purpose: main for QuickFeather helloworldsw and LED/UserButton test
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
#include "eoss3_hal_fpga_usbserial.h"
#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"
#include "cli.h"

#include "fpga_loader.h"    // API for loading FPGA
#include "AL4S3B_FPGA_top_bit.h"   // FPGA bitstream to load into FPGA

#include "gateware.h"           // FPGA bitstream to load into FPGA
#include "sensor_config.h"
#include "datablk_mgr.h"
#include "process_ids.h"
#include "datablk_processor.h"
#include "process_imu.h"
#include "eoss3_hal_i2c.h"
#include "mc3635.h"



#define FIFO_CH1 (1)
#define FIFO_CH2 (2)
#define FIFO_CH3 (3)


extern const struct cli_cmd_entry my_main_menu[];

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
#define IMU_BLOCK_PROCESSOR (1)
#if (IMU_BLOCK_PROCESSOR == 1)
/*========== BEGIN: IMU SENSOR Datablock processor definitions =============*/
/** @addtogroup QAI_IMU_PIPELINE_EXAMPLE QuickAI SDK IMU pipeline example
 *
 *  @brief IMU (Inertial measurement unit) pipeline example code
 *
 * This example code demonstrates setting up IMU Queues,
 * setting up the datablock buffer manager (\ref DATABLK_MGR)
 * and setting up the datablock processor processing elements (\ref DATABLK_PE).
 * A specific IMU processing element for motion detection is provided in this
 * example.
 *
 * @{
 */

#define IMU_MAX_BATCH_SIZE (18)      ///< Approximately 10ms @1660Hz
#define IMU_FRAME_SIZE_PER_CHANNEL       (IMU_MAX_BATCH_SIZE)
#define IMU_NUM_CHANNELS      (6)    ///< (X,Y,Z) for Accel and (X,Y,Z) for Gryo
#define IMU_FRAME_SIZE      ( (IMU_FRAME_SIZE_PER_CHANNEL) * (IMU_NUM_CHANNELS) )
#define NUM_IMU_DATA_BLOCKS ( 20)    ///< 100 data blocks approximately 1 sec of data

/** Maximum number of imu data blocks that may be queued for chain processing */
#define MAX_IMU_DATA_BLOCKS                  (NUM_IMU_DATA_BLOCKS)

/** maximum number of vertical (parallel processing elements) that may generate datablock outputs
 *  that may add to the front of the queue.
 *
 *  Queue size of a given datablock processor must be atleast
 *  summation of maximum datablocks of all sensors registered for
 *  processing with some room to handle the vertical depth
 */
#define MAX_THREAD_VERTICAL_DEPTH_DATA_BLOCKS  (5)

#define DBP_IMU_THREAD_Q_SIZE   (MAX_IMU_DATA_BLOCKS+MAX_THREAD_VERTICAL_DEPTH_DATA_BLOCKS)
#define DBP_IMU_THREAD_PRIORITY (10)

typedef struct {
  QAI_DataBlockHeader_t dbHeader;
  int16_t imu_data[IMU_FRAME_SIZE];
} QAI_IMU_DataBlock_t ;

QAI_IMU_DataBlock_t imu_data_blocks[NUM_IMU_DATA_BLOCKS] ;
QAI_DataBlockMgr_t imuBuffDataBlkMgr;
QueueHandle_t  dbp_imu_thread_q;

/* IMU AI processing element functions */
datablk_pe_funcs_t imu_sensiml_ai_funcs = {NULL, imu_ai_data_processor, NULL, NULL, (void *)NULL } ;

// outQ processor for IMU block processor
outQ_processor_t imu_sensiml_ai_outq_processor =
{
  .process_func = NULL,
  .p_dbm = &imuBuffDataBlkMgr,
  .in_pid = IMU_SENSIML_AI_PID,
  .outQ_num = 0,
  .outQ = NULL,
  .p_event_notifier = NULL
};

datablk_pe_descriptor_t  datablk_pe_descr_imu[] =
{ // { IN_ID, OUT_ID, ACTIVE, fSupplyOut, fReleaseIn, outQ, &pe_function_pointers, bypass_function, pe_semaphore }

    /* processing element descriptor for SensiML AI for IMU sensor */
    { IMU_ISR_PID, IMU_SENSIML_AI_PID, true, false, true, &imu_sensiml_ai_outq_processor, &imu_sensiml_ai_funcs, NULL, NULL},
};

datablk_processor_params_t datablk_processor_params_imu[] = {
    { DBP_IMU_THREAD_PRIORITY,
      &dbp_imu_thread_q,
      sizeof(datablk_pe_descr_imu)/sizeof(datablk_pe_descr_imu[0]),
      datablk_pe_descr_imu,
      256,
      "DBP_IMU_THREAD",
      NULL
    }
};

/* IMU sensors data reading function */
#define IMU_ISR_OUTQS_NUM (1)
QueueHandle_t *imu_isr_outQs[IMU_ISR_OUTQS_NUM] = { &dbp_imu_thread_q };
extern void imu_sensordata_read_callback(void);
extern void imu_event_notifier(int pid, int event_type, void *p_event_data, int num_data_bytes);
extern void set_first_imu_data_block(void);

outQ_processor_t imu_isr_outq_processor =
{
  .process_func = imu_sensordata_read_callback,
  .p_dbm = &imuBuffDataBlkMgr,
  .in_pid = IMU_ISR_PID,
  .outQ_num = 1,
  .outQ = imu_isr_outQs,
  .p_event_notifier = imu_event_notifier
};

void imu_block_processor(void)
{
  /** IMU datablock processor thread : Create IMU Queues */
  dbp_imu_thread_q = xQueueCreate(DBP_IMU_THREAD_Q_SIZE, sizeof(QAI_DataBlock_t *));
  vQueueAddToRegistry( dbp_imu_thread_q, "IMUPipelineExampleQ" );

  /** IMU datablock processor thread : Setup IMU Thread Handler Processing Elements */
  datablk_processor_task_setup(&datablk_processor_params_imu[0]);

  /** Set the first data block for the ISR or callback function */
  set_first_imu_data_block();
  /* [TBD]: sensor configuration : should this be here or after scheduler starts? */
  //sensor_imu_configure();
}

// temporary buffer to read sensor data from the FIFO
// Note: Needed because FIFO stores Gyro samples first and then Accel samples,
// where as the IMU processor needs Accel samples first and then Gyro samples.
//int16_t fifo_samples[IMU_FRAME_SIZE+6];
int imu_get_max_datablock_size(void)
{
  return IMU_MAX_BATCH_SIZE;
}

TimerHandle_t sensorTimId ;
void dataTimer_Callback(TimerHandle_t hdl)
{
  // Warning: must not call vTaskDelay(), vTaskDelayUntil(), or specify a non zero
  // block time when accessing a queue or a semaphore.
  imu_sensordata_read_callback(); //osSemaphoreRelease(readDataSem_id);
}

static int mc3635_threshold_count = 1;
void mc3635_set_threshold_count(int count)
{
  mc3635_threshold_count = count;
}

int mc3635_get_threshold_count(void)
{
  return mc3635_threshold_count;
}

extern void imu_batch_size_set(int batch_size);

void dataTimerStart(void)
{
  BaseType_t status;
#if (USE_IMU_FIFO_MODE)
    int milli_secs = 17; // reads fifo @17milli-secs intervals
#else
    int milli_secs = 2; // reads when a sample is available (upto 416Hz)
#endif
  // Create periodic timer
  //exec = 1;
  if (!sensorTimId) {
    sensorTimId = xTimerCreate("SensorTimer", pdMS_TO_TICKS(milli_secs), pdTRUE, (void *)0, dataTimer_Callback);
    configASSERT(sensorTimId != NULL);
  }

  if (sensorTimId)  {
    status = xTimerStart (sensorTimId, 0);  // start timer
    if (status != pdPASS)  {
      // Timer could not be started
    }
  }
  //set_first_imu_data_block();
#if (USE_IMU_FIFO_MODE)
  int num_samples;
  int srate = imu_get_accel_odr();
  if (srate > 29) {
    num_samples = srate / 50;
  } else {
    num_samples = 1;
  }
  mc3635_fifo_reset();
  mc3635_fifo_enable_threshold(num_samples);
  mc3635_set_threshold_count(num_samples);
  imu_batch_size_set(num_samples);
  vTaskDelay(2); // delay for 2ms to accumulate atleast 1 threshold
  //enable_lsm6dsm_stream();
#endif
  //mc3635_interrupt_enable();
  mc3635_set_mode(MC3635_MODE_CWAKE);
}

void dataTimerStop(void)
{
  if (sensorTimId) {
    xTimerStop(sensorTimId, 0);
  }
#if (USE_IMU_FIFO_MODE)
  //disable_lsm6dsm_stream();
#endif
  mc3635_interrupt_disable();
}
#endif /* IMU_BLOCK_PROCESSOR */

struct st_dbm_init {
  QAI_DataBlockMgr_t *pdatablk_mgr_handle;
  void  *pmem;
  int mem_size;
  int item_count;
  int item_size_bytes;
} ;

struct st_dbm_init dbm_init_table[] =
{
  {&imuBuffDataBlkMgr,   (void *)  imu_data_blocks, sizeof(  imu_data_blocks), IMU_FRAME_SIZE, sizeof(int16_t)},
//  {&audioBuffDataBlkMgr, (void *)audio_data_blocks, sizeof(audio_data_blocks), DBP_AUDIO_FRAME_SIZE, sizeof(int16_t)},
};

void init_all_datablock_managers(struct st_dbm_init *p_dbm_table, int len_dbm_table)
{
  /** Setup the data block managers */
  for (int k = 0; k < len_dbm_table; k++) {
      datablk_mgr_init( p_dbm_table[k].pdatablk_mgr_handle,
                        p_dbm_table[k].pmem,
                        p_dbm_table[k].mem_size,
                        p_dbm_table[k].item_count,
                        p_dbm_table[k].item_size_bytes
                      );
  }

}

void setup_sensors_data_block_processor(void)
{
  /** Initialize all datablock managers */
  init_all_datablock_managers(dbm_init_table, sizeof(dbm_init_table)/sizeof(struct st_dbm_init));
#if (AUDIO_DRIVER == 1)
  audio_block_processor();
#endif
  imu_block_processor();

#if (AD7476_FPGA_DRIVER == 1)  
  ad7476_block_processor();
#endif
}

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void sccb_init(void);
#if 1
static I2C_Config i2c0config =
{
  .eI2CFreq = I2C_400KHZ,    // 400kHz
  .eI2CInt = I2C_DISABLE,    // enabled interrupt
  .ucI2Cn = 0
};
#endif

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

    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "Quicklogic QuickFeather FPGA Example\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n\n");

	  dbg_str( "\n\nfpga test start!\n\n");	// <<<<<<<<<<<<<<<<<<<<<  Change me!

    // init ov5642
    sccb_init();

    fpga_gpio_setdir(0xff);
    for(uint32_t i=0; i<16; i++) {
      fpga_setfifo(FIFO_CH1,i);
    }
    for(uint32_t i=0; i<16; i++) {
      dbg_str("\r\nsta = 0x");
      dbg_hex32(fpga_getflag(FIFO_CH1));
      dbg_str("....fvalue = 0x");
      dbg_hex32(fpga_getfifo(FIFO_CH1));
      
    }

    xTaskCreate(vTask1,"Task1", 100, NULL, 1, NULL);
    xTaskCreate(vTask2,"Task2", 100, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1) ;
}

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
  sccb_dat[0]=0x43; sccb_dat[1]=0x00 ; sccb_dat[2]=0x30 ;   // YUV422
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

  // sccb_dat[0]=0x47; sccb_dat[1]=0x40 ; sccb_dat[2]=0x21 ;//VSYNC F
  // HAL_I2C_WriteRawData(0x78>>1, sccb_dat, 3, 1); 
  // sccb_dat[0]=0x3a; sccb_dat[1]=0x00 ; sccb_dat[2]=0x78 ;

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
    dbg_str("\r\n\r\nLED Blink Test!\r\nRead data from FPGA=0x");
    dbg_hex32(fpga_getgpio());
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
