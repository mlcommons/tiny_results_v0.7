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

#define TRANSFER_LENGTH (128)

uint8_t  g_dest[TRANSFER_LENGTH];
uint8_t  g_src[TRANSFER_LENGTH];
uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
uint32_t g_transfer_complete = 0;
uint32_t g_receive_complete  = 0;
uint32_t g_out_of_band_index = 0;

uint32_t rx_read_pos = 0;

uint64_t g_capture_overflows = 0U;

void user_uart7_callback (uart_callback_args_t * p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            /* Only put the next character in the receive buffer if there is space for it */
            if (sizeof(g_out_of_band_received) > g_out_of_band_index)
            {
                /* Write either the next one or two bytes depending on the receive data size */
                if (UART_DATA_BITS_8 >= g_uart7_cfg.data_bits)
                {
                    g_out_of_band_received[g_out_of_band_index++] = (uint8_t) p_args->data;
                }
                else
                {
                    uint16_t * p_dest = (uint16_t *) &g_out_of_band_received[g_out_of_band_index];
                    *p_dest              = (uint16_t) p_args->data;
                    g_out_of_band_index += 2;
                }
                if( g_out_of_band_index >=TRANSFER_LENGTH)
                {
                    g_out_of_band_index = 0;
                }
            }
            break;
        }
        /* Receive complete */
        case UART_EVENT_RX_COMPLETE:
        {
            g_receive_complete = 1;
            break;
        }
        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            g_transfer_complete = 0;
            break;
        }
        default:
        {
        }
    }
}


uint8_t r_uart7_receive_getch(void)
{
    uint8_t ch = 0;

    if( g_out_of_band_index != rx_read_pos )
    {
        ch = g_out_of_band_received[rx_read_pos++];
    }

    if( TRANSFER_LENGTH <= rx_read_pos)
    {
        rx_read_pos = 0;
    }
    return ch;
}


void timer_capture_callback (timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* An overflow occurred during capture. This must be accounted for at the application layer. */
        g_capture_overflows++;
    }
}

