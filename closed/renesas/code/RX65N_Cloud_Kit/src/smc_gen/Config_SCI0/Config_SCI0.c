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
* File Name    : Config_SCI0.c
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
volatile uint8_t * gp_sci0_tx_address;                /* SCI0 transmit buffer address */
volatile uint16_t  g_sci0_tx_count;                   /* SCI0 transmit data number */
volatile uint8_t * gp_sci0_rx_address;                /* SCI0 receive buffer address */
volatile uint16_t  g_sci0_rx_count;                   /* SCI0 receive data number */
volatile uint16_t  g_sci0_rx_length;                  /* SCI0 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Create
* Description  : This function initializes the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Create(void)
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
    SCI0.SEMR.BYTE = _00_SCI_BIT_MODULATION_DISABLE | _10_SCI_8_BASE_CLOCK | _00_SCI_NOISE_FILTER_DISABLE | 
                     _40_SCI_BAUDRATE_DOUBLE | _00_SCI_LOW_LEVEL_START_BIT;

    /* Set bit rate */
    SCI0.BRR = 0x40U;

    /* Set RXD0 pin */
    MPC.P33PFS.BYTE = 0x0BU;
    PORT3.PMR.BYTE |= 0x08U;

    /* Set TXD0 pin */
    MPC.P32PFS.BYTE = 0x0BU;
    PORT3.PODR.BYTE |= 0x04U;
    PORT3.PDR.BYTE |= 0x04U;

    R_Config_SCI0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Start
* Description  : This function starts the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Start(void)
{
    /* Clear interrupt flag */
    IR(SCI0, TXI0) = 0U;
    IR(SCI0, RXI0) = 0U;

    /* Enable SCI interrupt */
    IEN(SCI0, TXI0) = 1U;
    ICU.GENBL0.BIT.EN0 = 1U;
    IEN(SCI0, RXI0) = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Stop
* Description  : This function stop the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Stop(void)
{
    /* Set TXD0 pin */
    PORT3.PMR.BYTE &= 0xFBU;

    /* Disable serial transmit */
    SCI0.SCR.BIT.TE = 0U;

    /* Disable serial receive */
    SCI0.SCR.BIT.RE = 0U;

    /* Disable SCI interrupt */
    SCI0.SCR.BIT.TIE = 0U;
    SCI0.SCR.BIT.RIE = 0U;
    IEN(SCI0, TXI0) = 0U;
    ICU.GENBL0.BIT.EN0 = 0U;
    IR(SCI0, TXI0) = 0U;
    IEN(SCI0, RXI0) = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Serial_Receive
* Description  : This function receive SCI0data
* Arguments    : rx_buf -
*                    receive buffer pointer (Not used when receive data handled by DMAC/DTC)
*                rx_num -
*                    buffer size (Not used when receive data handled by DMAC/DTC)
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/

MD_STATUS R_Config_SCI0_Serial_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (1U > rx_num)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_sci0_rx_count = 0U;
        g_sci0_rx_length = rx_num;
        gp_sci0_rx_address = rx_buf;
        SCI0.SCR.BIT.RIE = 1U;
        SCI0.SCR.BIT.RE = 1U;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Serial_Send
* Description  : This function transmits SCI0data
* Arguments    : tx_buf -
*                    transfer buffer pointer (Not used when transmit data handled by DMAC/DTC)
*                tx_num -
*                    buffer size (Not used when transmit data handled by DMAC/DTC)
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/

MD_STATUS R_Config_SCI0_Serial_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (1U > tx_num)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_sci0_tx_address = tx_buf;
        g_sci0_tx_count = tx_num;

        /* Set TXD0 pin */
        PORT3.PMR.BYTE |= 0x04U;
        SCI0.SCR.BYTE |= 0xA0U;
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
