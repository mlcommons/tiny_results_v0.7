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

#include "ee_util.h"

/**
 * @brief convert a hexidecimal string to a signed long
 * will not produce or process negative numbers except
 * to signal error.
 *
 * @param hex without decoration, case insensitive.
 *
 * @return -1 on error, or result (max (sizeof(long)*8)-1 bits)
 *
 */
long
ee_hexdec(char *hex)
{
    char c;
    long dec = 0;
    long ret = 0;

    while (*hex && ret >= 0)
    {
        c = *hex++;
        if (c >= '0' && c <= '9')
        {
            dec = c - '0';
        }
        else if (c >= 'a' && c <= 'f')
        {
            dec = c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'F')
        {
            dec = c - 'A' + 10;
        }
        else
        {
            return -1;
        }
        ret = (ret << 4) + dec;
    }
    return ret;
}
