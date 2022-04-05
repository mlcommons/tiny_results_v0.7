/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : st_port.c
  * @brief          : Helper routines
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */


#include"st_port.h"
#include "internally_implemented.h"
#include "submitter_implemented.h"

static char eembc_char=0;

float Deq(int8_t value, float scale, int8_t zero_point)
{
	float re =0.0;
	re = (float)((value - zero_point) * scale);
  return re;
}

int8_t QuantizeFloatToInt8(float value, float scale, int zero_point)
{
	//int8_t re=0;
  int8_t result = round(value / scale) + zero_point;
  if (result < INT8_MIN)
  {
    result = INT8_MIN;
  }
  if (result > INT8_MAX)
  {
    result = INT8_MAX;
  }
  //re =(int8_t)result;
  return result;
}

//=============================================== AI  INIT ================================================
ai_handle act_addr[] = {pool0};
void AI_Init()
{
  /* Create and initialize an instance of the model */
  ai_error err;
  err = ai_network_create_and_init(&network, act_addr, NULL);

  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);

  /* IO buffers ---*/

  #if !defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
  AI_ALIGNED(4) ai_i8 data_in_1[AI_NETWORK_IN_1_SIZE_BYTES];
  ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
  data_in_1
  };
  #else
  ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
  NULL
  };
  #endif

  #if !defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
  AI_ALIGNED(4) ai_i8 data_out_1[AI_NETWORK_OUT_1_SIZE_BYTES];
  ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
  data_out_1
  };
  #else
  ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
  NULL
  };
  #endif

#if defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-inputs" option is used, memory buffer can be
   *  used from the activations buffer. This is not mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
  data_ins[idx] = ai_input[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
    ai_input[idx].data = data_ins[idx];
  }
#endif

#if defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-outputs" option is used, memory buffer can be
   *  used from the activations buffer. This is no mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
  data_outs[idx] = ai_output[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
  ai_output[idx].data = data_outs[idx];
  }
#endif
}

void AI_Run(int8_t *pIn, int8_t *pOut)
{
  ai_i32 batch;
  ai_input[0].data = AI_HANDLE_PTR(pIn);
  ai_output[0].data = AI_HANDLE_PTR(pOut);
  batch = ai_network_run(network, ai_input, ai_output);
}
//============================================================================================================

void US_Delay(uint8_t us) // microseconds
{
    int startTick = __HAL_TIM_GET_COUNTER(&htim5);
    while (__HAL_TIM_GET_COUNTER(&htim5) - startTick < us);
}

void clear_Timestamp_Port(void)
{
    __HAL_RCC_GPIOG_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);
}
void set_Timestamp_Port(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);
    __HAL_RCC_GPIOG_CLK_DISABLE();
}

void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */
#if EE_CFG_ENERGY_MODE == 0
  	  HAL_UART_Receive_IT(&huart3, (uint8_t *)&eembc_char, 1);
  	  ee_serial_callback(eembc_char);
#endif
  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles LPUART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN LPUART1_IRQn 0 */

  /* USER CODE END LPUART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN LPUART1_IRQn 1 */
  #if EE_CFG_ENERGY_MODE == 1
	  HAL_UART_Receive_IT(&huart1, (uint8_t *)&eembc_char, 1);
	  ee_serial_callback(eembc_char);
  #endif
  /* USER CODE END LPUART1_IRQn 1 */
}

void rxSetInterrupt(uint8_t *pData)
{
#if EE_CFG_ENERGY_MODE == 0
	if (HAL_UART_Receive_IT(&huart3, pData, 1) != HAL_OK)
	{
		Error_Handler();
	}
#else
	if (HAL_UART_Receive_IT(&huart1, pData, 1) != HAL_OK)
	{
		Error_Handler();
	}
#endif
}
void rxInit(void)
{
	rxSetInterrupt((uint8_t *)&eembc_char);
}

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if (UartHandle->Instance == USART2)
	{
		ee_serial_callback(g_rxByte);
		rxSetInterrupt((uint8_t *)&g_rxByte);
	}
}
*/
