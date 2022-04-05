//../../common/hlsmover_main.c

#include "xparameters.h"
#include "unistd.h"      // sleep
#include "xil_printf.h"  // UART debug print functions
#include "xil_cache.h"
#include "stdio.h"       // printf (w/ float) support
#include "platform.h"    // platform init/cleanup functions
#include "xtime_l.h"     // to measure performance of the system
#include "xil_io.h"
//#include "src.h"

/* the top of the hill :-) */

int main(int argc, char** argv)
{

    // ****** TEST HARNESS *******
    //xil_printf("INFO: Initalizing EEMBC Test Harness\r\n");
    //Initalize the test harness
    ee_benchmark_initialize();

    //Pass UART input to the test harness command parser
    //xil_printf("INFO: Handing off to EEMBC Test Harness Command Parser!\r\n");
    while (1) { //Optimize me! TODO Is there a more power efficient way to do this? does it matter?
    	ee_serial_callback(inbyte());
    }

    cleanup_platform();

    return 0;
}
