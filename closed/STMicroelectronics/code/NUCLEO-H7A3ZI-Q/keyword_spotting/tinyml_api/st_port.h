/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : st_port.h
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


#ifndef INC_ST_PORT_H_
#define INC_ST_PORT_H_

#include <limits.h>
#include <math.h>
#include "main.h"
#include "ai_platform.h"
#include "network.h"
#include "network_data.h"
#include <stdio.h>

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
TIM_HandleTypeDef htim5;


//==============================================================================================================
int8_t aiInData_int[AI_NETWORK_IN_1_SIZE];
int8_t aiOutData[AI_NETWORK_OUT_1_SIZE];

uint8_t pool0[AI_NETWORK_DATA_ACTIVATION_1_SIZE];
ai_handle network ;

ai_buffer* ai_input;
ai_buffer* ai_output;

void AI_Init();
void AI_Run(int8_t *pIn, int8_t *pOut);

//============================================================================================================

float Deq(int8_t value, float scale, int8_t zero_point);
int8_t QuantizeFloatToInt8(float value, float scale, int zero_point);



void clear_Timestamp_Port(void);
void set_Timestamp_Port(void);
void rxInit(void);

void US_Delay(uint8_t us);

#endif /* INC_ST_PORT_H_ */
