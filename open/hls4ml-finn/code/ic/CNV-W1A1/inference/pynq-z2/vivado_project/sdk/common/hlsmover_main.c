#include "xparameters.h"
#include "unistd.h"      /* sleep */
#include "xil_printf.h"  /* UART debug print functions */
#include "xil_cache.h"
#include "stdio.h"       /* printf (w/ float) support */
#include "platform.h"    /* platform init/cleanup functions */
#include "xtime_l.h"     /* to measure performance of the system */
#include "xil_io.h"
#include "assert.h"

#include "src.h"
#include "dst.h"

#define IMG_CNT 2000
#ifndef IMG_N
#define IMG_N 2000
#endif
#define IMG_H 32
#define IMG_W 32
#define IMG_C 3
#define INPUT_N_FEATURES (IMG_H * IMG_W * IMG_C)
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

static u8 *tx_buffer_ptr = (u8*)TX_BUFFER_BASE;
static u8 *rx_buffer_ptr = (u8*)RX_BUFFER_BASE;
static u8 *rx_buffer_gld_ptr = (u8*)RX_BUFFER_GLD_BASE;


/* Golden model of the accelerator in software */
int finn_cnv_w1a1_sw(u8 *rx, u8 *tx, unsigned img_n, unsigned input_n_features, unsigned output_n_features) {
    xil_printf("INFO: Golden results are pre-compiled\r\n");
    xil_printf("INFO: It would be nice to run a software model\r\n");
    // See src.h and dst.h for input and golden output respectively.
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
    for (unsigned i = sample_count - MAX_PRINT_ELEMENTS; i < sample_count; i++)
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

    xil_printf("\n\r");
    xil_printf("INFO: =========================================================================\n\r");
#ifdef __BATCH_MODE__
    xil_printf("INFO: FINN cnv-w1a1 (w/ polling, HLS data mover, batch = %u)\n\r", IMG_N);
#else
    xil_printf("INFO: FINN cnv-w1a1 (w/ polling, HLS data mover, batch = 1)\n\r");
#endif
    xil_printf("INFO: =========================================================================\n\r");

    /* Initialize platform (uart and caches) */
    init_platform();

    //init_axi_dma(DMA_DEV_ID);

    /* Calibration */
    XTime_GetTime(&start);
    sleep(1);
    XTime_GetTime(&stop);
    calibration_time = get_elapsed_time(start, stop);
    printf("INFO: Time calibration for one second (%lf sec)\n\r", calibration_time);

    /* Initialize memory */
    xil_printf("INFO: Initialize memory\n\r");
    xil_printf("INFO:   - Total image count: %u\n\r", IMG_CNT);
    xil_printf("INFO:   - Image batch size: %u\n\r", IMG_N);
    xil_printf("INFO:   - Input-feature count: %u\n\r", INPUT_N_FEATURES);
    xil_printf("INFO:   - Output-feature count: %u\n\r", OUTPUT_N_FEATURES);
    xil_printf("INFO:   - Data size: %u B\n\r", sizeof(u8));

    /* Set Heap Size in ldscript.ld to 0x1000000 (16MB) */
    /* malloc_stats(); */

#if 0
    /* Load data in the TX/RX buffers */
    for (int i = 0; i < INPUT_N_FEATURES; i++) {
        tx_buffer_ptr[i] = (src_data + IMG_ID * INPUT_N_FEATURES)[i];
    }
    for (int i = 0; i < OUTPUT_N_FEATURES; i++) {
        rx_buffer_gld_ptr[i] = (dst_data + IMG_ID * OUTPUT_N_FEATURES)[i];
        rx_buffer_ptr[i] = 0x0;
    }
#else
    /* Load data in the TX/RX buffers */
    for (int i = 0; i < INPUT_N_FEATURES * IMG_N; i++) {
        tx_buffer_ptr[i] = src_data[i];
    }
    for (int i = 0; i < OUTPUT_N_FEATURES * IMG_N; i++) {
        rx_buffer_gld_ptr[i] = dst_data[i];
        rx_buffer_ptr[i] = 0x0;
    }
#endif

    /* ****** SOFTWARE REFERENCE ****** */
#ifdef __DEBUG__
    xil_printf("INFO: Start SW accelerator\n\r");
#endif
    XTime_GetTime(&start);
    finn_cnv_w1a1_sw(tx_buffer_ptr, rx_buffer_gld_ptr, IMG_N, INPUT_N_FEATURES, OUTPUT_N_FEATURES);
    XTime_GetTime(&stop);
    sw_elapsed = get_elapsed_time(start, stop);

    /* ****** ACCELERATOR ****** */
#if 0
    xil_printf("INFO: Press any key to start the accelerator: ");
    dummy = inbyte();
    xil_printf("\n\rINFO: \n\r");
#endif

#ifdef __DEBUG__
    xil_printf("INFO: Configure and start accelerator\n\r");
#endif


#ifdef __BATCH_MODE__

    XTime_GetTime(&start);
    Xil_DCacheFlushRange((UINTPTR)tx_buffer_ptr, IMG_N * INPUT_N_FEATURES * sizeof(u8));
    Xil_DCacheFlushRange((UINTPTR)rx_buffer_ptr, IMG_N * OUTPUT_N_FEATURES * sizeof(u8));
    XTime_GetTime(&stop);
    cache_elapsed = get_elapsed_time(start, stop);

    /* Make sure the data movers are idle */
    assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
    assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");

    /* Program data movers with the address of the TX and RX buffers and batch size */
    Xil_Out32(IDMA0_IN0_REG, (UINTPTR)tx_buffer_ptr);
    Xil_Out32(IDMA0_NUMREPS_REG, IMG_N);
    Xil_Out32(ODMA0_OUT_REG, (UINTPTR)rx_buffer_ptr);
    Xil_Out32(ODMA0_NUMREPS_REG, IMG_N);

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
    Xil_DCacheInvalidateRange((UINTPTR)rx_buffer_ptr, IMG_N * OUTPUT_N_FEATURES * sizeof(u8));
    XTime_GetTime(&stop);
    cache_elapsed += get_elapsed_time(start, stop);

    assert (((Xil_In32(IDMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "idma0 NOT IDLE!");
    assert (((Xil_In32(ODMA0_AP_CTRL_REG) >> 2) & 0x1) == 0x1 && "odma0 NOT IDLE!");

#else /* __BATCH_MODE__ */
#ifdef __DEBUG__
    xil_printf("INFO: ");
#endif


    for (int idx = 0; idx < IMG_N; idx++) {
#ifdef __DEBUG__
        /* xil_printf("INFO: Image #%u\n\r", idx); */
        xil_printf(".");
#endif

        XTime_GetTime(&start);
        Xil_DCacheFlushRange((UINTPTR)(tx_buffer_ptr + idx * INPUT_N_FEATURES), INPUT_N_FEATURES * sizeof(u8));
        Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(u8));
        XTime_GetTime(&stop);
        cache_elapsed = get_elapsed_time(start, stop);

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
#ifdef __DEBUG__
        xil_printf("\n\r");
#endif
#endif  /* __BATCH_MODE__ */

    /* ****** VALIDATION ****** */

    xil_printf("INFO: ============================== Validation ===============================\n\r");

#ifdef __DEBUG__
    xil_printf("INFO: Dump data\n\r");
    //dump_data("src", (u8*)(tx_buffer_ptr + IMG_ID * INPUT_N_FEATURES), 1, INPUT_N_FEATURES, 1, 0);
    dump_data("sw_dst", (u8*)(rx_buffer_gld_ptr), 1, OUTPUT_N_FEATURES * IMG_N, 1, 0);
    dump_data("hw_dst", (u8*)(rx_buffer_ptr), 1, OUTPUT_N_FEATURES * IMG_N, 1, 0);
#endif

    printf("INFO: Software execution time: %f sec\n\r", sw_elapsed);

    printf("INFO: Total accelerator execution time: %f sec\n\r", hw_elapsed);
    printf("INFO: Average accelerator execution time: %f sec\n\r", hw_elapsed / IMG_N);
    printf("INFO: Cache flush time: %f sec\n\r", cache_elapsed);
    printf("INFO: Accelerator/software speedup (the sofware is fake so this does not count...): %.2f X\n\r", (sw_elapsed >= (hw_elapsed+cache_elapsed))?(sw_elapsed/(hw_elapsed+cache_elapsed)):-((hw_elapsed+cache_elapsed)/sw_elapsed));
    printf("INFO: CSV: %u,%.12f\n\r", IMG_N, hw_elapsed);

    /* Accelerator validation */
    hw_errors = 0;
    for (int i = 0; i < OUTPUT_N_FEATURES * IMG_N; i++) {
        if ((rx_buffer_ptr)[i] != (rx_buffer_gld_ptr)[i]) {
#ifdef __DEBUG__
            xil_printf("ERROR: [%d]: Accelerator hw %02X != sw %02X !!!\n\r", i, (rx_buffer_ptr)[i], (rx_buffer_gld_ptr)[i]);
#endif
            hw_errors++;
        }
    }

    float error_rate = hw_errors / ((float) OUTPUT_N_FEATURES * IMG_N);
    printf("INFO: Total errors = %d (out of %d elements, error rate = %.1f%%)\n\r", hw_errors, OUTPUT_N_FEATURES * IMG_N, error_rate * 100);
    if (hw_errors > 0)
        xil_printf("INFO: Accelerator validation: FAIL\n\r");
    else
        xil_printf("INFO: Accelerator validation: PASS!\n\r");

    xil_printf("INFO: Done!\n\r");
    xil_printf("INFO: =========================================================================\n\r");

    cleanup_platform();

    return 0;
}


