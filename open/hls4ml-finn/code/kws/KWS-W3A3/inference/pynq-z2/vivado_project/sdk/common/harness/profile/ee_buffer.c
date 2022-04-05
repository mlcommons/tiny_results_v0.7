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

/**
 * These varaibles describe the generic buffer. The buffer is created via
 * LIBC malloc()/free().
 */
uint8_t *gp_buff     = NULL;
size_t   g_buff_size = 0u;
size_t   g_buff_pos  = 0u;

arg_claimed_t
ee_buffer_parse(char *p_command)
{
    char *p_next;
    if (th_strncmp(p_command, "db", EE_CMD_SIZE) != 0)
    {
        return EE_ARG_UNCLAIMED;
    }

    p_next = th_strtok(NULL, EE_CMD_DELIMITER);

    if (p_next == NULL)
    {
        th_printf("e-[Command 'db' requires a subcommand]\r\n");
    }
    else if (th_strncmp(p_next, "load", EE_CMD_SIZE) == 0)
    {
        p_next = th_strtok(NULL, EE_CMD_DELIMITER);

        if (p_next == NULL)
        {
            th_printf("e-[Command 'db load' requires the # of bytes]\r\n");
        }
        else
        {
            g_buff_size = (size_t)atoi(p_next);
            if (g_buff_size == 0)
            {
                th_printf("e-[Command 'db load' must be >0 bytes]\r\n");
            }
            else
            {
                g_buff_pos = 0;
                if (gp_buff != NULL)
                {
                    free(gp_buff);//th_
                    gp_buff = NULL;
                }
                gp_buff = (void *)th_malloc(g_buff_size);
                if (gp_buff == NULL)
                {
                    th_printf("e-[Unable to malloc %u bytes]\r\n", g_buff_size);
                }
                else
                {
                    th_printf("m-[Expecting %d bytes]\r\n", g_buff_size);
                }
            }
        }
    }
    else if (th_strncmp(p_next, "print", EE_CMD_SIZE) == 0)
    {
        if (gp_buff == NULL)
        {
            th_printf("e-[Buffer not allocated]\r\n");
        }
        else
        {
            size_t       i   = 0;
            const size_t max = 8;
            for (; i < g_buff_size; ++i)
            {
                if ((i + max) % max == 0 || i == 0)
                {
                    th_printf("m-buffer-");
                }
                /* N.B. Not every `printf` supports the spacing prefix! */
                th_printf("%02x", gp_buff[i]);
                if (((i + 1) % max == 0) || ((i + 1) == g_buff_size))
                {
                    th_printf("\r\n");
                }
                else
                {
                    th_printf("-");
                }
            }
            if (i % max != 0)
            {
                th_printf("\r\n");
            }
        }
    }
    else
    {
        size_t numbytes;
        char   test[3];
        long   res;

        if (gp_buff == NULL)
        {
            th_printf("e-[Buffer not allocated]\r\n");
            return EE_ARG_CLAIMED;
        }

        /* Two hexdigits per byte */
        numbytes = th_strnlen(p_next, EE_CMD_SIZE);

        if ((numbytes & 1) != 0)
        {
            th_printf("e-[Insufficent number of hex digits]\r\n");
            return EE_ARG_CLAIMED;
        }
        test[2] = 0;
        for (size_t i = 0; i < numbytes;)
        {
            test[0] = p_next[i++];
            test[1] = p_next[i++];
            res     = ee_hexdec(test);
            if (res < 0)
            {
                th_printf("e-[Invalid hex digit '%s']\r\n", test);
                return EE_ARG_CLAIMED;
            }
            else
            {
                gp_buff[g_buff_pos] = (uint8_t)res;
                g_buff_pos++;
                if (g_buff_pos == g_buff_size)
                {
                    th_printf("m-load-done\r\n");
                    /* Disregard the remainder of the digits when done. */
                    return EE_ARG_CLAIMED;
                }
            }
        }
    }
    return EE_ARG_CLAIMED;
}
