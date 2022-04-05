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

#include "ee_profile.h"

arg_claimed_t ee_buffer_parse(char *command);
void          ee_infer(size_t n, size_t w);

/* These functions are expected from profile/th_api/profile.c */
void th_load_tensor(void);
void th_infer(void);
void th_results(void);

arg_claimed_t
ee_profile_parse(char *command)
{
    char *p_next; /* strtok already primed from ee_main.c */

    if (th_strncmp(command, "profile", EE_CMD_SIZE) == 0)
    {
        th_printf("m-profile-[%s]\r\n", EE_FW_VERSION);
        th_printf("m-model-[%s]\r\n", TH_MODEL_VERSION);
    }
    else if (th_strncmp(command, "help", EE_CMD_SIZE) == 0)
    {
        th_printf("%s\r\n", EE_FW_VERSION);
        th_printf("\r\n");
        /* These are the three common functions for all IoTConnect f/w. */
        th_printf("help         : Print this information\r\n");
        th_printf("name         : Print the name of the device\r\n");
        th_printf("timestsamp   : Generate a timetsamp\r\n");
        /* These are profile-specific commands. */
        th_printf("db SUBCMD    : Manipulate a generic byte buffer\r\n");
        th_printf("  load N     : Allocate N bytes and set load counter\r\n");
        th_printf("  db HH[HH]* : Load 8-bit hex byte(s) until N bytes\r\n");
        th_printf("  print [N=16] [offset=0]\r\n");
        th_printf("             : Print N bytes at offset as hex\r\n");
        th_printf("infer N [W=0]: Load input, execute N inferences after W warmup loops\r\n");
        th_printf("results      : Return the result fp32 vector\r\n");
    }
    else if (ee_buffer_parse(command) == EE_ARG_CLAIMED)
    {
    }
    else if (th_strncmp(command, "infer", EE_CMD_SIZE) == 0)
    {
        size_t n = 1;
        size_t w = 10;
        int    i;

        /* Check for inference iterations */
        p_next = th_strtok(NULL, EE_CMD_DELIMITER);
        if (p_next)
        {
            i = th_atoi(p_next);
            if (i <= 0)
            {
                th_printf("e-[Inference iterations must be >0]\r\n");
                return EE_ARG_CLAIMED;
            }
            n = (size_t)i;
            /* Check for warmup iterations */
            p_next = th_strtok(NULL, EE_CMD_DELIMITER);
            if (p_next)
            {
                i = th_atoi(p_next);
                if (i < 0)
                {
                    th_printf("e-[Inference warmup must be >=0]\r\n");;
                    return EE_ARG_CLAIMED;
                }
                w = (size_t)i;
            }
        }

        ee_infer(n, w);
    }
    else if (th_strncmp(command, "results", EE_CMD_SIZE) == 0)
    {
        th_results();
    }
    else
    {
        return EE_ARG_UNCLAIMED;
    }
    return EE_ARG_CLAIMED;
}

/**
 * Inference without feature engineering. The inpput tensor is expected to
 * have been loaded from the buffer via the th_load_tensor() function, which in
 * turn was loaded from the interface via `db` commands.
 *
 * For testing, you can pre-load known-good data into the buffer during the
 * th_final_initialize() function.
 *
 */
void
ee_infer(size_t n, size_t n_warmup)
{
    th_load_tensor(); /* if necessary */
    th_printf("m-warmup-start-%d\r\n", n_warmup);
    while (n_warmup-- > 0)
    {
        th_infer(); /* call the API inference function */
    }
    th_printf("m-warmup-done\r\n");
    th_printf("m-infer-start-%d\r\n", n);
    th_timestamp();
    th_pre();
    while (n-- > 0)
    {
        th_infer(); /* call the API inference function */
    }
    th_post();
    th_timestamp();
    th_printf("m-infer-done\r\n");
    th_results();
}

