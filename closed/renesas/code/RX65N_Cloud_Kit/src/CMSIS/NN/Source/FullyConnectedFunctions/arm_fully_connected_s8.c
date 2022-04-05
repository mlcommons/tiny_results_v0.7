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
 * Title:        arm_fully_connected_s8
 * Description:  Fully connected function compatible with TF Lite.
 *
 * $Date:        19. March 2021
 * $Revision:    V.3.0.0
 *
 * Target Processor:  Cortex-M and Cortex-A cores
 *
 * -------------------------------------------------------------------- */

#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup FC
 * @{
 */

/*
 * S8 basic fully-connected and matrix multiplication layer function for TensorFlow Lite
 *
 * Refer header file for details.
 *
 */
#if defined (RX_DSP_INT8)
q31_t outres3[640];
#endif

arm_status arm_fully_connected_s8(const cmsis_nn_context *ctx,
                                  const cmsis_nn_fc_params *fc_params,
                                  const cmsis_nn_per_tensor_quant_params *quant_params,
                                  const cmsis_nn_dims *input_dims,
                                  const q7_t *input,
                                  const cmsis_nn_dims *filter_dims,
                                  const q7_t *kernel,
                                  const cmsis_nn_dims *bias_dims,
                                  const int32_t *bias,
                                  const cmsis_nn_dims *output_dims,
                                  q7_t *output)
{
  (void)bias_dims;
  (void)ctx;
  (void)fc_params->filter_offset;

  int32_t batch_cnt = input_dims->n;
#if defined(RX_DSP_INT8)

  const q7_t *rhs = kernel;
  int32_t outres1 = fc_params->input_offset * fc_params->filter_offset * filter_dims->n;
  const int32_t loop_res1 = filter_dims->n - (filter_dims->n & 0x0f);

  for (int32_t rhs_rows_idx = 0; rhs_rows_idx < output_dims->c; rhs_rows_idx += 1)
  {
    int32_t tmp = 0;

    // Process by 16
    for (int32_t rhs_cols_idx = 0; rhs_cols_idx < loop_res1; rhs_cols_idx = rhs_cols_idx + 16)
    {
      tmp += rhs[rhs_cols_idx];
      tmp += rhs[rhs_cols_idx+1];
      tmp += rhs[rhs_cols_idx+2];
      tmp += rhs[rhs_cols_idx+3];
      tmp += rhs[rhs_cols_idx+4];
      tmp += rhs[rhs_cols_idx+5];
      tmp += rhs[rhs_cols_idx+6];
      tmp += rhs[rhs_cols_idx+7];
      tmp += rhs[rhs_cols_idx+8];
      tmp += rhs[rhs_cols_idx+9];
      tmp += rhs[rhs_cols_idx+10];
      tmp += rhs[rhs_cols_idx+11];
      tmp += rhs[rhs_cols_idx+12];
      tmp += rhs[rhs_cols_idx+13];
      tmp += rhs[rhs_cols_idx+14];
      tmp += rhs[rhs_cols_idx+15];
    }
    // Remaining process
    for (int32_t rhs_cols_idx = loop_res1; rhs_cols_idx < filter_dims->n; rhs_cols_idx++)
    {
      tmp += rhs[rhs_cols_idx];
    }
    outres3[rhs_rows_idx] = tmp * fc_params->input_offset;
    rhs += filter_dims->n;
  }
  //*********************************************************************//
  while (batch_cnt)
  {
    const q7_t *lhs = input;
    int32_t outres2 = 0;
    for (int32_t rhs_cols_idx = 0; rhs_cols_idx < loop_res1; rhs_cols_idx = rhs_cols_idx + 16)
    {
      outres2 += lhs[rhs_cols_idx];
      outres2 += lhs[rhs_cols_idx+1];
      outres2 += lhs[rhs_cols_idx+2];
      outres2 += lhs[rhs_cols_idx+3];
      outres2 += lhs[rhs_cols_idx+4];
      outres2 += lhs[rhs_cols_idx+5];
      outres2 += lhs[rhs_cols_idx+6];
      outres2 += lhs[rhs_cols_idx+7];
      outres2 += lhs[rhs_cols_idx+8];
      outres2 += lhs[rhs_cols_idx+9];
      outres2 += lhs[rhs_cols_idx+10];
      outres2 += lhs[rhs_cols_idx+11];
      outres2 += lhs[rhs_cols_idx+12];
      outres2 += lhs[rhs_cols_idx+13];
      outres2 += lhs[rhs_cols_idx+14];
      outres2 += lhs[rhs_cols_idx+15];
    }
    // Remaining process
    for (int32_t rhs_cols_idx = loop_res1; rhs_cols_idx < filter_dims->n; rhs_cols_idx++)
    {
      outres2 += lhs[rhs_cols_idx];
    }
    outres2 *= fc_params->filter_offset;

    for (int32_t rhs_rows_idx = 0; rhs_rows_idx < output_dims->c; rhs_rows_idx += 1)
    {
      rhs = kernel;
      lhs = input;
      q31_t res = 0;
      if (bias)
      {
          res = *bias++;
      }
      int32_t rhs_cols_idx = filter_dims->n;
#if defined __CCRX__ || defined __CCRL__
      res = __rmpab(res, rhs_cols_idx, (signed char *)lhs,(signed char *)rhs)

#else
      asm inline ("mov.l %1, r1\n"
      "mov.l %2, r2\n"
      "mov.l %3, r3\n"
      "mov.l %0, r4\n"
      "rmpa.b\n"
      "mov.l r4, %0\n"
            : "+r" (res)
      :  "r" (lhs), "r" (rhs), "r" (rhs_cols_idx)
      :  "r1", "r2", "r3", "r4", "r5", "r6", "cc", "memory");
#endif
      res += outres1 + outres2 + outres3[rhs_rows_idx];
      //********************************************************************//
      // Quantize down
      res = arm_nn_requantize(res, quant_params->multiplier, quant_params->shift);

      // Add offset
      res += fc_params->output_offset;

      // Clamp the result
      res = MAX(res, fc_params->activation.min);
      res = MIN(res, fc_params->activation.max);

      *output++ = (q7_t)res;

      kernel += filter_dims->n;
    }
    input += filter_dims->n;
    output += output_dims->c;
    batch_cnt--;
  }

#else
  while (batch_cnt)
  {
    arm_nn_vec_mat_mult_t_s8(input,
                              kernel,
                              bias,
                              output,
                              fc_params->input_offset,
                              0,
                              fc_params->output_offset,
                              quant_params->multiplier,
                              quant_params->shift,
                              filter_dims->n, /* col_dim or accum_depth */
                              output_dims->c, /* row_dim or output_depth */
                              fc_params->activation.min,
                              fc_params->activation.max);
    input += filter_dims->n;
    output += output_dims->c;
    batch_cnt--;
  }
#endif
  return (ARM_MATH_SUCCESS);
}

int32_t arm_fully_connected_s8_get_buffer_size(const cmsis_nn_dims *filter_dims)
{
  (void)filter_dims;
  return 0;
}

/**
 * @} end of FC group
 */
