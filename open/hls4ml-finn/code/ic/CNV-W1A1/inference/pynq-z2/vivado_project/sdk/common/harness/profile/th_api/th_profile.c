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
//"profile/th_api/th_profile.h"
#include "th_profile.h"
//#include "../../src.h"
/* From profile/ee_buffer.c */
extern uint8_t *gp_buff;
extern size_t   g_buff_size;
extern size_t   g_buff_pos;


/* from ../../../main.c */
extern uint32_t src_mem_size;
extern uint32_t dst_mem_size;

/**
 * Copy the octet buffer `gp_buff` into your input tensor in the proper
 * format.
 */

void Start(UINTPTR BaseAddress, UINTPTR AP_CTRL) {
    u32 Data;
    Data = ReadReg(BaseAddress, AP_CTRL) & 0x80;
    WriteReg(BaseAddress, AP_CTRL, Data | 0x01);
}

u32 IsDone(UINTPTR BaseAddress, UINTPTR AP_CTRL) {
    u32 Data;
    Data = ReadReg(BaseAddress, AP_CTRL);
    return (Data >> 1) & 0x1;
}

void
th_load_tensor(void)
{
    init_platform();
    //init_accelerators();

    Xil_DCacheFlushRange((UINTPTR)tx_buffer_ptr, CMD_N * INPUT_N_FEATURES * sizeof(uint8_t));
    Xil_DCacheFlushRange((UINTPTR)rx_buffer_ptr, CMD_N * OUTPUT_N_FEATURES * sizeof(uint8_t));

    /* Load data in the TX/RX buffers */
    for (int i = 0; i < INPUT_N_FEATURES * CMD_N; i++) {
        tx_buffer_ptr[i] = gp_buff[i];//sample_data[i];//
        //th_printf("%i\r\n",i);
    }
    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_N; i++) {
        rx_buffer_ptr[i] = 0x0;
        //th_printf("%i\r\n",i);
    }
    /*
    for (int i = 0; i < INPUT_N_FEATURES * CMD_N; i++) {
        unsigned char b = tx_buffer_ptr[i];
        if (i%25 == 0 && i != 0){
            th_printf("%X\r\n",b);
        }
        else{
            th_printf("%X ",b);
        }
    }
    th_printf("\r\n");

    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_N; i++) {
        unsigned char b = rx_buffer_ptr[i];
        if (i%25 == 0 && i != 0){
            th_printf("%X\r\n",b);
        }
        else{
            th_printf("%X ",b);
        }
    }
    th_printf("\r\n");
	*/
    /* Make sure the data movers are idle */
    assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
    assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");

    /* Program data movers with the address of the TX and RX buffers and batch size */
    Xil_Out32(IDMA0_IN0_REG, (UINTPTR)tx_buffer_ptr);
    Xil_Out32(IDMA0_NUMREPS_REG, CMD_N);
    Xil_Out32(ODMA0_OUT_REG, (UINTPTR)rx_buffer_ptr);
    Xil_Out32(ODMA0_NUMREPS_REG, CMD_N);
}

/**
 * Perform a single inference.
 */
void
th_infer(void)
{
	//init_platform();
	//init_accelerators();
    Xil_DCacheFlushRange((UINTPTR)tx_buffer_ptr, CMD_N * INPUT_N_FEATURES * sizeof(uint8_t));
    Xil_DCacheFlushRange((UINTPTR)rx_buffer_ptr, CMD_N * OUTPUT_N_FEATURES * sizeof(uint8_t));
    /* Start */
    Start(XPAR_IDMA0_BASEADDR, XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_AP_CTRL);
    Start(XPAR_ODMA0_BASEADDR, XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_AP_CTRL);

    /* Wait for completion */

    while (!IsDone(XPAR_IDMA0_BASEADDR, XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_AP_CTRL)) {
           ; /* Wait */
    }
    while (!IsDone(XPAR_ODMA0_BASEADDR, XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_AP_CTRL)) {
           ; /* Wait */
    }
    Xil_DCacheInvalidateRange((UINTPTR)rx_buffer_ptr, CMD_N * OUTPUT_N_FEATURES * sizeof(u8));

    assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
    assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");
}

void
th_results(void)
{
    float *results  = NULL;
    size_t nresults = 0;
    /* USER CODE 1 BEGIN */
    float res_arr[10] = {0,0,0,0,0,0,0,0,0,0};
    nresults = 10;
    results = res_arr;
    int maxclass = rx_buffer_ptr[0];
    //th_printf("Max Class: %i\r\n", maxclass);
    /*
    for (int i = 0; i < 12 * CMD_N; i++) {
        unsigned char b = rx_buffer_ptr[i];
        if (i%25 == 0 && i != 0){
            th_printf("%X\r\n",b);
        }
        else{
            th_printf("%X ",b);
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
            th_printf("%.3f", (float)results[i]);
            if (i < (nresults - 1))
            {
                th_printf(",");
            }
        }
        th_printf("]\r\n");
}
