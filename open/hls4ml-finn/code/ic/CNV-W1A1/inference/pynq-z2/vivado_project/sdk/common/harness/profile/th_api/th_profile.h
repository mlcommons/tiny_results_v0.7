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


#ifndef __TH_PROFILE_H
#define __TH_PROFILE_H

#include "../ee_profile.h"
#include "th_util.h"
#include "assert.h"
#include "xil_cache.h"   /* enable/disable caches etc */

//#include <math.h>
/* PORTME:
 * Select an EE_MODEL_VERSION_* from ee_profile.h, so that the Host UI
 * knows which model is inside this DUT!
 * */
#define TH_MODEL_VERSION EE_MODEL_VERSION_IC01


#define CMD_CNT 1
#ifndef CMD_N
#define CMD_N 1
#endif
#define INPUT_N_FEATURES (3072)
#define OUTPUT_N_FEATURES (1)

#define __BATCH_MODE__

/* Copied these macro definitions from the IODMA IPs */
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_AP_CTRL      0x00
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_GIE          0x04
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_IER          0x08
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_ISR          0x0c
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_IN0_V_DATA   0x10
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_BITS_IN0_V_DATA   64
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_NUMREPS_DATA 0x1c
#define XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_BITS_NUMREPS_DATA 32

#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_AP_CTRL      0x00
#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_GIE          0x04
#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_IER          0x08
#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_ISR          0x0c
#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_OUT_V_DATA   0x10
#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_BITS_OUT_V_DATA   64
#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_NUMREPS_DATA 0x1c
#define XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_BITS_NUMREPS_DATA 32


#define WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

#define IDMA0_AP_CTRL_REG ((XPAR_IDMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_AP_CTRL))
#define IDMA0_IN0_REG ((XPAR_IDMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_IN0_V_DATA))
#define IDMA0_NUMREPS_REG ((XPAR_IDMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_NUMREPS_DATA))


#define ODMA0_AP_CTRL_REG ((XPAR_ODMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_AP_CTRL))
#define ODMA0_OUT_REG ((XPAR_ODMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_OUT_V_DATA))
#define ODMA0_NUMREPS_REG ((XPAR_ODMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_NUMREPS_DATA))

/*#define __DEBUG__*/
#define MAX_PRINT_ELEMENTS 16

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

#define SHORT_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define SHORT_TO_BINARY(byte)  \
    (byte & 0x8000 ? '1' : '0'), \
    (byte & 0x4000 ? '1' : '0'), \
    (byte & 0x2000 ? '1' : '0'), \
    (byte & 0x1000 ? '1' : '0'), \
    (byte & 0x800 ? '1' : '0'), \
    (byte & 0x400 ? '1' : '0'), \
    (byte & 0x200 ? '1' : '0'), \
    (byte & 0x100 ? '1' : '0'), \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')


#define DDR_BASE_ADDR XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define MEM_BASE_ADDR (DDR_BASE_ADDR + 0x1000000)

/* Data offsets and pointers */
#define TX_BUFFER_BASE (MEM_BASE_ADDR + 0x01000000)
#define RX_BUFFER_BASE (MEM_BASE_ADDR + 0x03000000)
#define RX_BUFFER_GLD_BASE (MEM_BASE_ADDR + 0x05000000)

static uint8_t *tx_buffer_ptr = (uint8_t*)TX_BUFFER_BASE;
static uint8_t *rx_buffer_ptr = (uint8_t*)RX_BUFFER_BASE;
//static u8 *rx_buffer_gld_ptr = (u8*)RX_BUFFER_GLD_BASE;


unsigned char *src_mem_pos;
unsigned char *dst_mem_pos;
uint8_t slices;
uint8_t bins;
unsigned int accel_cnt;
#endif
