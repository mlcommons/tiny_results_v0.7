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

/**
 * These functions are needed by the main framework. If no LIBC
 * is provided, please implmenent these.
 *
 * TODO: This is a bit archaic, as modern SDKs provide nearly all of these.
 *       Is it time to finally remove LIBC abstraction?
 */

#include "monitor/th_api/th_libc.h"

#include "hal_data.h"

#define MAX_TX_BUFFER	(256)
char tx_buffer[MAX_TX_BUFFER];

extern char* model_input_ptr;
extern uint32_t model_input_size;

int
th_strncmp(const char *str1, const char *str2, size_t n)
{
    return strncmp(str1, str2, n);
}

char *
th_strncpy(char *dest, const char *src, size_t n)
{
    return strncpy(dest, src, n);
}

size_t
th_strnlen(const char *str, size_t maxlen)
{
    return strnlen(str, maxlen);
}

/*@-mayaliasunique*/
/*@-temptrans*/
char *
th_strcat(char *dest, const char *src)
{
    return strcat(dest, src);
}

char *
th_strstr(const char *str1, const char *str2)
{
    return strstr(str1, str2);
}

char *
th_strtok(char *str1, const char *sep)
{
    return strtok(str1, sep);
}

int
th_atoi(const char *str)
{
    return atoi(str);
}

void *
th_memset(void *b, int c, size_t len)
{
    return memset(b, c, len);
}

void *
th_memcpy(void *dst, const void *src, size_t n)
{
    return memcpy(dst, src, n);
}

void *
th_malloc(size_t size)
{
	if( size <= model_input_size)
	{
		return model_input_ptr;
	}
	return NULL;
//    return malloc(size);
}

void *
th_calloc(size_t count, size_t size)
{
    for(int i = 0; i < size; i++)
    {
    	model_input_ptr[i] = 0;
    }

    return model_input_ptr;
//    return calloc(count, size);
}

void
th_free(void *ptr)
{
//    free(ptr);
}

/* N.B.: Many embedded *printf SDKs do not support all format specifiers. */
int
th_vprintf(const char *format, va_list ap)
{
	vsprintf(tx_buffer, format, ap);

    R_SCI_UART_Write(&g_uart7_ctrl, (uint8_t *)tx_buffer, strnlen(tx_buffer, MAX_TX_BUFFER));
//    return vprintf(format, ap);
    return 0;
}
