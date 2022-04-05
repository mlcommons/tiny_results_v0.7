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
* File Name    : r_cg_tpu.h
* Version      : 1.2.102
* Device(s)    : R5F565NEDxFP
* Description  : General header file for TPU peripheral.
* Creation Date: 2021-06-15
***********************************************************************************************************************/

#ifndef TPU_H
#define TPU_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Timer Control Register (TCR)
*/
/* Time Pre-scaler Select (TPSC[2:0]) */
#define _00_TPU_PCLK_1                     (0x00U) /* Internal clock: counts on PCLK/1 */
#define _01_TPU_PCLK_4                     (0x01U) /* Internal clock: counts on PCLK/4 */
#define _02_TPU_PCLK_16                    (0x02U) /* Internal clock: counts on PCLK/16 */
#define _03_TPU_PCLK_64                    (0x03U) /* Internal clock: counts on PCLK/64 */
#define _06_TPU_PCLK_256                   (0x06U) /* Internal clock: counts on PCLK/256 */
#define _07_TPU2_PCLK_1024                 (0x07U) /* TPU2 Internal clock: counts on PCLK/1024 */
#define _05_TPU3_PCLK_1024                 (0x05U) /* TPU3 Internal clock: counts on PCLK/1024 */
#define _06_TPU4_PCLK_1024                 (0x06U) /* TPU4 Internal clock: counts on PCLK/1024 */
#define _07_TPU_PCLK_4096                  (0x07U) /* Internal clock: counts on PCLK/4096 */
#define _04_TPU_TCLKA                      (0x04U) /* External clock: counts on TCLKA pin input */
#define _05_TPU_TCLKB                      (0x05U) /* External clock: counts on TCLKB pin input */
#define _06_TPU_TCLKC                      (0x06U) /* External clock: counts on TCLKC pin input */
#define _05_TPU_TCLKC                      (0x05U) /* External clock: counts on TCLKC pin input */
#define _07_TPU_TCLKD                      (0x07U) /* External clock: counts on TCLKD pin input */
#define _07_TPU2_COUNT                     (0x07U) /* TPU1: Counts on TPU2.TCNT counter overflow/underflow */
#define _07_TPU5_COUNT                     (0x07U) /* TPU4: Counts on TPU5.TCNT counter overflow/underflow */
/* Clock Edge Select (CKEG[1:0]) */
#define _00_TPU_CKEG_IT_F                  (0x00U) /* Internal Clock: Count at falling edge */
#define _00_TPU_CKEG_EX_R                  (0x00U) /* External Clock: Count at rising edge */
#define _08_TPU_CKEG_IT_R                  (0x08U) /* Internal Clock: Count at rising edge */
#define _08_TPU_CKEG_EX_F                  (0x08U) /* External Clock: Count at falling edge */
#define _10_TPU_CKEG_BOTH                  (0x10U) /* Count at both edge */
/* Counter Clear Select (CCLR[2:0]) */
#define _00_TPU_CKCL_DIS                   (0x00U) /* TCNT clearing disabled */
#define _20_TPU_CKCL_A                     (0x20U) /* TCNT cleared by TGRA compare match/input capture */
#define _40_TPU_CKCL_B                     (0x40U) /* TCNT cleared by TGRB compare match/input capture */
#define _60_TPU_CKCL_SYN                   (0x60U) /* TCNT cleared by counter clearing in another synchronous channel */
#define _A0_TPU_CKCL_C                     (0xA0U) /* TCNT cleared by TGRC compare match/input capture */
#define _C0_TPU_CKCL_D                     (0xC0U) /* TCNT cleared by TGRD compare match/input capture */

/*
    Timer Mode Register (TMDR)
*/
/* Mode Select (MD[3:0]) */
#define _00_TPU_NORMAL                     (0x00U) /* Normal mode */
#define _02_TPU_PWM1                       (0x02U) /* PWM mode 1 */
#define _03_TPU_PWM2                       (0x03U) /* PWM mode 2 */
#define _04_TPU_COT1                       (0x04U) /* Phase counting mode 1 */
#define _05_TPU_COT2                       (0x05U) /* Phase counting mode 2 */
#define _06_TPU_COT3                       (0x06U) /* Phase counting mode 3 */
#define _07_TPU_COT4                       (0x07U) /* Phase counting mode 4 */
/* Buffer Operation A (BFA) */
#define _00_TPU_BFA_NORMAL                 (0x00U) /* TPUm.TGRA operates normally (m = 0, 3) */
#define _10_TPU_BFA_BUFFER                 (0x10U) /* TPUm.TGRA and TPUm.TGRC used together for buffer operation */
/* Buffer Operation B (BFB) */
#define _00_TPU_BFB_NORMAL                 (0x00U) /* TPUm.TGRB operates normally (m = 0, 3) */
#define _20_TPU_BFB_BUFFER                 (0x20U) /* TPUm.TGRB and TPUm.TGRD used together for buffer operation */
/* TGRB Input Capture Input Select (ICSELB) */
#define _00_TPU_ICSELB_BPIN                (0x00U) /* Input capture input source is TIOCBn pin */
#define _40_TPU_ICSELB_APIN                (0x40U) /* Input capture input source is TIOCAn pin (n = 0 to 5) */
/* TGRD Input Capture Input Select (ICSELD) */
#define _00_TPU_ICSELD_DPIN                (0x00U) /* Input capture input source is TIOCDn pin */
#define _80_TPU_ICSELD_CPIN                (0x80U) /* Input capture input source is TIOCCn pin (n = 0, 3) */

/*
    Timer I/O Control Register (TIOR)
*/
/* I/O Control A (IOA[3:0]) for TPU0.TIORH, TPU1.TIOR, TPU2.TIOR, TPU3.TIORH, TPU4.TIOR, TPU5.TIOR */
#define _00_TPU_IOA_DISABLE                (0x00U) /* Output prohibited */
#define _01_TPU_IOA_LL                     (0x01U) /* Initial output is low. Low output at compare match */
#define _02_TPU_IOA_LH                     (0x02U) /* Initial output is low. High output at compare match */
#define _03_TPU_IOA_LT                     (0x03U) /* Initial output is low. Toggle output at compare match */
#define _05_TPU_IOA_HL                     (0x05U) /* Initial output is high. Low output at compare match */
#define _06_TPU_IOA_HH                     (0x06U) /* Initial output is high. High output at compare match */
#define _07_TPU_IOA_HT                     (0x07U) /* Initial output is high. Toggle output at compare match */
#define _08_TPU_IOA_IR                     (0x08U) /* Input capture at rising edge */
#define _09_TPU_IOA_IF                     (0x09U) /* Input capture at falling edge */
#define _0A_TPU_IOA_IB                     (0x0AU) /* Input capture at both edges */
#define _0C_TPU_IOA_EX                     (0x0CU) /* Input capture at TPU1,4.TCNT up-count/down-count */
#define _0D_TPU_IOA_TGRA                   (0x0DU) /* Input capture at TPU0,3.TGRA compare match/input capture */
/* I/O Control B (IOB[3:0]) for TPU0.TIORH, TPU1.TIOR, TPU2.TIOR, TPU3.TIORH, TPU4.TIOR, TPU5.TIOR */
#define _00_TPU_IOB_DISABLE                (0x00U) /* Output prohibited */
#define _10_TPU_IOB_LL                     (0x10U) /* Initial output is low. Low output at compare match */
#define _20_TPU_IOB_LH                     (0x20U) /* Initial output is low. High output at compare match */
#define _30_TPU_IOB_LT                     (0x30U) /* Initial output is low. Toggle output at compare match */
#define _50_TPU_IOB_HL                     (0x50U) /* Initial output is high. Low output at compare match */
#define _60_TPU_IOB_HH                     (0x60U) /* Initial output is high. High output at compare match */
#define _70_TPU_IOB_HT                     (0x70U) /* Initial output is high. Toggle output at compare match */
#define _80_TPU_IOB_IR                     (0x80U) /* Input capture at rising edge */
#define _90_TPU_IOB_IF                     (0x90U) /* Input capture at falling edge */
#define _A0_TPU_IOB_IB                     (0xA0U) /* Input capture at both edges */
#define _C0_TPU_IOB_EX                     (0xC0U) /* Input capture at TPU1,4.TCNT up-count/down-count */
#define _C0_TPU_IOB_TGRC0                  (0xC0U) /* Input capture at TPU0,3.TGRC compare match/input capture */
/* I/O Control C (IOC[3:0]) for TPU0.TIORL, TPU3.TIORL */
#define _00_TPU_IOC_DISABLE                (0x00U) /* Output prohibited */
#define _01_TPU_IOC_LL                     (0x01U) /* Initial output is low. Low output at compare match */
#define _02_TPU_IOC_LH                     (0x02U) /* Initial output is low. High output at compare match */
#define _03_TPU_IOC_LT                     (0x03U) /* Initial output is low. Toggle output at compare match */
#define _05_TPU_IOC_HL                     (0x05U) /* Initial output is high. Low output at compare match */
#define _06_TPU_IOC_HH                     (0x06U) /* Initial output is high. High output at compare match */
#define _07_TPU_IOC_HT                     (0x07U) /* Initial output is high. Toggle output at compare match */
#define _08_TPU_IOC_IR                     (0x08U) /* Input capture at rising edge */
#define _09_TPU_IOC_IF                     (0x09U) /* Input capture at falling edge */
#define _0A_TPU_IOC_IB                     (0x0AU) /* Input capture at both edges */
#define _0C_TPU_IOC_EX                     (0x0CU) /* Input capture at TPU1,4.TCNT up-count/down-count */
/* I/O Control D (IOD[3:0]) for TPU0.TIORL, TPU3.TIORL */
#define _00_TPU_IOD_DISABLE                (0x00U) /* Output prohibited */
#define _10_TPU_IOD_LL                     (0x10U) /* Initial output is low. Low output at compare match */
#define _20_TPU_IOD_LH                     (0x20U) /* Initial output is low. High output at compare match */
#define _30_TPU_IOD_LT                     (0x30U) /* Initial output is low. Toggle output at compare match */
#define _50_TPU_IOD_HL                     (0x50U) /* Initial output is high. Low output at compare match */
#define _60_TPU_IOD_HH                     (0x60U) /* Initial output is high. High output at compare match */
#define _70_TPU_IOD_HT                     (0x70U) /* Initial output is high. Toggle output at compare match. */
#define _80_TPU_IOD_IR                     (0x80U) /* Input capture at rising edge */
#define _90_TPU_IOD_IF                     (0x90U) /* Input capture at falling edge */
#define _A0_TPU_IOD_IB                     (0xA0U) /* Input capture at both edges */
#define _C0_TPU_IOD_EX                     (0xC0U) /* Input capture at TPU1,4.TCNT up-count/down-count */

/*
    Timer Start Registers (TSTR)
*/
/* Counter Start 0 (CST0) */
#define _00_TPU_CST0_OFF                   (0x00U) /* TPU0.TCNT performs count stop */
#define _01_TPU_CST0_ON                    (0x01U) /* TPU0.TCNT performs count operation */
/* Counter Start 1 (CST1) */
#define _00_TPU_CST1_OFF                   (0x00U) /* TPU1.TCNT performs count stop */
#define _02_TPU_CST1_ON                    (0x02U) /* TPU1.TCNT performs count operation */
/* Counter Start 2 (CST2) */
#define _00_TPU_CST2_OFF                   (0x00U) /* TPU2.TCNT performs count stop */
#define _04_TPU_CST2_ON                    (0x04U) /* TPU2.TCNT performs count operation */
/* Counter Start 3 (CST3) */
#define _00_TPU_CST3_OFF                   (0x00U) /* TPU3.TCNT performs count stop */
#define _08_TPU_CST3_ON                    (0x08U) /* TPU3.TCNT performs count operation */
/* Counter Start 4 (CST4) */
#define _00_TPU_CST4_OFF                   (0x00U) /* TPU4.TCNT performs count stop */
#define _10_TPU_CST4_ON                    (0x10U) /* TPU4.TCNT performs count operation */
/* Counter Start 5 (CST5) */
#define _00_TPU_CST5_OFF                   (0x00U) /* TPU5.TCNT performs count stop */
#define _20_TPU_CST5_ON                    (0x20U) /* TPU5.TCNT performs count operation */

/*
    Timer Synchronous Register (TSYR)
*/
/* Timer Synchronization 0 (SYNC0) */
#define _00_TPU_SYNC0_OFF                  (0x00U) /* TPU0.TCNT operates independently */
#define _01_TPU_SYNC0_ON                   (0x01U) /* TPU0.TCNT performs synchronous operation */
/* Timer Synchronization 1 (SYNC1) */
#define _00_TPU_SYNC1_OFF                  (0x00U) /* TPU1.TCNT operates independently */
#define _02_TPU_SYNC1_ON                   (0x02U) /* TPU1.TCNT performs synchronous operation */
/* Timer Synchronization 2 (SYNC2) */
#define _00_TPU_SYNC2_OFF                  (0x00U) /* TPU2.TCNT operates independently */
#define _04_TPU_SYNC2_ON                   (0x04U) /* TPU2.TCNT performs synchronous operation */
/* Timer Synchronization 3 (SYNC3) */
#define _00_TPU_SYNC3_OFF                  (0x00U) /* TPU3.TCNT operates independently */
#define _08_TPU_SYNC3_ON                   (0x08U) /* TPU3.TCNT performs synchronous operation */
/* Timer Synchronization 4 (SYNC4) */
#define _00_TPU_SYNC4_OFF                  (0x00U) /* TPU4.TCNT operates independently */
#define _10_TPU_SYNC4_ON                   (0x10U) /* TPU4.TCNT performs synchronous operation */
/* Timer Synchronization 5 (SYNC5) */
#define _00_TPU_SYNC5_OFF                  (0x00U) /* TPU5.TCNT operates independently */
#define _20_TPU_SYNC5_ON                   (0x20U) /* TPU5.TCNT performs synchronous operation */

/*
    Noise Filter Control Register (NFCR) (m = 0 to 5)
*/
/* Noise Filter A Enable Bit (NFAEN) */
#define _00_TPU_NFAEN_DISABLE              (0x00U) /* The noise filter for the TIOCAm pin is disabled */
#define _01_TPU_NFAEN_ENABLE               (0x01U) /* The noise filter for the TIOCAm pin is enabled */
/* Noise Filter B Enable Bit (NFBEN) */
#define _00_TPU_NFBEN_DISABLE              (0x00U) /* The noise filter for the TIOCBm pin is disabled */
#define _02_TPU_NFBEN_ENABLE               (0x02U) /* The noise filter for the TIOCBm pin is enabled */
/* Noise Filter C Enable Bit (NFCEN) */
#define _00_TPU_NFCEN_DISABLE              (0x00U) /* The noise filter for the TIOCCm pin is disabled */
#define _04_TPU_NFCEN_ENABLE               (0x04U) /* The noise filter for the TIOCCm pin is enabled */
/* Noise Filter D Enable Bit (NFDEN) */
#define _00_TPU_NFDEN_DISABLE              (0x00U) /* The noise filter for the TIOCDm pin is disabled */
#define _08_TPU_NFDEN_ENABLE               (0x08U) /* The noise filter for the TIOCDm pin is enabled */
/* Noise Filter Clock Select (NFCS[1:0]) */
#define _00_TPU_NFCS_PCLK_1                (0x00U) /* PCLK/1 */
#define _10_TPU_NFCS_PCLK_8                (0x10U) /* PCLK/8 */
#define _20_TPU_NFCS_PCLK_32               (0x20U) /* PCLK/32 */
#define _30_TPU_NFCS_EXCLK                 (0x30U) /* The clock source for counting is the external clock */

/*
    Timer Interrupt Enable Register (TIER)
*/
/* TGR Interrupt Enable A (TGIEA) */
#define _00_TPU_TGIEA_DISABLE              (0x00U) /* Interrupt requests TGIA disabled */
#define _01_TPU_TGIEA_ENABLE               (0x01U) /* Interrupt requests TGIA enabled */
/* TGR Interrupt Enable B (TGIEB) */
#define _00_TPU_TGIEB_DISABLE              (0x00U) /* Interrupt requests TGIB disabled */
#define _02_TPU_TGIEB_ENABLE               (0x02U) /* Interrupt requests TGIB enabled */
/* TGR Interrupt Enable C (TGIEC) */
#define _00_TPU_TGIEC_DISABLE              (0x00U) /* Interrupt requests TGIC disabled */
#define _04_TPU_TGIEC_ENABLE               (0x04U) /* Interrupt requests TGIC enabled */
/* TGR Interrupt Enable D (TGIED) */
#define _00_TPU_TGIED_DISABLE              (0x00U) /* Interrupt requests TGID disabled */
#define _08_TPU_TGIED_ENABLE               (0x08U) /* Interrupt requests TGID enabled */
/* Overflow Interrupt Enable (TCIEV) */
#define _00_TPU_TCIEV_DISABLE              (0x00U) /* Interrupt requests TCIV disabled */
#define _10_TPU_TCIEV_ENABLE               (0x10U) /* Interrupt requests TCIV enabled */
/* Underflow Interrupt Enable (TCIEU) */
#define _00_TPU_TCIEU_DISABLE              (0x00U) /* Interrupt requests TCIU disabled */
#define _20_TPU_TCIEU_ENABLE               (0x20U) /* Interrupt requests TCIU enabled */
/* A/D Converter Start Request Enable (TTGE) */
#define _00_TPU_TTGE_DISABLE               (0x00U) /* A/D converter start request generation disabled */
#define _80_TPU_TTGE_ENABLE                (0x80U) /* A/D converter start request generation enabled */
/* TIER default value */
#define _40_TPU_TIER_DEFAULT               (0x40U) /* Write default value of TIER */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_TPU_PRIORITY_LEVEL0            (0x00U) /* Level 0 (interrupt disabled) */
#define _01_TPU_PRIORITY_LEVEL1            (0x01U) /* Level 1 */
#define _02_TPU_PRIORITY_LEVEL2            (0x02U) /* Level 2 */
#define _03_TPU_PRIORITY_LEVEL3            (0x03U) /* Level 3 */
#define _04_TPU_PRIORITY_LEVEL4            (0x04U) /* Level 4 */
#define _05_TPU_PRIORITY_LEVEL5            (0x05U) /* Level 5 */
#define _06_TPU_PRIORITY_LEVEL6            (0x06U) /* Level 6 */
#define _07_TPU_PRIORITY_LEVEL7            (0x07U) /* Level 7 */
#define _08_TPU_PRIORITY_LEVEL8            (0x08U) /* Level 8 */
#define _09_TPU_PRIORITY_LEVEL9            (0x09U) /* Level 9 */
#define _0A_TPU_PRIORITY_LEVEL10           (0x0AU) /* Level 10 */
#define _0B_TPU_PRIORITY_LEVEL11           (0x0BU) /* Level 11 */
#define _0C_TPU_PRIORITY_LEVEL12           (0x0CU) /* Level 12 */
#define _0D_TPU_PRIORITY_LEVEL13           (0x0DU) /* Level 13 */
#define _0E_TPU_PRIORITY_LEVEL14           (0x0EU) /* Level 14 */
#define _0F_TPU_PRIORITY_LEVEL15           (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif

