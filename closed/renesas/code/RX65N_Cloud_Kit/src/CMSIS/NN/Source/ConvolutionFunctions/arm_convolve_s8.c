/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software.
 *
 * rmpa instruction is applied for RX devices
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/* ----------------------------------------------------------------------
 * Project:      CMSIS NN Library
 * Title:        arm_convolve_s8.c
 * Description:  s8 version of convolution using symmetric quantization.
 *
 * $Date:        January 26, 2021
 * $Revision:    V.2.0.4
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup NNConv
 * @{
 */

/*
 * Basic s8 convolution function.
 *
 * Refer header file for details. Optimal use case for the DSP/MVE implementation is when input and output channels
 * are multiples of 4 or atleast greater than 4.
 *
 */

#if defined (RX_DSP_INT8)
q31_t a_in_offset[64];
#endif

arm_status arm_convolve_s8(const cmsis_nn_context *ctx,
                           const cmsis_nn_conv_params *conv_params,
                           const cmsis_nn_per_channel_quant_params *quant_params,
                           const cmsis_nn_dims *input_dims,
                           const q7_t *input_data,
                           const cmsis_nn_dims *filter_dims,
                           const q7_t *filter_data,
                           const cmsis_nn_dims *bias_dims,
                           const int32_t *bias_data,
                           const cmsis_nn_dims *output_dims,
                           q7_t *output_data)
{
(void)bias_dims;
    q15_t *buffer_a = (q15_t *)ctx->buf;

    const uint16_t input_batches = input_dims->n;
    const uint16_t input_x = input_dims->w;
    const uint16_t input_y = input_dims->h;
    const uint16_t input_ch = input_dims->c;
    const uint16_t kernel_x = filter_dims->w;
    const uint16_t kernel_y = filter_dims->h;
    const uint16_t output_x = output_dims->w;
    const uint16_t output_y = output_dims->h;
    const uint16_t output_ch = output_dims->c;

    const uint16_t pad_x = conv_params->padding.w;
    const uint16_t pad_y = conv_params->padding.h;
    const uint16_t stride_x = conv_params->stride.w;
    const uint16_t stride_y = conv_params->stride.h;

    const int32_t input_offset = conv_params->input_offset;
    const int32_t out_offset = conv_params->output_offset;
    const int32_t out_activation_min = conv_params->activation.min;
    const int32_t out_activation_max = conv_params->activation.max;
    int32_t *output_mult = quant_params->multiplier;
    int32_t *output_shift = quant_params->shift;

    int i_batch;
    for (i_batch = 0; i_batch < input_batches; i_batch++)
    {
#if defined(ARM_MATH_MVEI)
        /* Generate upto four columns from the input tensor a GEMM computation */
        q7_t *im2col_buf = (q7_t *)buffer_a;
        q7_t *out = output_data;
        int32_t buffer_fill_cnt = 0;
        int32_t padded = 0;
        const int32_t num_elem = kernel_x * kernel_y * input_ch;

        /* This part implements the im2col function */
        for (int i_out_y = 0; i_out_y < output_y; i_out_y++)
        {
            for (int i_out_x = 0; i_out_x < output_x; i_out_x++)
            {
                for (int i_ker_y = i_out_y * stride_y - pad_y; i_ker_y < i_out_y * stride_y - pad_y + kernel_y;
                     i_ker_y++)
                {
                    for (int i_ker_x = i_out_x * stride_x - pad_x; i_ker_x < i_out_x * stride_x - pad_x + kernel_x;
                         i_ker_x++)
                    {
                        if (i_ker_y < 0 || i_ker_y >= input_y || i_ker_x < 0 || i_ker_x >= input_x)
                        {
                            memset(im2col_buf, (int8_t)-input_offset, sizeof(q7_t) * input_ch);
                            padded = 1;
                        }
                        else
                        {
                            arm_memcpy_q7(im2col_buf, input_data + (i_ker_y * input_x + i_ker_x) * input_ch, input_ch);
                        }
                        im2col_buf += input_ch;
                    }
                }

                buffer_fill_cnt++;

                /* Computation is filed for every 4 columns */
                if (buffer_fill_cnt == 4 && (padded == 0))
                {
                    buffer_fill_cnt = 0;
                    for (int i_out_ch = 0; i_out_ch < output_ch; i_out_ch++)
                    {
                        int32_t sum_row;
                        int32_t acc[4];

                        (void)arm_nn_mat_mul_core_4x_s8(
                            num_elem, num_elem, (q7_t *)buffer_a, filter_data + num_elem * i_out_ch, &sum_row, acc);
                        int32x4_t s_offset = vdupq_n_s32(sum_row);

                        int32x4_t res = vldrwq_s32(acc);
                        s_offset = vmulq_n_s32(s_offset, input_offset);
                        if (bias_data)
                        {
                            res = vaddq_n_s32(res, bias_data[i_out_ch]);
                        }
                        res = vaddq_s32(res, s_offset);
                        res = arm_requantize_mve(res, output_mult[i_out_ch], output_shift[i_out_ch]);
                        res = vaddq_n_s32(res, out_offset);

                        res = vmaxq_s32(res, vdupq_n_s32(out_activation_min));
                        res = vminq_s32(res, vdupq_n_s32(out_activation_max));

                        const uint32x4_t scatter_offset = {0, output_ch, output_ch * 2, output_ch * 3};
                        vstrbq_scatter_offset_s32(out, scatter_offset, res);
                        out++;
                    }
                    out += (3 * output_ch);
                    im2col_buf = (q7_t *)buffer_a;
                }
                else if (buffer_fill_cnt == 4 && (padded != 0))
                {
                    buffer_fill_cnt = 0;
                    out = arm_nn_mat_mult_s8(filter_data,
                                             (q7_t *)buffer_a,
                                             output_ch,
                                             4,
                                             output_shift,
                                             output_mult,
                                             out_offset,
                                             input_offset,
                                             0,
                                             out_activation_min,
                                             out_activation_max,
                                             num_elem,
                                             bias_data,
                                             out);

                    im2col_buf = (q7_t *)buffer_a;
                    padded = 0;
                }
            }
        }
        /* Handle left over columns */
        if (buffer_fill_cnt != 0)
        {
            out = arm_nn_mat_mult_s8(filter_data,
                                     (q7_t *)buffer_a,
                                     output_ch,
                                     buffer_fill_cnt,
                                     output_shift,
                                     output_mult,
                                     out_offset,
                                     input_offset,
                                     0,
                                     out_activation_min,
                                     out_activation_max,
                                     num_elem,
                                     bias_data,
                                     out);
        }

#elif defined(ARM_MATH_DSP)
        int32_t i_out_y, i_out_x, i_ker_y, i_ker_x;

        /* Generate two columns from the input tensor a GEMM computation */
        q15_t *two_column_buf = buffer_a;
        q7_t *out = output_data;

        /* This part implements the im2col function */
        for (i_out_y = 0; i_out_y < output_y; i_out_y++)
        {
            for (i_out_x = 0; i_out_x < output_x; i_out_x++)
            {
                for (i_ker_y = i_out_y * stride_y - pad_y; i_ker_y < i_out_y * stride_y - pad_y + kernel_y; i_ker_y++)
                {
                    for (i_ker_x = i_out_x * stride_x - pad_x; i_ker_x < i_out_x * stride_x - pad_x + kernel_x;
                         i_ker_x++)
                    {
                        if (i_ker_y < 0 || i_ker_y >= input_y || i_ker_x < 0 || i_ker_x >= input_x)
                        {
                            /* Filling 0 for out-of-bound paddings */
                            memset(two_column_buf, 0, sizeof(q15_t) * input_ch);
                        }
                        else
                        {
                            /* Copying the pixel data to column */
                            arm_q7_to_q15_with_offset(input_data + (i_ker_y * input_x + i_ker_x) * input_ch,
                                                      two_column_buf,
                                                      input_ch,
                                                      input_offset);
                        }
                        two_column_buf += input_ch;
                    }
                }

                /* Computation is filed for every 2 columns */
                if (two_column_buf == buffer_a + 2 * input_ch * kernel_y * kernel_x)
                {
                    out = arm_nn_mat_mult_kernel_s8_s16(filter_data,
                                                        buffer_a,
                                                        output_ch,
                                                        output_shift,
                                                        output_mult,
                                                        out_offset,
                                                        out_activation_min,
                                                        out_activation_max,
                                                        input_ch * kernel_y * kernel_x,
                                                        bias_data,
                                                        out);

                    /* counter reset */
                    two_column_buf = buffer_a;
                }
            }
        }

        /* left-over because odd number of output pixels */
        if (two_column_buf != buffer_a)
        {
            const q7_t *ker_a = filter_data;
            int i;

            for (i = 0; i < output_ch; i++)
            {
                /* Load the accumulator with bias first */
                q31_t sum = 0;
                if (bias_data)
                {
                    sum = bias_data[i];
                }

                /* Point to the beginning of the im2col buffer where the input is available as a rearranged column */
                const q15_t *ip_as_col = buffer_a;

                /* 4 multiply and accumulates are done in one loop. */
                uint16_t col_count = (input_ch * kernel_y * kernel_x) >> 2;

                while (col_count)
                {
                    q31_t ker_a1, ker_a2;
                    q31_t ip_b1, ip_b2;

                    ker_a = read_and_pad(ker_a, &ker_a1, &ker_a2);

                    ip_b1 = arm_nn_read_q15x2_ia(&ip_as_col);
                    sum = __SMLAD(ker_a1, ip_b1, sum);
                    ip_b2 = arm_nn_read_q15x2_ia(&ip_as_col);
                    sum = __SMLAD(ker_a2, ip_b2, sum);

                    col_count--;
                }
                /* Handle left over mac */
                col_count = input_ch * kernel_y * kernel_x & 0x3;
                while (col_count)
                {
                    q7_t ker_a1 = *ker_a++;
                    q15_t ip_b1 = *ip_as_col++;
                    sum += ker_a1 * ip_b1;
                    col_count--;
                }

                sum = arm_nn_requantize(sum, output_mult[i], output_shift[i]);
                sum += out_offset;
                sum = MAX(sum, out_activation_min);
                sum = MIN(sum, out_activation_max);
                *out++ = (q7_t)sum;
            }
        }
#elif defined(RX_DSP_INT8)

        int32_t i_out_y, i_out_x, i_ker_y, i_ker_x;

        /* Generate two columns from the input tensor a GEMM computation */
        q7_t *out = output_data;

        q7_t *column_buf = (q7_t*)buffer_a;


        const uint16_t num_col_a = input_ch * kernel_y * kernel_x;

        // input_offset
        int i = 0;
        q31_t t_offset_base;
        const q7_t *ip_a0x = filter_data;

        while(i < output_ch)
        {
            uint16_t col_count = num_col_a;
            t_offset_base = 0;
            while(col_count) 
            {
                t_offset_base += (q31_t)(*ip_a0x++);
                col_count--;
            }
            a_in_offset[i] = t_offset_base * input_offset;
            i++;
        }


        /* This part implements the im2col function */
        for (i_out_y = 0; i_out_y < output_y; i_out_y++)
        {
            for (i_out_x = 0; i_out_x < output_x; i_out_x++)
            {
                for (i_ker_y = i_out_y * stride_y - pad_y; i_ker_y < i_out_y * stride_y - pad_y + kernel_y; i_ker_y++)
                {
                    for (i_ker_x = i_out_x * stride_x - pad_x; i_ker_x < i_out_x * stride_x - pad_x + kernel_x;
                         i_ker_x++)
                    {
                        if (i_ker_y < 0 || i_ker_y >= input_y || i_ker_x < 0 || i_ker_x >= input_x)
                        {
                            /* Filling 0 for out-of-bound paddings */
                            memset(column_buf, (int8_t)-input_offset, sizeof(q7_t) * input_ch); // like MVE
                            // input_offset is always 128 in this model.
                        }
                        else
                        {
                            /* Copying the pixel data to column */
                            memcpy(column_buf, input_data + (i_ker_y * input_x + i_ker_x) * input_ch, input_ch);

                        }
                        column_buf += input_ch;
                    }
                }

                /* Computation is filed for every 1 column */
                const int32_t *out_shift = output_shift;
                const int32_t *out_mult = output_mult;
                const int16_t activation_min = out_activation_min;
                const int16_t activation_max = out_activation_max;
                const int32_t *bias = bias_data;
                const q7_t *ip_a0 = filter_data;
                const q31_t *in_offset = a_in_offset;

                int32_t row_count;
                row_count = output_ch;

                while(row_count)  
                {
                    const q7_t *ip_b0 = (q7_t*)buffer_a;

                    q31_t ch_0_out_0 = *bias++ + *in_offset++;

                    uint16_t col_count = num_col_a;
#if defined __CCRX__ || defined __CCRL__
					ch_0_out_0 = __rmpab(ch_0_out_0, col_count, (signed char *)ip_a0,(signed char *)ip_b0)

#else
                    asm inline ("mov.l %1, r1\n"
                                "mov.l %2, r2\n"
                                "mov.l %3, r3\n"
                                "mov.l %0, r4\n"
                                "rmpa.b\n"
                                "mov.l r4, %0\n"
                                "mov.l r1, %1\n"
                                : "+r" (ch_0_out_0), "+r" (ip_a0)
                                : "r" (ip_b0), "r" (col_count)
                                :  "r1", "r2", "r3", "r4", "r5", "r6", "cc", "memory");
#endif
                    ch_0_out_0 = arm_nn_requantize(ch_0_out_0, *out_mult++, *out_shift++);
                    ch_0_out_0 += out_offset;
                    ch_0_out_0 = MAX(ch_0_out_0, activation_min);
                    ch_0_out_0 = MIN(ch_0_out_0, activation_max);

                    *out++ = (q7_t)ch_0_out_0;

                    row_count--;
                }
                    /* counter reset */
                column_buf = (q7_t*)buffer_a;
            }
        }
#else
        /* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
        (void)buffer_a;
        int32_t i_out_ch, i_out_y, i_out_x, i_input_ch, i_ker_y, i_ker_x;
        int32_t conv_out;

        for (i_out_ch = 0; i_out_ch < output_ch; i_out_ch++)
        {
            for (i_out_y = 0; i_out_y < output_y; i_out_y++)
            {
                for (i_out_x = 0; i_out_x < output_x; i_out_x++)
                {
                    conv_out = 0;

                    const int32_t base_idx_y = stride_y * i_out_y - pad_y;
                    const int32_t base_idx_x = stride_x * i_out_x - pad_x;

                    const int32_t ker_y_start = MAX(0, -base_idx_y);
                    const int32_t ker_x_start = MAX(0, -base_idx_x);

                    const int32_t ker_y_end = MIN(kernel_y, input_y - base_idx_y);
                    const int32_t ker_x_end = MIN(kernel_x, input_x - base_idx_x);

                    for (i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                    {
                        for (i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                        {
                            const int32_t in_row = base_idx_y + i_ker_y;
                            const int32_t in_col = base_idx_x + i_ker_x;
                            for (i_input_ch = 0; i_input_ch < input_ch; i_input_ch++)
                            {
                                conv_out +=
                                    (input_data[(in_row * input_x + in_col) * input_ch + i_input_ch] + input_offset) *
                                    filter_data[i_out_ch * input_ch * kernel_y * kernel_x +
                                                (i_ker_y * kernel_x + i_ker_x) * input_ch + i_input_ch];
                            }
                        }
                    }
                    if (bias_data)
                    {
                        conv_out += bias_data[i_out_ch];
                    }
                    conv_out = arm_nn_requantize(conv_out, output_mult[i_out_ch], output_shift[i_out_ch]);
                    conv_out += out_offset;
                    conv_out = MAX(conv_out, out_activation_min);
                    conv_out = MIN(conv_out, out_activation_max);
                    output_data[i_out_ch + (i_out_y * output_x + i_out_x) * output_ch] = (int8_t)conv_out;
                }
            }
        }
#endif
        /* Advance to the next batch */
        input_data += (input_x * input_y * input_ch);
        output_data += (output_x * output_y * output_ch);
    }

    /* Return to application */
    return ARM_MATH_SUCCESS;
}

int32_t arm_convolve_s8_get_buffer_size(const cmsis_nn_dims *input_dims, const cmsis_nn_dims *filter_dims)
{
#if defined(ARM_MATH_DSP)
    return (2 * input_dims->c * filter_dims->w * filter_dims->h) * (int32_t)sizeof(int16_t);
#else
    (void)input_dims;
    (void)filter_dims;
    return 0;
#endif
}

/**
 * @} end of NNConv group
 */
