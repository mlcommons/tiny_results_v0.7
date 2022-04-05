/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. 
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"
#ifdef __cplusplus
 extern "C" {
#endif

#include "tensorflow/lite/micro/examples/main_functions.h"

 #include "monitor/ee_main.h"

#ifdef __cplusplus
             }
#endif

void R_BSP_WarmStart(bsp_warm_start_event_t event);
extern uint8_t r_uart7_receive_getch(void);

void hal_entry (void)
{

    R_BSP_PinAccessEnable();
    R_BSP_PinWrite(BSP_IO_PORT_03_PIN_04, BSP_IO_LEVEL_HIGH);

#if 1
    /* Configure the S-Cache line size. */
    R_CACHE->SCALCF = BSP_CFG_C_CACHE_LINE_SIZE;

    /* Enable the S-Cache. */
    R_CACHE->SCACTL = 1U;

#endif

    ee_benchmark_initialize();

    char    ch=0;
    while(1)
    {
        ch = (char)r_uart7_receive_getch();
       if( ch != 0)
        {
            ee_serial_callback(ch);
        }

    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}
