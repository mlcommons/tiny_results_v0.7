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

#include "monitor/th_api/th_lib.h"
#include "tensorflow/lite/micro/examples/main_functions.h"
#include "hal_data.h"

extern int g_transfer_complete ;
extern uint64_t g_capture_overflows;

extern baud_setting_t g_uart7_baud_setting;

#if EE_CFG_SELFHOSTED != 1

/**
 * PORTME: If there's anything else that needs to be done on init, do it here,
 * othewise OK to leave it alone.
 */
void
th_final_initialize(void)
{
	Setup();
}

/**
 * PORTME: If using energy mode, set up an open-drain GPIO (if it hasn't been
 * done already by the IDE boilerplate). If using performance mode, prepare a
 * timer if necessary.
 */
void
th_timestamp_initialize(void)
{

    /* Initializes the module. */
    R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_AGT_Open(&g_timer1_ctrl, &g_timer1_cfg);

    /* Start the timer. */
    R_AGT_Start(&g_timer1_ctrl);
    R_AGT_Start(&g_timer0_ctrl);

    /* USER CODE 1 END */
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP_MODE);
    /* Always call the timestamp on initialize so that the open-drain output
       is set to "1" (so that we catch a falling edge) */
    th_timestamp();
}

/**
 * PORTME: In "energy mode",generate a falling edge. Since GPIO pin is
 * OPEN-DRAIN it is OK to float and let the pullup resistor drive. In
 * "performance mode", a timer is needed. Depending on the benchmark, the
 * resolution may be as large as 1ms (1000us). In this case, it is OK for the
 * DUT to supply a 1ms timer, just multiply by 1000. Either way, the user must
 * set the microSeconds value in "performance mode".
 *
 * NOTE: The hold time for the GPIO is 1us
 */
void
th_timestamp(void)
{
//#warning "th_timestamp() not implemented"
#if EE_CFG_ENERGY_MODE == 1
/* USER CODE 1 BEGIN */
/* Step 1. Pull pin low */
    R_BSP_PinWrite(BSP_IO_PORT_03_PIN_04, BSP_IO_LEVEL_LOW);
/* Step 2. Hold low for at least 1us */
    for( int i=0; i<30; i++)
    {
        __asm("nop");
    }
/* Step 3. Release driver */
    R_BSP_PinWrite(BSP_IO_PORT_03_PIN_04, BSP_IO_LEVEL_HIGH);
/* USER CODE 1 END */
#else
    unsigned long microSeconds = 0ul;
    /* USER CODE 2 BEGIN */

    timer_info_t info;
    (void) R_AGT_InfoGet(&g_timer0_ctrl, &info);

    uint64_t microsec;
    timer_status_t status0, status1;

    (void) R_AGT_StatusGet(&g_timer0_ctrl, &status0);
    (void) R_AGT_StatusGet(&g_timer1_ctrl, &status1);
    microsec = 0xffff - status0.counter;
    microsec += (0xffff - status1.counter) << 16;
    microsec += g_capture_overflows <<32;
    microsec = ((uint64_t)microsec * 1000 * 1000) / info.clock_frequency;;

    microSeconds = microsec;
    /* USER CODE 2 END */
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP, microSeconds);
#endif
}

/**
 * PORTME: Set up a serialport at 9600 baud to use for communication to the
 * host system if it hasn't already been done, otherwise it is OK to leave this
 * blank.
 *
 * Repeat: for connections through the IO Manager, baud rate is 9600!
 * For connections directly to the Host UI, baud must be 115200.
 */
void
th_serialport_initialize(void)
{
#if EE_CFG_ENERGY_MODE == 1

    g_uart7_baud_setting.cks = 1;
    g_uart7_baud_setting.brr = 161;

    R_SCI_UART_Open(&g_uart7_ctrl, &g_uart7_cfg);

/* In energy mode, we talk to the DUT through the IO Manager at 9600 baud */
#else
/* In performance mode, we talk directly to the DUT at 115200 baud */
    R_SCI_UART_Open(&g_uart7_ctrl, &g_uart7_cfg);

#endif
}

/**
 * PORTME: Modify this function to call the proper printf and send to the
 * serial port.
 *
 * It may only be necessary to comment out this function and define
 * th_printf as printf and just rerout fputc();
 */
void
th_printf(const char *p_fmt, ...)
{
    va_list args;

    while( g_transfer_complete  != 0)		// Wait for message send if need
    {
    	__asm("nop");
    }
    va_start(args, p_fmt);
    g_transfer_complete  = 1;				// Message sending flag.
    (void)th_vprintf(p_fmt, args); /* ignore return */
    va_end(args);
}

/**
 * PORTME: This function is called with a pointer to the command built from the
 * ee_serial_callback() function during the ISR. It is up to the developer
 * to call ee_serial_command_parser_callback() at the next available non-ISR
 * clock with this command string.
 */
void
th_command_ready(char volatile *p_command)
{
//    p_command = p_command;
/**
 * Example of how this might be implemented if there's no need to store
 * the command string locally:
 *
 * ee_serial_command_parser_callback(command);
 *
 * Or, depending on the baremetal/RTOS, it might be necessary to create a
 * static char array in this file, store the command, and then call
 * ee_serial_command_parser_callback() when the system is ready to do
 * work.
 */
//#warning "th_command_ready() not implemented"
	ee_serial_command_parser_callback(p_command);
}

#endif /* EE_CFG_SELFHOSTED */
