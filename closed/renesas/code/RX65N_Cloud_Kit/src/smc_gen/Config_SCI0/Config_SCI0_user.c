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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : Config_SCI0_user.c
* Version      : 1.9.2
* Device(s)    : R5F565NEDxFP
* Description  : This file implements device driver for Config_SCI0.
* Creation Date: 2021-06-15
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_SCI0.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_sci0_tx_address;                /* SCI0 transmit buffer address */
extern volatile uint16_t  g_sci0_tx_count;                   /* SCI0 transmit data number */
extern volatile uint8_t * gp_sci0_rx_address;                /* SCI0 receive buffer address */
extern volatile uint16_t  g_sci0_rx_count;                   /* SCI0 receive data number */
extern volatile uint16_t  g_sci0_rx_length;                  /* SCI0 receive data length */
/* Start user code for global. Do not edit comment generated here */
#define TRANSFER_LENGTH (128)

uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
uint32_t g_transfer_complete = 0;
uint32_t g_receive_complete  = 0;
uint32_t g_out_of_band_index = 0;

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Create_UserInit
* Description  : This function adds user code after initializing the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    g_sci0_rx_count = 0U;
    g_sci0_rx_length = TRANSFER_LENGTH;
    gp_sci0_rx_address = g_out_of_band_received;
    SCI0.SCR.BIT.RIE = 1U;
    SCI0.SCR.BIT.RE = 1U;

    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_transmit_interrupt
* Description  : This function is TXI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI0_transmit_interrupt(void)
{
    if (0U < g_sci0_tx_count)
    {
        SCI0.TDR = *gp_sci0_tx_address;
        gp_sci0_tx_address++;
        g_sci0_tx_count--;
    }
    else
    {
        SCI0.SCR.BIT.TIE = 0U;
        SCI0.SCR.BIT.TEIE = 1U;
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_transmitend_interrupt
* Description  : This function is TEI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI0_transmitend_interrupt(void)
{
    /* Set TXD0 pin */
    PORT3.PMR.BYTE &= 0xFBU;

    SCI0.SCR.BIT.TIE = 0U;
    SCI0.SCR.BIT.TE = 0U;
    SCI0.SCR.BIT.TEIE = 0U;
    
    r_Config_SCI0_callback_transmitend();
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_receive_interrupt
* Description  : This function is RXI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI0_receive_interrupt(void)
{
	g_out_of_band_received[g_out_of_band_index++] = SCI0.RDR;
    
    if( TRANSFER_LENGTH <= g_out_of_band_index )
    {
        g_out_of_band_index = 0;
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_callback_transmitend
* Description  : This function is a callback function when SCI0 finishes transmission
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_SCI0_callback_transmitend(void)
{
    /* Start user code for r_Config_SCI0_callback_transmitend. Do not edit comment generated here */
	g_transfer_complete = 0;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_callback_receiveend
* Description  : This function is a callback function when SCI0 finishes reception
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_SCI0_callback_receiveend(void)
{
    /* Start user code for r_Config_SCI0_callback_receiveend. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
void R_Config_SCI0_Create_9600bps(void)
{
    /* Cancel SCI stop state */
    MSTP(SCI0) = 0U;

    /* Set interrupt priority */
    IPR(SCI0, RXI0) = _02_SCI_PRIORITY_LEVEL2;
    IPR(SCI0, TXI0) = _02_SCI_PRIORITY_LEVEL2;

    /* Clear the control register */
    SCI0.SCR.BYTE = 0x00U;

    /* Set clock enable */
    SCI0.SCR.BYTE = _00_SCI_INTERNAL_SCK_UNUSED;

    /* Clear the SIMR1.IICM, SPMR.CKPH, and CKPOL bit, and set SPMR */
    SCI0.SIMR1.BIT.IICM = 0U;
    SCI0.SPMR.BYTE = _00_SCI_RTS | _00_SCI_CLOCK_NOT_INVERTED | _00_SCI_CLOCK_NOT_DELAYED;

    /* Set control registers */
    SCI0.SMR.BYTE = _00_SCI_CLOCK_PCLK | _00_SCI_MULTI_PROCESSOR_DISABLE | _00_SCI_STOP_1 | _00_SCI_PARITY_DISABLE | 
                    _00_SCI_DATA_LENGTH_8 | _00_SCI_ASYNCHRONOUS_OR_I2C_MODE;
    SCI0.SCMR.BYTE = _00_SCI_SERIAL_MODE | _00_SCI_DATA_INVERT_NONE | _00_SCI_DATA_LSB_FIRST | 
                     _10_SCI_DATA_LENGTH_8_OR_7 | _62_SCI_SCMR_DEFAULT;
    SCI0.SEMR.BYTE = _00_SCI_BIT_MODULATION_DISABLE | _00_SCI_16_BASE_CLOCK | _00_SCI_NOISE_FILTER_DISABLE | 
                     _00_SCI_BAUDRATE_SINGLE | _00_SCI_LOW_LEVEL_START_BIT;

    /* Set bit rate */
    SCI0.BRR = 0xC2U;

    /* Set RXD0 pin */
    MPC.P33PFS.BYTE = 0x0BU;
    PORT3.PMR.BYTE |= 0x08U;

    /* Set TXD0 pin */
    MPC.P32PFS.BYTE = 0x0BU;
    PORT3.PODR.BYTE |= 0x04U;
    PORT3.PDR.BYTE |= 0x04U;

    R_Config_SCI0_Create_UserInit();
}
/* End user code. Do not edit comment generated here */
