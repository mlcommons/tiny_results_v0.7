/*
Copyright (C) EEMBC(R). All Rights Reserved

All EEMBC Benchmark Software are products of EEMBC and are provided under the
terms of the EEMBC Benchmark License Agreements. The EEMBC Benchmark Software
are proprietary intellectual properties of EEMBC and its Members and is
protected under all applicable laws, including all applicable copyright laws.

If you received this EEMBC Benchmark Software without having a currently
effective EEMBC Benchmark License Agreement, you must discontinue use.

Copyright 2020 The MLPerf Authors. All Rights Reserved.
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
#include "api/internally_implemented.h"

#include "xaxidma.h"
#include "xil_cache.h"   /* enable/disable caches etc */
#include "platform.h"
#include "src_mini.h"
#include <string>
#include "unistd.h"  /* usleep */
/**
 * Timing from Xilinx IP Timer Counter.
 */
#include "xtmrctr.h"     /* timer */

#define CMD_CNT (1)
#ifndef CMD_N
#define CMD_N (1)
#endif

/* From internally_implemented.cpp */
extern uint8_t *gp_buff[MAX_DB_INPUT_SIZE];
extern size_t   g_buff_size;
extern size_t   g_buff_pos;


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

//unsigned char src_data[490] = {
//-81, -80, -81, -82, -83, -85, -85, -87, -92, -89, -87, -89, -89, -83, -83, -87, -56, -42, -35, -34, -25, -7, 32, 48, 36, 39, 31, 23, 14, 9, 5, -3, -11, -14, -13, -18, -28, -37, -42, -32, -22, -33, -44, -56, -64, -74, -72, -80, -80, 9, 10, 9, 5, 6, 8, 5, 6, 7, 6, 5, 5, 8, 8, 9, 7, 27, 32, 33, 33, 30, 22, 15, 14, 12, 14, 13, 16, 18, 18, 20, 25, 26, 33, 34, 34, 31, 29, 22, 17, 4, 5, 9, 12, 10, 7, 9, 7, 7, 6, 6, 7, 4, 4, 5, 5, 4, 5, 6, 7, 6, 8, 5, 12, 5, 19, 19, 18, 16, 14, 12, 5, -4, -1, 1, 1, 2, 2, 3, 2, 4, 7, 8, 8, 5, 8, 9, 7, -7, -14, -11, -4, 0, 1, 5, 4, 4, 7, 5, 5, 2, 3, 5, 0, 2, 5, 1, 1, 3, 3, 2, 2, 6, 3, 4, 6, 4, 4, 8, 12, 5, 4, 1, 0, 4, 3, 0, 0, 2, 2, 1, -4, -5, -8, -9, -6, -4, -4, 3, 4, 4, 4, 2, 1, 0, 0, 5, 4, 0, 1, 0, 0, 1, -1, 3, 1, 2, 1, 1, 4, 1, 1, 2, 1, 0, 0, 1, 0, -5, -8, -5, 1, 0, 4, 2, 5, 7, 9, 7, 6, 1, 0, -2, -4, -1, 1, 4, 7, 7, 3, 3, 3, 3, 1, 0, 2, -3, -4, -5, -3, -1, -2, -2, -1, 0, -1, 1, -3, 3, 1, -1, 0, -4, -4, -5, -3, -3, -11, -7, -2, -2, -2, -3, 0, 2, 1, 4, 0, 2, 0, -3, -4, -2, 3, -2, -3, -8, -10, -6, -7, -5, -4, -2, -2, 0, -4, 0, -1, -2, 0, -2, 1, 0, -2, -2, -1, -2, -1, -2, -4, 0, -5, -7, -6, -9, -10, -1, 6, 3, 6, 5, 4, 5, 5, 3, 0, -3, -2, -3, -3, -2, -2, 3, -3, -1, -6, -5, -5, -6, -5, -7, -3, -2, -4, 0, 2, 0, -2, -1, -4, -1, -1, -3, 1, 0, 0, -1, -6, -4, 1, -1, -3, -2, -2, -1, 2, 6, 4, 7, 5, 7, 7, 10, 8, 5, 5, 2, 1, 0, -3, -4, -3, -3, 5, 4, -1, -3, 0, 2, -2, 1, -4, 0, -3, 2, -2, -2, 0, -1, 0, -3, -1, 2, -3, 0, 1, -3, 0, -1, -2, -1, 0, 0, -1, -2, -5, -3, -3, -3, -2, -5, -2, 1, -1, -2, -4, -2, -4, -6, -5, -3, 1, -1, -1, -1, -3, -3, -2, 0, -1, -1, 0, -1, -1, -1, 1, -1, -1, -2, -2, -1, -2, -3, 0, -1, -1, 0, 2, -1, 3, 2, 4, 5, 9, 2, -1, -1, -3, -3, -4, -7, -6, -7, -2, -2, 3, 3, 1, 1, 1, 6, -1, -3, -3, -3, -3, -4, -3, -1, -2, -1,
//};

/* Base address for the AXI DMA */
#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID

/* AXI DMA instance */
XAxiDma axi_dma;
XAxiDma_Config *axi_dma_cfg;

/* Set Heap Size in ldscript.ld to 0x1000000 (16MB) */
uint8_t tx_buffer_ptr[INPUT_N_FEATURES]; //(uint8_t*)malloc(INPUT_N_FEATURES * sizeof(uint8_t));
uint8_t rx_buffer_ptr[OUTPUT_N_FEATURES];//(uint8_t*)malloc(OUTPUT_N_FEATURES  * sizeof(uint8_t));





/* AXI DMA initialization routine */
int init_axi_dma(u16 device_id) {
    int status;

    //xil_printf("INFO: Initializing AXI DMA\r\n");
    axi_dma_cfg = XAxiDma_LookupConfig(device_id);
    if (!axi_dma_cfg) {
        xil_printf("No config found for %d\r\n", device_id);
        return XST_FAILURE;
    }

    status = XAxiDma_CfgInitialize(&axi_dma, axi_dma_cfg);
    if (status != XST_SUCCESS) {
        xil_printf("Initialization failed %d\r\n", status);
        return XST_FAILURE;
    }

    if(XAxiDma_HasSg(&axi_dma)){
        xil_printf("Device configured as SG mode \r\n");
        return XST_FAILURE;
    }

    /* Disable interrupts, we use polling mode */
    XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    return XST_SUCCESS;
}

// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
	//init_platform(); DONT CALL THIS! Overwrites/flushes a portion of memory that contains stack/heap, and overwrites the counters for number of inferences/warmups when running a benchmark

	Xil_DCacheFlushRange((UINTPTR)(gp_buff), INPUT_N_FEATURES * sizeof(uint8_t));
	Xil_DCacheFlushRange((UINTPTR)(tx_buffer_ptr), INPUT_N_FEATURES * sizeof(uint8_t));
	Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES * sizeof(uint8_t));

    /* Load data in the TX/RX buffers */
    for (int i = 0; i < INPUT_N_FEATURES * CMD_N; i++) {
        tx_buffer_ptr[i] = *gp_buff[i];//sample_data[i];//
        //th_printf("%i\r\n",i);
    }
    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_N; i++) {
        rx_buffer_ptr[i] = 0x0;
        //th_printf("%i\r\n",i);
    }
/*
    th_printf("LOADED DATA: \r\n");
    for (int i = 0; i < INPUT_N_FEATURES; i++) {
        unsigned char b = *gp_buff[i];//tx_buffer_ptr[i];
        if (i%25 == 0 && i != 0){
            th_printf("%02X\r\n",b);
        }
        else{
            th_printf("%02X ",b);
        }
    }
    th_printf("\r\n");
    th_printf("REFERENCE DATA:\r\n");
    for (int i = 0; i < INPUT_N_FEATURES; i++) {
        unsigned char b = src_data[i];
        if (i%25 == 0 && i != 0){
            th_printf("%02X\r\n",b);
        }
        else{
            th_printf("%02X ",b);
        }
    }
    th_printf("\r\n");

    //malloc_stats();
*/
}

// Add to this method to return real inference results.
void th_results(void)
{
    float *results  = NULL;
    size_t nresults = 0;
    /* USER CODE 1 BEGIN */
    float res_arr[10] = {0,0,0,0,0,0,0,0,0,0};
    nresults = 10;
    results = res_arr;
    int maxclass = rx_buffer_ptr[0];

    /*
    for (int i = 0; i < 12 * CMD_N; i++) {
        unsigned char b = rx_buffer_ptr[i];
        if (i%25 == 0 && i != 0){
            th_printf("%02X\r\n",b);
        }
        else{
            th_printf("%02X ",b);
        }
    }
    th_printf("\r\n");
	*/

    res_arr[maxclass] = 1;
    /* USER CODE 1 END */
    /**
     * The results need to be printed back in exactly this forth_printf("m-results-[%0.3f]\r\n", result);mat; if easier
     * to just modify this loop than copy to results[] above, do that.
     */
    th_printf("m-results-[");
        for (size_t i = 0; i < nresults; i++)
        {
            /* N.B. Be sure %f is enabled in SDK */
            th_printf("%i.000", (int)results[i]);
            if (i < (nresults - 1))
            {
                th_printf(",");
            }
        }
        th_printf("]\r\n");
}

void th_infer() {
	//Xil_DCacheFlushRange((UINTPTR)(tx_buffer_ptr), INPUT_N_FEATURES * sizeof(uint8_t));
	//Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES * sizeof(uint8_t));


	//int status = XAxiDma_SimpleTransfer(&axi_dma, (UINTPTR)(tx_buffer_ptr), INPUT_N_FEATURES, XAXIDMA_DMA_TO_DEVICE);
	int status = XAxiDma_SimpleTransfer(&axi_dma, (UINTPTR)(gp_buff), INPUT_N_FEATURES, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		xil_printf("ERROR: DMA_TO_DEVICE failed, status %u\r\n", status);
	}
	status = XAxiDma_SimpleTransfer(&axi_dma, (UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
		xil_printf("ERROR: DEVICE_TO_DMA failed, status %u\r\n", status);
	}

	while (XAxiDma_Busy(&axi_dma, XAXIDMA_DEVICE_TO_DMA)) {
		; /* Wait */
	}
	while (XAxiDma_Busy(&axi_dma, XAXIDMA_DMA_TO_DEVICE)) {
		; /* Wait */
	}
	//Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(data_t));
	Xil_DCacheInvalidateRange((UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES * sizeof(uint8_t));
}

/// \brief optional API.
void th_final_initialize(void) {
	//init_accelerators();

	init_axi_dma(DMA_DEV_ID);
	//floatsize = sizeof(float);
	//src_mem_size = (INPUT_N_FEATURES* sizeof(unsigned char));
	//dst_mem_size = (OUTPUT_N_FEATURES* sizeof(unsigned char));

	//malloc_stats();
    /* Set Heap Size in ldscript.ld to 0x1000000 (16MB) */

	//printf("src: %p, dst %p \n",src_mem,dst_mem);
	//th_printf("INFO: Init Finished!\r\n");
	//print out this info on startup so the power init actually works
	th_printf(EE_MSG_NAME, EE_DEVICE_NAME, TH_VENDOR_NAME_STRING);
    th_printf("m-profile-[%s]\r\n", EE_FW_VERSION);
    th_printf("m-model-[%s]\r\n", TH_MODEL_VERSION);
}

void th_pre() {};
void th_post() {};

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
	//FW Needs to be recompiled for lower speed?
#else
/* In performance mode, we talk directly to the DUT at 115200 baud */
	//init_uart();
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
	gpio_status = XGpio_Initialize(&Gpio, GPIO_PMOD_PIN_DEVICE_ID);
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
