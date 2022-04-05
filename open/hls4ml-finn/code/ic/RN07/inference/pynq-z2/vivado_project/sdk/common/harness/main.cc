#include "api/internally_implemented.h"

#include "xil_printf.h"
#include "xil_io.h"      /* peripheral read/write wrappers */
#include "platform.h"    /* platform init/cleanup functions */
#include "xil_cache.h"   /* enable/disable caches etc */
#include "xil_printf.h"  /* UART debug print functions */
#include "xparameters.h" /* peripherals base addresses */
#define DEBUG 1
int main(void) {
	init_platform();
    ee_benchmark_initialize();
    //xil_printf("EEMBC Harness Init!");
    while (1) {
        char c = inbyte();
        if (c == '\000') continue;
        if (c == '\r') continue; //xSDK Serial Terminal Fix
        ee_serial_callback(c);
    }

    return 0;
}
