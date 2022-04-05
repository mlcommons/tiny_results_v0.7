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
* File Name    : Config_MTU1.c
* Version      : 1.9.2
* Device(s)    : R5F565NEDxFP
* Description  : This file implements device driver for Config_MTU1.
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
#include "Config_MTU1.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU1_Create
* Description  : This function initializes the MTU1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU1_Create(void)
{
    /* Release MTU channel 1 from stop state */
    MSTP(MTU3) = 0U;

    /* Stop MTU channel 1 counter */
    MTU.TSTRA.BIT.CST1 = 0U;

    /* Set TCIV1 interrupt priority level */
    ICU.SLIAR217.BYTE = 0x0AU;
    IPR(PERIA, INTA217) = _0F_MTU_PRIORITY_LEVEL15;

    /* MTU channel 1 is used as normal mode */
    MTU.TSYRA.BIT.SYNC1 = 0U;
    MTU1.TCR.BYTE = _07_MTU_COUNT | _00_MTU_CKCL_DIS;
    MTU1.TCR2.BYTE = _00_MTU_PCLK_1;
    MTU1.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _10_MTU_TCIEV_ENABLE | _00_MTU_TTGE_DISABLE;
    MTU1.TIOR.BYTE = _00_MTU_IOA_DISABLE | _00_MTU_IOB_DISABLE;
    MTU1.TGRB = _0063_TGRB1_VALUE;

    R_Config_MTU1_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU1_Start
* Description  : This function starts the MTU1 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU1_Start(void)
{
    /* Enable TCIV1 interrupt in ICU */
    IEN(PERIA, INTA217) = 1U;
    
    /* Start MTU channel 1 counter */
    MTU.TSTRA.BIT.CST1 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU1_Stop
* Description  : This function stops the MTU1 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU1_Stop(void)
{
    /* Disable TCIV1 interrupt in ICU */
    IEN(PERIA, INTA217) = 0U;
    
    /* Stop MTU channel 1 counter */
    MTU.TSTRA.BIT.CST1 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */


/* End user code. Do not edit comment generated here */
