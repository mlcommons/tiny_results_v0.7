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

#include "profile/th_api/th_profile.h"

/* From profile/ee_buffer.c */
//extern uint8_t *gp_buff;
//extern size_t   g_buff_size;
//extern size_t   g_buff_pos;

extern float    EEMBC_reslut[20];

/**
 * Copy the octet buffer `gp_buff` into your input tensor in the proper
 * format.
 */
void
th_load_tensor(void)
{
//#warning "th_load_tensor() not implemented"
    EEMBC_LoadTensor();
}

/**
 * Perform a single inference.
 */
void
th_infer(void)
{
//#warning "th_infer() not implemented"
    EEMBC_Inference();
}

void
th_results(void)
{
    float *results  = NULL;
    size_t nresults = 0;
    /* USER CODE 1 BEGIN */
    /* Populate results[] and n from the fp32 prediction tensor. */
//#warning "th_results() not implemented"
    nresults = EEMBC_Result();
    results = EEMBC_reslut;

    /* USER CODE 1 END */
    /**
     * The results need to be printed back in exactly this format; if easier
     * to just modify this loop than copy to results[] above, do that.
     */
    th_printf("m-results-[");
    for (size_t i = 0; i < nresults; i++)
    {
        /* N.B. Be sure %f is enabled in SDK */
//        th_printf("%.3f", results[i]);
//        th_printf("%d", (int)results[i]);
    	th_printf("%d", (int)results[i]);
    	th_printf(".");
    	th_printf("%03d", (int)((int)(results[i]*1000)-((int)results[i]*1000)));

        if (i < (nresults - 1))
        {
            th_printf(",");
        }
    }
    th_printf("]\r\n");
}
