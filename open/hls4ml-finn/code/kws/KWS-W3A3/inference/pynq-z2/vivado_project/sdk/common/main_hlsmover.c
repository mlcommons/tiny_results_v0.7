#include "xparameters.h"
#include "unistd.h"      /* sleep */
#include "xil_printf.h"  /* UART debug print functions */
#include "xil_cache.h"
#include "stdio.h"       /* printf (w/ float) support */
#include "stdlib.h"      /* malloc */
#include "platform.h"    /* platform init/cleanup functions */
#include "xtime_l.h"     /* to measure performance of the system */
#include "xil_io.h"
#include "assert.h"

#include "src.h"
#include "dst.h"

#ifdef __USE_OCM__
#define ITER_N 505100
#define CMD_CNT (1)
#ifndef CMD_N
#define CMD_N (1)
#endif
#else
#define ITER_N 50
#define CMD_CNT (10102)
#ifndef CMD_N
#define CMD_N (10102)
#endif
#endif
#define INPUT_N_FEATURES (490)
#define OUTPUT_N_FEATURES (1)

//#define __BATCH_MODE__

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

#define IDMA0_AP_CTRL_REG ((XPAR_IDMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_AP_CTRL))
#define IDMA0_IN0_REG ((XPAR_IDMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_IN0_V_DATA))
#define IDMA0_NUMREPS_REG ((XPAR_IDMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_0_IODMA_0_STREAMINGDATAFLOWPARTITION_0_IODMA_0_CONTROL_ADDR_NUMREPS_DATA))

#define ODMA0_AP_CTRL_REG ((XPAR_ODMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_AP_CTRL))
#define ODMA0_OUT_REG ((XPAR_ODMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_OUT_V_DATA))
#define ODMA0_NUMREPS_REG ((XPAR_ODMA0_BASEADDR) + (XSTREAMINGDATAFLOWPARTITION_2_IODMA_0_STREAMINGDATAFLOWPARTITION_2_IODMA_0_CONTROL_ADDR_NUMREPS_DATA))

//#define __DEBUG__
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

/* Golden model of the accelerator in software */
int finn_cnv_w1a1_sw(u8 *rx, u8 *tx, unsigned img_n, unsigned input_n_features, unsigned output_n_features) {
    xil_printf("INFO: Golden results are pre-compiled\r\n");
    xil_printf("INFO: It would be nice to run a software model\r\n");
    // See src.h and dst.h for input and golden output respectively.
    for (u32 i = 0; i < (img_n * output_n_features); i++) {
        rx[i] = dst_data[i];
    }
    return 0;
}

/* Profiling utility */
double get_elapsed_time(XTime start, XTime stop) {
    return 1.0 * (stop - start) / (COUNTS_PER_SECOND);
}

/* Dump data to the console */
void dump_data(const char* label, u8* data, unsigned sample_count, unsigned feature_count, unsigned print_hex, unsigned print_bin) {
    xil_printf("INFO:   %s[%u][%u]:\n\r", label, sample_count, feature_count);
    /* print at most MAX_PRINT_ELEMENTS */
    for (unsigned i = 0; i < sample_count && i < MAX_PRINT_ELEMENTS; i++)
    {
        xil_printf("INFO:     [%u] ", i);
        if (print_hex)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf("%02X ", data[index]);
            }
        if (print_bin)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf(""SHORT_TO_BINARY_PATTERN, SHORT_TO_BINARY(data[index]));
                xil_printf(" ");
            }
        xil_printf("\n\r");
    }
}

/* The top of the hill :-) */
int main(int argc, char** argv) {

    //int status;
    XTime start, stop;
    double calibration_time;
    double sw_elapsed;
    char __attribute__ ((unused)) dummy; /* dummy input */

    int hw_errors;
    double hw_elapsed = 0;
    double cache_elapsed = 0;

    /* Initialize platform (uart and caches) */
    init_platform();

    xil_printf("\n\r");
    xil_printf("INFO: =========================================================================\n\r");
#ifdef __BATCH_MODE__
    xil_printf("INFO: FINN KWS-W3A3 (w/ polling, HLS data mover, batch = %u)\n\r", CMD_N);
#else
    xil_printf("INFO: FINN KWS-W3A3 (w/ polling, HLS data mover, batch = 1)\n\r");
#endif
    xil_printf("INFO: =========================================================================\n\r");

    /* Calibration */
    XTime_GetTime(&start);
    sleep(1);
    XTime_GetTime(&stop);
    calibration_time = get_elapsed_time(start, stop);
    printf("INFO: Time calibration for one second (%lf sec)\n\r", calibration_time);

    /* Initialize memory */
    xil_printf("INFO: Initialize memory\n\r");
    xil_printf("INFO:   - Total inputs: %u\r\n", CMD_CNT);
    xil_printf("INFO:   - Batch size: %u\r\n", CMD_N);
    xil_printf("INFO:   - Iterations: %u\r\n", ITER_N);
    xil_printf("INFO:   - Input features: %u\r\n", INPUT_N_FEATURES);
    xil_printf("INFO:   - Output features: %u\r\n", OUTPUT_N_FEATURES);
    xil_printf("INFO:   - Single feature data size: %u B\r\n", sizeof(u8));
    printf("INFO:   - Total input data size: %uB, %.2fKB\r\n", INPUT_N_FEATURES * CMD_CNT * sizeof(u8), (INPUT_N_FEATURES * CMD_CNT * sizeof(u8))/1024.0);
    printf("INFO:   - Total output data size: %uB, %.2fKB\r\n", OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8), (OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8))/1024.0);

    /* Set Heap Size in ldscript.ld to 0x1000000 (16MB) */
    u8 *tx_buffer_ptr = (u8*)malloc(INPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    u8 *rx_buffer_ptr = (u8*)malloc(OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    u8 *rx_buffer_gld_ptr = (u8*)malloc(OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    /* malloc_stats(); */

    /* Load data in the TX/RX buffers */
    for (u32 i = 0; i < INPUT_N_FEATURES * CMD_N; i++) {
        tx_buffer_ptr[i] = src_data[i];
    }
    for (u32 i = 0; i < OUTPUT_N_FEATURES * CMD_N; i++) {
        rx_buffer_gld_ptr[i] = 0x0;
        rx_buffer_ptr[i] = 0x0;
    }

    /* ****** SOFTWARE REFERENCE ****** */
    xil_printf("INFO: Run SW reference model\r\n");
    XTime_GetTime(&start);
    finn_cnv_w1a1_sw(rx_buffer_gld_ptr, tx_buffer_ptr, CMD_N, INPUT_N_FEATURES, OUTPUT_N_FEATURES);
    XTime_GetTime(&stop);
    sw_elapsed = get_elapsed_time(start, stop);

    /* ****** ACCELERATOR ****** */
    xil_printf("INFO: Press any key to start the HW accelerator: ");
    dummy = inbyte();
    xil_printf("\n\rINFO: \n\r");

#ifdef __BATCH_MODE__

    for (u32 b = 0; b < ITER_N; b++) {
    XTime_GetTime(&start);
    Xil_DCacheFlushRange((UINTPTR)tx_buffer_ptr, CMD_N * INPUT_N_FEATURES * sizeof(u8));
    Xil_DCacheFlushRange((UINTPTR)rx_buffer_ptr, CMD_N * OUTPUT_N_FEATURES * sizeof(u8));
    XTime_GetTime(&stop);
    cache_elapsed += get_elapsed_time(start, stop);

    /* Make sure the data movers are idle */
    assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
    assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");

    /* Program data movers with the address of the TX and RX buffers and batch size */
    Xil_Out32(IDMA0_IN0_REG, (UINTPTR)tx_buffer_ptr);
    Xil_Out32(IDMA0_NUMREPS_REG, CMD_N);
    Xil_Out32(ODMA0_OUT_REG, (UINTPTR)rx_buffer_ptr);
    Xil_Out32(ODMA0_NUMREPS_REG, CMD_N);

    XTime_GetTime(&start);

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

    XTime_GetTime(&stop);
    hw_elapsed += get_elapsed_time(start, stop);

    XTime_GetTime(&start);
    /* Xil_DCacheFlushRange((UINTPTR)rx_buffer_ptr, OUTPUT_N_FEATURES * sizeof(u8)); */
    Xil_DCacheInvalidateRange((UINTPTR)rx_buffer_ptr, CMD_N * OUTPUT_N_FEATURES * sizeof(u8));
    XTime_GetTime(&stop);
    cache_elapsed += get_elapsed_time(start, stop);

    assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
    assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");
    }

#else /* __BATCH_MODE__ */
    for (u32 b = 0; b < ITER_N; b++)
    for (u32 idx = 0; idx < CMD_N; idx++) {

        XTime_GetTime(&start);
        Xil_DCacheFlushRange((UINTPTR)(tx_buffer_ptr + idx * INPUT_N_FEATURES), INPUT_N_FEATURES * sizeof(u8));
        Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(u8));
        XTime_GetTime(&stop);
        cache_elapsed += get_elapsed_time(start, stop);

        /* Make sure the data movers are idle */
        assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
        assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");

        /* Program data movers with the address of the TX and RX buffers and batch size */
        Xil_Out32(IDMA0_IN0_REG, (UINTPTR)tx_buffer_ptr + idx * INPUT_N_FEATURES);
        Xil_Out32(IDMA0_NUMREPS_REG, 0x1);
        Xil_Out32(ODMA0_OUT_REG, (UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES));
        Xil_Out32(ODMA0_NUMREPS_REG, 1);

        XTime_GetTime(&start);

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


        XTime_GetTime(&stop);
        hw_elapsed += get_elapsed_time(start, stop);

        XTime_GetTime(&start);
        /* Xil_DCacheFlushRange((UINTPTR)rx_buffer_ptr, OUTPUT_N_FEATURES * sizeof(u8)); */
        Xil_DCacheInvalidateRange((UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES * sizeof(u8));
        XTime_GetTime(&stop);
        cache_elapsed += get_elapsed_time(start, stop);

        assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
        assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");
    }
#endif  /* __BATCH_MODE__ */

    /* ****** VALIDATION ****** */
    xil_printf("INFO: ============================== Validation ===============================\n\r");
#ifdef __DEBUG__
    xil_printf("INFO: Dump data\n\r");
    //dump_data("src", (u8*)(tx_buffer_ptr + IMG_ID * INPUT_N_FEATURES), 1, INPUT_N_FEATURES, 1, 0);
    dump_data("sw_dst", (u8*)(rx_buffer_gld_ptr), 1, OUTPUT_N_FEATURES * CMD_N, 1, 0);
    dump_data("hw_dst", (u8*)(rx_buffer_ptr), 1, OUTPUT_N_FEATURES * CMD_N, 1, 0);
#endif

    printf("INFO: Total SW reference model execution time: %f sec\r\n", sw_elapsed);
    printf("INFO: Total HW accelerator execution time: %f sec\r\n", hw_elapsed);
    printf("INFO: Average HW accelerator execution time: %f sec\r\n", hw_elapsed / (CMD_N * ITER_N));
    printf("INFO: Total cache flush time: %f sec\r\n", cache_elapsed);
    //printf("INFO: Accelerator/software speedup (the sofware is fake so this does not count...): %.2f X\r\n", (sw_elapsed >= (hw_elapsed+cache_elapsed))?(sw_elapsed/(hw_elapsed+cache_elapsed)):-((hw_elapsed+cache_elapsed)/sw_elapsed));
    //printf("INFO: CSV: %u,%.12f\n\r", CMD_N, hw_elapsed);

    /* Accelerator validation */
    hw_errors = 0;
    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_N; i++) {
        if ((rx_buffer_ptr)[i] != (rx_buffer_gld_ptr)[i]) {
#ifdef __DEBUG__
            xil_printf("ERROR: [%d]: Accelerator hw %02X != sw %02X !!!\n\r", i, (rx_buffer_ptr)[i], (rx_buffer_gld_ptr)[i]);
#endif
            hw_errors++;
        }
    }

    float error_rate = hw_errors / ((float) OUTPUT_N_FEATURES * CMD_CNT);
    printf("INFO: Total errors = %d (out of %d elements)\r\n", hw_errors, OUTPUT_N_FEATURES * CMD_N);
    printf("INFO: Error rate = %.1f%%\r\n", error_rate * 100);
    printf("INFO: Model accuracy = %.1f%%\r\n", 100 - (error_rate*100));
    if (hw_errors > 0)
        xil_printf("INFO: Accelerator validation: FAIL\n\r");
    else
        xil_printf("INFO: Accelerator validation: PASS!\n\r");

    xil_printf("INFO: Done!\n\r");
    xil_printf("INFO: =========================================================================\n\r");
    cleanup_platform();

    return 0;
}


