/*
Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file reflects a modified version of th_lib from EEMBC. The reporting logic
in th_results is copied from the original in EEMBC.
==============================================================================*/
/// \file
/// \brief C++ implementations of submitter_implemented.h

#include "api/submitter_implemented.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>


#include "api/internally_implemented.h"
//#include "internally_implemented.cpp"

#if EE_CFG_ENERGY_MODE == 0
/* Profiling utilities */
#include "xtmrctr.h"     /* timer, Xilinx IP Timer Counter */
static XTmrCtr TimerCounterInst;
#define TMRCTR_DEVICE_ID    XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_CNTR_0        0
#define TIMER_CNTR_1        1

double th_timestamp_counts;
u64 th_timer_timestamp;
u64 th_timer_start;
u64 th_calibration_time;

void start_64b_counter() {
    XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);
    XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_1);
}

void stop_64b_counter() {
    XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_0);
    XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_1);
}

u64 get_64b_counter_value() {
    //printf("bytes %u\n\r", sizeof(u64));
    u64 lo_counter = XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
    u64 hi_counter = XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_1);
    u64 counter = (hi_counter << 32) | lo_counter;
    //printf("INFO: hi = %lu, lo = %lu, total = %lu\n\r", hi_counter, lo_counter, counter);
    return counter;
}

double get_elapsed_time(u64 clks) {
    return clks * 1000000.0/XPAR_AXI_TIMER_MCU_CLOCK_FREQ_HZ;
}
#elif EE_CFG_ENERGY_MODE == 1
//GPIO Config for Timestamp
#include "xgpio.h"
#define PIN 0x01
#define GPIO_PMOD_PIN_DEVICE_ID  XPAR_GPIO_0_DEVICE_ID

#define set_pin_high(InstancePtr, Mask) \
        XGpio_DiscreteWrite(InstancePtr, 1, Mask)

#define set_pin_low(InstancePtr, Mask) \
        XGpio_DiscreteClear(InstancePtr, 1, Mask)
XGpio Gpio;
int gpio_status;
#endif


extern uint8_t  gp_buff[MAX_DB_INPUT_SIZE];
extern size_t   g_buff_size;
extern size_t   g_buff_pos;


XMyproject_axi do_resnet;
XMyproject_axi_Config *do_resnet_cfg;

uint8_t src_mem[src_FEATURE_COUNT]; //[src_FEATURE_COUNT*sizeof(float)];//[128];
float dst_mem[dst_FEATURE_COUNT]; //[dst_FEATURE_COUNT*sizeof(float)];//[128];
unsigned char *src_mem_pos;
unsigned char *dst_mem_pos;
uint32_t src_mem_size;
uint32_t dst_mem_size;

uint8_t  floatsize = sizeof(float);


uint8_t slices;
uint8_t bins;


// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
    //init_platform();
    //init_accelerators();
	//th_printf("g_buff_pos %i\r\n", g_buff_pos);
    Xil_DCacheFlushRange((UINTPTR)src_mem, src_FEATURE_COUNT * sizeof(unsigned char));
    Xil_DCacheFlushRange((UINTPTR)dst_mem, dst_FEATURE_COUNT * sizeof(float));
    //this doesn't work, never seems to update src_mem past the first time, just bypassing in th_infer since there's no processing/selection...
    for (int i = 0; i < src_mem_size; i++) {//Init SRC mem with input data (32x32x3, 3072 uint8)
    	uint8_t data_char = 0;
    	memcpy(&data_char,&gp_buff[i],sizeof(uint8_t));
        src_mem[i] = data_char;
    }

    for (int i = 0; i < dst_mem_size; i++) {//Init DST mem with 0's
        dst_mem[i] = 0x00;
    }
    return;
}
void init_accelerators()
{
    //printf("INFO: Initializing accelerator\n\r");
	do_resnet_cfg = XMyproject_axi_LookupConfig(XPAR_MYPROJECT_AXI_DEVICE_ID);
    if (do_resnet_cfg)
    {
        int status  = XMyproject_axi_CfgInitialize(&do_resnet, do_resnet_cfg);
        if (status != XST_SUCCESS)
        {
            //printf("ERROR: Initializing accelerator\n\r");
        }
    }
}
// Add to this method to return real inference results.
void th_results() {
    float *results  = NULL;
    size_t nresults = 0;
    /* USER CODE 1 BEGIN */
    // reset max, i_max
    float max = -128;
    int i_max = -1;
    float res_arr[10] = {0,0,0,0,0,0,0,0,0,0};
    results = res_arr;
    nresults = dst_FEATURE_COUNT;
    /*
    printf("SRC mem: \r\n");
    for (int i =0; i < src_FEATURE_COUNT; i++){
    	printf("%2X, ",(unsigned char) src_mem[i]);
    }
    printf("\r\nGP Buff: \r\n");
    for (int i =0; i < src_FEATURE_COUNT; i++){
    	printf("%2X, ",(unsigned char) gp_buff[i]);
    }
    printf("\r\nDST mem: \r\n");
    for (int i =0; i < dst_FEATURE_COUNT; i++){
    	printf("%.3f, ",(float) dst_mem[i]);
    }
    //printf("\r\n");
    printf("\r\nRaw Values: [");
    for (nresults = 0; nresults < dst_FEATURE_COUNT; ++nresults)
    {
      printf("%.3f,", (float)dst_mem[nresults]);
    }
    printf("]\r\n");
    */
    for(short i = 0; i < dst_FEATURE_COUNT; i++){
          if (dst_mem[i] > max){
        	  max = dst_mem[i];
        	  i_max = i;
          }
    	   //printf("check %i",i);
    }
    //   printf("The max index is: %i\r\n", i_max);
    res_arr[i_max] = 1.0;

    /* USER CODE 1 END */
    /**
     * The results need to be printed back in exactly this forth_printf("m-results-[%0.3f]\r\n", result);mat; if easier
     * to just modify this loop than copy to results[] above, do that.
     */
    th_printf("m-results-[");
        for (size_t i = 0; i < nresults; i++)
        {
            /* N.B. Be sure %f is enabled in SDK */
            th_printf("%.3f", (float)results[i]);
            if (i < (nresults - 1))
            {
                th_printf(",");
            }
        }
        th_printf("]\r\n");
}


void th_infer() {
	init_platform();
	init_accelerators();
    //Xil_DCacheFlushRange((UINTPTR)src_mem, src_FEATURE_COUNT * sizeof(unsigned char));//
    Xil_DCacheFlushRange((UINTPTR)gp_buff, src_FEATURE_COUNT * sizeof(unsigned char));
    Xil_DCacheFlushRange((UINTPTR)dst_mem, dst_FEATURE_COUNT * sizeof(float));
    unsigned char *src_mem_i = gp_buff;//src_mem
    float *dst_mem_i = dst_mem;
    XMyproject_axi_Set_in_V(&do_resnet, (unsigned)src_mem_i);
    XMyproject_axi_Set_out_r(&do_resnet, (unsigned)dst_mem_i);
    //XResnet_axi_Set_batch(&do_resnet, 1);
    XMyproject_axi_Start(&do_resnet);
    /* polling */
    while (!XMyproject_axi_IsDone(&do_resnet));
    Xil_DCacheFlushRange((UINTPTR)dst_mem, dst_FEATURE_COUNT * sizeof(float));
}

float mse(float a[], float b[], int size) {
	float error = 0;
	for (int i = 0; i < size; i++) {
		error += pow((b[i] - a[i]), 2);
		//th_printf("i: %i, b: %.3f a: %.3f, diff: %.3f\r\n",i,b[i],a[i],(b[i] - a[i]));
	}
	return error / size;
}

/// \brief optional API.
void th_final_initialize(void) {
	init_platform();
	init_accelerators();

	floatsize = sizeof(float);
	src_mem_size = (src_FEATURE_COUNT* sizeof(unsigned char));
	dst_mem_size = (dst_FEATURE_COUNT* sizeof(float));

	//malloc_stats();
	//src_mem = malloc(src_mem_size);
	//dst_mem = malloc(dst_mem_size);
	//printf("src: %p, dst %p \n",src_mem,dst_mem);
	//th_printf("INFO: Init Finished!\r\n");
	th_printf(EE_MSG_NAME, EE_DEVICE_NAME, TH_VENDOR_NAME_STRING);
    th_printf("m-profile-[%s]\r\n", EE_FW_VERSION);
    th_printf("m-model-[%s]\r\n", TH_MODEL_VERSION);
}

void th_pre() {}
void th_post() {}

void th_command_ready(char volatile *p_command) {
  p_command = p_command;
  ee_serial_command_parser_callback((char *)p_command);
}

// th_libc implementations.
int th_strncmp(const char *str1, const char *str2, size_t n) {
  return strncmp(str1, str2, n);
}

char *th_strncpy(char *dest, const char *src, size_t n) {
  return strncpy(dest, src, n);
}

size_t th_strnlen(const char *str, size_t maxlen) {
  return strnlen(str, maxlen);
}

char *th_strcat(char *dest, const char *src) { return strcat(dest, src); }

char *th_strtok(char *str1, const char *sep) { return strtok(str1, sep); }

int th_atoi(const char *str) { return atoi(str); }

void *th_memset(void *b, int c, size_t len) { return memset(b, c, len); }

void *th_memcpy(void *dst, const void *src, size_t n) {
  return memcpy(dst, src, n);
}

/* N.B.: Many embedded *printf SDKs do not support all format specifiers. */
int th_vprintf(const char *format, va_list ap) { return vprintf(format, ap); }
void th_printf(const char *p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  (void)th_vprintf(p_fmt, args); /* ignore return */
  va_end(args);
}

char th_getchar() { return getchar(); }

void th_serialport_initialize(void) {
#if EE_CFG_ENERGY_MODE == 1
/* In energy mode, we talk to the DUT through the IO Manager at 9600 baud */
	//Need to re-build firmware to get 9600b sadly ):
#else
/* In performance mode, we talk directly to the DUT at 115200 baud */
		//Need to re-build firmware to get 115200b sadly ):
			/*
			 * Initialize the UART driver so that it's ready to use
			 * Look up the configuration in the config table and then initialize it.
			 */
#endif
}

void th_timestamp(void) {
#if EE_CFG_ENERGY_MODE == 1
/* USER CODE 1 BEGIN */
/* Step 1. Pull pin low */
	set_pin_low(&Gpio, PIN);
/* Step 2. Hold low for at least 1us */
	usleep(10); //Holding for 10us, unsure of max speed of GPIOs, should be fine?
	//continue; //Not worrying about for performance mode
/* Step 3. Release driver */
	set_pin_high(&Gpio, PIN);
/* USER CODE 1 END */
#else
    unsigned long microSeconds = 0ul;
    /* USER CODE 2 BEGIN */
    th_timer_timestamp = get_64b_counter_value();
	th_timestamp_counts = get_elapsed_time(th_timer_timestamp);
	//microSeconds = th_timestamp_counts/th_calibration_time;
    /* USER CODE 2 END */
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP, th_timestamp_counts);
#endif
}

void th_timestamp_initialize(void) {
    /* USER CODE 1 BEGIN */
	//GPIO Initilization
#if EE_CFG_ENERGY_MODE == 1
	int gpio_status = XGpio_Initialize(&Gpio, GPIO_PMOD_PIN_DEVICE_ID);
		if (gpio_status != XST_SUCCESS) {
			xil_printf("GPIO Initialization Failed\r\n");
		}

	set_pin_high(&Gpio, PIN);
	//XGpioPs_SetDirectionPin(&Gpio, PIN, 1);
	//XGpioPs_SetOutputEnablePin(&Gpio, PIN, 1);
	//XGpioPs_WritePin(&Gpio, TIMER_PIN, 0x1); //Make sure we're starting High, as device triggered on low

#else
    /* Timer Counter */
    int status = XTmrCtr_Initialize(&TimerCounterInst, TMRCTR_DEVICE_ID);
    if (status != XST_SUCCESS){
        th_printf("ERROR: Timer counter initialization failed \r\n");
        //return status;
    }

    XTmrCtr_SetOptions(&TimerCounterInst, TIMER_CNTR_0,
                XTC_AUTO_RELOAD_OPTION |
                XTC_CASCADE_MODE_OPTION);

    th_printf("INFO: Timer counter initialized\r\n");

    /* Calibration */
    start_64b_counter();
    //sleep(1);
    th_timer_start = get_64b_counter_value();
    //th_printf("INFO: Time calibration for one second (%lf sec, %llu clk)\r\n", get_elapsed_time(calibration_time), calibration_time);

#endif
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP_MODE);
    /* Always call the timestamp on initialize so that the open-drain output
       is set to "1" (so that we catch a falling edge) */
    th_timestamp();
}
