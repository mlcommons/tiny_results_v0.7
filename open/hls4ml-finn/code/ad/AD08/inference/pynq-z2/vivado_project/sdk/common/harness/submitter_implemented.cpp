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

#if EE_CFG_ENERGY_MODE == 1
//GPIO Config for Timestamp
XGpioPs Gpio;
XGpioPs_Config *GPIOConfigPtr;
int GPIO_Status;
#endif

//#define UART_DEVICE_ID  XPAR_XUARTPS_0_DEVICE_ID
XUartPs Uart_Ps;		/* The instance of the UART Driver */


extern uint8_t  *gp_buff[MAX_DB_INPUT_SIZE];
extern size_t   g_buff_size;
extern size_t   g_buff_pos;


XMyproject_axi do_anomaly_detector;
XMyproject_axi_Config *do_anomaly_detector_cfg;

float src_mem[src_FEATURE_COUNT]; //[src_FEATURE_COUNT*sizeof(float)];//[128];
float dst_mem[dst_FEATURE_COUNT]; //[dst_FEATURE_COUNT*sizeof(float)];//[128];
unsigned char *src_mem_pos;
unsigned char *dst_mem_pos;
uint32_t src_mem_size;
uint32_t dst_mem_size;

uint8_t  floatsize = sizeof(float);

XTime th_timer_start;
XTime th_timer_timestamp;

double th_calibration_time;
double th_timestamp_counts;

uint8_t slices;
uint8_t bins;


double get_elapsed_time(XTime start, XTime stop)
{
    return 1.0 * (stop - start) / (COUNTS_PER_SECOND);
}

/* accelerator initialization routine */
void init_accelerators()
{
    //printf("INFO: Initializing accelerator\n\r");
    do_anomaly_detector_cfg = XMyproject_axi_LookupConfig(XPAR_MYPROJECT_AXI_0_DEVICE_ID);//XPAR_ANOMALY_DETECTOR_AXI_DEVICE_ID);
    if (do_anomaly_detector_cfg)
    {
        int status  = XMyproject_axi_CfgInitialize(&do_anomaly_detector, do_anomaly_detector_cfg);
        if (status != XST_SUCCESS)
        {
            //printf("ERROR: Initializing accelerator\n\r");
        }
    }
}

float mse(float a[], float b[], int size) {
	float error = 0;
	for (int i = 0; i < size; i++) {
		error += pow((b[i] - a[i]), 2);
		//th_printf("i: %i, b: %.3f a: %.3f, diff: %.3f\r\n",i,b[i],a[i],(b[i] - a[i]));
	}
	return error / size;
}

// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {

    //init_platform();
    //init_accelerators();

    Xil_DCacheFlushRange((UINTPTR)src_mem, src_FEATURE_COUNT * sizeof(float));
    Xil_DCacheFlushRange((UINTPTR)dst_mem, src_FEATURE_COUNT * sizeof(float));


    float data_flt = 0.0;
	int skip = 4; // take only every nth bin within a time slice, where n = skip-1
	int cutoff = 640; //128 * 2 frames
	slices = 2; // 4 or 2, depending on AD05 vs AD06
	bins = 32;
	float flt_buffer[cutoff*floatsize] = {0}; // load 4/5 total frames (128 Features [32b floats] x 2/5 Frames = 2048b vs 2560b) Only pulling from first 2 frames (can modify?)
	for (int i = 0; i < cutoff*floatsize; i+=floatsize){
		memcpy(&data_flt,&gp_buff[i],(int)floatsize); //TODO
		flt_buffer[i] = data_flt; //data_flt;// /(int)floatsize
		//printf("Loaded %.3f into float buffer at pos %i, i=%i\r\n",data_flt,(i),i);
	}
	for (int i=0; i < slices; i++){
	  //printf("i %i \r\n",i);
	  for (int j=0; j < bins*skip; j+=skip){
		  //printf("j %i \r\n",j);
		  src_mem[((i*(bins)+(j/skip)))] = flt_buffer[((i*bins*skip)+j)];
		  //printf("Destination pos %i, src pos %i\r\n",((i*(bins)+(j/skip))),((i*bins*skip)+j));
		  //used_floats[((i*(bins/skip)+(j/skip)))]=flt_buffer[(i*bins+j)];
	  }
	}

	//for (int i = 0; i < src_mem_size; i++) {//Init DST mem with 0's
	//    src_mem[i] = 0;
   // }
	for (int i = 0; i < dst_mem_size; i++) {//Init DST mem with 0's
		dst_mem[i] = 0;
	}
    //malloc_stats();
    return;

}

// Add to this method to return real inference results.
void th_results() {
	    //float *results  = NULL;
	    //size_t nresults = 0;
	    /* USER CODE 1 BEGIN */
	    float result = 0;
	    Xil_DCacheFlushRange((UINTPTR)dst_mem, src_FEATURE_COUNT * sizeof(float));
	    /* Populate results[] and n from the fp32 prediction tensor. */
	    //for(size_t i = 0; i < src_FEATURE_COUNT; i++){ //find the error score of each feature, then average over all features
	    	//printf("INFO: Iteration %i\r\n",(i/floatsize));
	    	//printf("INFO: FLT  %.3f ",(used_floats[i]));
	    	//printf("INFO: SRC Mem:  %i",((signed char)src_mem[i]));
	    	//printf(" DST Mem:  %i\r\n",((signed char)dst_mem[i]));
	    	//printf(" DST Data:  %i\r\n",((signed char)dst_data[i]));
	    	//error += ((signed char)src_mem[i]-(signed char)dst_mem[i])*((signed char)src_mem[i]-(signed char)dst_mem[i]);
	    	//int diff = (signed char)src_mem[i]-(signed char)dst_mem[i];
	    	//error += powf((float)diff,2);
	    //}
	    /*
	    printf("[");
	    for(size_t i = 0; i < src_FEATURE_COUNT; i++){
	    	printf("%i, ",((signed char)src_mem[i]));
	    }
	    printf("]/r/n");
	    */
	    float mse_val = mse(dst_mem,src_mem,src_FEATURE_COUNT);//error/128.0;

	    //printf("INFO: Anomaly Score (MSE): %.3f\r\n",mse_val);
	    result = mse_val;

	    /* USER CODE 1 END */
	    /**
	     * The results need to be printed back in exactly this forth_printf("m-results-[%0.3f]\r\n", result);mat; if easier
	     * to just modify this loop than copy to results[] above, do that.
	     */
	    th_printf("m-results-[%0.3f]\r\n", result);
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() {
	/*
	//start accelerator

    //unsigned char *src_mem_i = src_mem;
    //unsigned char *dst_mem_i = dst_mem;
	//printf("INFO: SRC @%p\r\n", src_mem_i);
	//printf("INFO: DST @%p\r\n", dst_mem_i);
    Xil_DCacheFlushRange((UINTPTR)dst_mem, 128 * sizeof(unsigned char));

    XAnomaly_detector_axi_Set_in_V(&do_anomaly_detector, (unsigned)src_mem);
    XAnomaly_detector_axi_Set_out_V(&do_anomaly_detector, (unsigned)dst_mem);
    XAnomaly_detector_axi_Set_batch(&do_anomaly_detector, 1);

    XAnomaly_detector_axi_Start(&do_anomaly_detector);
    // polling
	while (!XAnomaly_detector_axi_IsDone(&do_anomaly_detector));
    Xil_DCacheFlushRange((UINTPTR)dst_mem, 128 * sizeof(unsigned char));
    */

	//printf("INFO: %s\n\r", __func__);


	//start accelerator
    //init_platform();
    //init_accelerators();


    Xil_DCacheFlushRange((UINTPTR)src_mem, src_FEATURE_COUNT * sizeof(float));
    Xil_DCacheFlushRange((UINTPTR)dst_mem, src_FEATURE_COUNT * sizeof(float));
    float *src_mem_i = src_mem; //Todo potential change? dont think so though
    float *dst_mem_i = dst_mem;

    XMyproject_axi_Set_in_r(&do_anomaly_detector, (unsigned)src_mem_i);
    XMyproject_axi_Set_out_r(&do_anomaly_detector, (unsigned)dst_mem_i);

    XMyproject_axi_Start(&do_anomaly_detector);
    /* polling */
    while (!XMyproject_axi_IsDone(&do_anomaly_detector));

    Xil_DCacheFlushRange((UINTPTR)dst_mem, src_FEATURE_COUNT * sizeof(float));

}

/// \brief optional API.
void th_final_initialize(void) {
	init_platform();
	init_accelerators();

	//floatsize = sizeof(float);
	src_mem_size = (src_FEATURE_COUNT* floatsize);//floatsize
	dst_mem_size = (dst_FEATURE_COUNT* floatsize);//floatsize

	//malloc_stats();
	//src_mem = malloc(src_mem_size);
	//dst_mem = malloc(dst_mem_size);
	//printf("src: %p, dst %p \n",src_mem,dst_mem);
	//th_printf("INFO: Init Finished!\r\n");
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
	u16 DeviceId = UART_DEVICE_ID;
	int UartStatus;
#if EE_CFG_ENERGY_MODE == 1
/* In energy mode, we talk to the DUT through the IO Manager at 9600 baud */
	//FW Needs to be recompiled for lower speed? #TODO
	XUartPs_Config *Config;

		/*
		 * Initialize the UART driver so that it's ready to use
		 * Look up the configuration in the config table and then initialize it.
		 */
		Config = XUartPs_LookupConfig(DeviceId);
		if (NULL == Config) {
		}

		UartStatus = XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress);
		if (UartStatus != XST_SUCCESS) {
		}

		XUartPs_SetBaudRate(&Uart_Ps, 9600);
		//printf("Info: Baudrate 9600b");
#else
/* In performance mode, we talk directly to the DUT at 115200 baud */
		XUartPs_Config *Config;

			/*
			 * Initialize the UART driver so that it's ready to use
			 * Look up the configuration in the config table and then initialize it.
			 */
			Config = XUartPs_LookupConfig(DeviceId);
			if (NULL == Config) {
			}

			UartStatus = XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress);
			if (UartStatus != XST_SUCCESS) {
			}

			XUartPs_SetBaudRate(&Uart_Ps, 115200);
			//printf("Info: Baudrate 115200");
#endif
}

void th_timestamp(void) {
#if EE_CFG_ENERGY_MODE == 1
/* USER CODE 1 BEGIN */
/* Step 1. Pull pin low */
	XGpioPs_WritePin(&Gpio, TIMER_PIN, 0x0);
/* Step 2. Hold low for at least 1us */
	usleep(10); //Holding for 10us, unsure of max speed of GPIOs, should be fine?
	//continue; //Not worrying about for performance mode
/* Step 3. Release driver */
	XGpioPs_WritePin(&Gpio, TIMER_PIN, 0x1);
/* USER CODE 1 END */
#else
    unsigned long microSeconds = 0ul;
    /* USER CODE 2 BEGIN */
	XTime_GetTime(&th_timer_timestamp);
	th_timestamp_counts = get_elapsed_time(th_timer_start, th_timer_timestamp);
	microSeconds = th_timestamp_counts/th_calibration_time;
    /* USER CODE 2 END */
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP, microSeconds);
#endif
}

void th_timestamp_initialize(void) {
    /* USER CODE 1 BEGIN */
	//GPIO Initilization
#if EE_CFG_ENERGY_MODE == 1
	GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	GPIO_Status = XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr, GPIOConfigPtr ->BaseAddr);

	if (GPIO_Status != XST_SUCCESS) {
	}

	XGpioPs_SetDirectionPin(&Gpio, TIMER_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, TIMER_PIN, 1);
	XGpioPs_WritePin(&Gpio, TIMER_PIN, 0x1); //Make sure we're starting High, as device triggered on low

#else
	XTime th_timer_stop;
	XTime_GetTime(&th_timer_start);
	usleep(1000); //sleep for 1000us to calibrate timer
	XTime_GetTime(&th_timer_stop);
	th_calibration_time = get_elapsed_time(th_timer_start, th_timer_stop)/1000;
    /* USER CODE 1 END */
#endif
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP_MODE);
    /* Always call the timestamp on initialize so that the open-drain output
       is set to "1" (so that we catch a falling edge) */
    th_timestamp();
}
