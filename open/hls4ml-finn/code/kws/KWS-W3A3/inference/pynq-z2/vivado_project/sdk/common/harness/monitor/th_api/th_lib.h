/**
 * Copyright (C) EEMBC(R). All Rights Reserved
 *
 * All EEMBC Benchmark Software are products of EEMBC and are provided under the
 * terms of the EEMBC Benchmark License Agreements. The EEMBC Benchmark Software
 * are proprietary intellectual properties of EEMBC and its Members and is
 * protected under all applicable laws, including all applicable copyright laws.
 *
 * If you received this EEMBC Benchmark Software without having a currently
 * effective EEMBC Benchmark License Agreement, you must discontinue use.
 */

#ifndef __TH_LIB_H
#define __TH_LIB_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "th_libc.h"
#include "../ee_main.h"

//#include "../../src.h"
//#include "../../dst.h"
#include "xtime_l.h" // Xilinx Timer Library
//#include "xanomaly_detector_axi.h"  /* accelerator */

#include "xuartps.h"  //Higher level UART library for baud config
#define UART_DEVICE_ID                  XPAR_XUARTPS_0_DEVICE_ID
XUartPs Uart_Ps;		/* The instance of the UART Driver */


#include "xgpiops.h" // GPIO PS Control Library
XGpioPs Gpio;
XGpioPs_Config *GPIOConfigPtr;
int GPIO_Status;
#define TIMER_PIN 12 //Arduino RST, Labeled "RST" on board between "IOREF" and "3.3V" on arduino shield connectors


#define src_FEATURE_COUNT 128
#define dst_FEATURE_COUNT 128

#if 0
/* base address for the accelerator */
#define MEM_BASE_ADDR XPAR_PS7_DDR_0_S_AXI_BASEADDR

/* data offsets and pointers */
#define SRC_BUFFER_BASE (MEM_BASE_ADDR + 0x00000000)
//unsigned char *src_mem = (unsigned char*)SRC_BUFFER_BASE;

#define DST_BUFFER_BASE (MEM_BASE_ADDR + 0x00020000)
//unsigned char *dst_mem = (unsigned char*)DST_BUFFER_BASE;
#else
unsigned char *src_mem;//[128];
unsigned char *dst_mem;//[128];
#endif

// It is crucial to follow EEMBC message syntax for key messages
#define EE_MSG_TIMESTAMP "m-lap-us-%lu\r\n"

#ifndef EE_CFG_ENERGY_MODE
#define EE_CFG_ENERGY_MODE 1
#endif

#if EE_CFG_ENERGY_MODE == 1
#define EE_MSG_TIMESTAMP_MODE "m-timestamp-mode-energy\r\n"
#else
#define EE_MSG_TIMESTAMP_MODE "m-timestamp-mode-performance\r\n"
#endif

/**
 * This string is used in the "name%" command. When the host UI starts a test,
 * it calles the "name%" command, and the result is captured in the log file.
 * It can be quite useful to have the device's name in the log file for future
 * reference or debug.
 */
#define TH_VENDOR_NAME_STRING "Xilinx_PynqZ2_Z7020"

XTime th_timer_start;
XTime th_timer_timestamp;
double th_calibration_time;
double th_timestamp_counts;

uint8_t  floatsize;
uint32_t src_mem_size;
uint32_t dst_mem_size;

void th_final_initialize(void);
void th_timestamp_initialize(void);
void th_timestamp(void);
void th_serialport_initialize(void);
void th_printf(const char *fmt, ...);
void th_command_ready(char volatile *);
/* accelerator initialization routine */
void init_accelerators();
double get_elapsed_time(XTime start, XTime stop);

#endif // __TH_LIB_H
