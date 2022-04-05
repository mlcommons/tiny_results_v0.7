/*
Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file reflects a modified version of th_lib from EEMBC. The reporting logic
in th_results is copied from the original in EEMBC.

This file has been modified by Silicon Labs 
==============================================================================*/
/// \file
/// \brief C++ implementations of submitter_implemented.h

#include "api/submitter_implemented.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "api/internally_implemented.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "util/quantization_helpers.h"
#include "vww/vww_model_settings.h"
#include "sl_sleeptimer.h"
#include "sl_tflite_micro_model.h"

#if EE_CFG_ENERGY_MODE
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_usart.h"
#include "sl_udelay.h"
#include "sl_iostream_usart_vcom_config.h"
extern "C" {
  // This file doesn't declare extern C internally 
  #include "sl_board_control.h"
}
#endif 

/*******************************************************************************
 ***********************        BENCHMARK API       ****************************
 ******************************************************************************/

//  Instance Pointers  
static tflite::ErrorReporter *error_reporter = nullptr;
static tflite::MicroInterpreter *micro_interpreter = nullptr;

constexpr int kTensorArenaSize = 100 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];
static uint8_t input_quantized[kVwwInputSize];

// Pointers to model input and output tensors 
static volatile TfLiteTensor* input_tensor = nullptr;
static TfLiteTensor* output_tensor = nullptr;

#define TIMESTAMP_PORT gpioPortB
#define TIMESTAMP_PIN  0


// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
  size_t bytes = ee_get_buffer(reinterpret_cast<uint8_t *>(input_quantized),
                               kVwwInputSize * sizeof(uint8_t));
  if (bytes != kVwwInputSize) {
    th_printf("Input db has %d elements, expected %d\n", bytes,
              kVwwInputSize);
    return;
  }
  uint16_t i = 0;
  for(i=0; i < kVwwInputSize; i++)
  {
   if(input_quantized[i]<=127){
      input_tensor->data.int8[i] = ((int8_t)input_quantized[i]) - 128;
   } else {
      input_tensor->data.int8[i] = (int8_t)(input_quantized[i] - 128);
    }
  }
}


// Add to this method to return real inference results.
void th_results() {
  /**
   * The results need to be printed back in exactly this format
   */
  th_printf("m-results-[");

  for (size_t i = 0; i < kCategoryCount; i++) {
    float converted = DequantizeInt8ToFloat(output_tensor->data.int8[i], 
          output_tensor->params.scale, output_tensor->params.zero_point);

    th_printf("%.3f", converted);
    if (i < (kCategoryCount - 1)) {
      th_printf(",");
    }
  }
  th_printf("]\r\n");
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() { 
  TfLiteStatus invoke_status = micro_interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    th_printf("Invoke Failed");
    return;
  }
}

/// \brief optional API.
void th_final_initialize(void) {
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Instantiate model from char array in autogen/sl_tflite_micro_model.h
  const tflite::Model* model = tflite::GetModel(default_model_array);

  // Check model schema version 
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    th_printf("Incorrect Schema version");
    return; 
  }

  // Load operations required to run model - fetched from Netron
  static tflite::MicroMutableOpResolver<6> micro_op_resolver(error_reporter);
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddAveragePool2D();

  // Instantiate interpreter
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  micro_interpreter = &static_interpreter;

  // Allocate memory from tensor_arena for the model's tensors.
  if (micro_interpreter->AllocateTensors() != kTfLiteOk) {
    return; 
  }

  // Obtain pointers to input and output tensors 
  input_tensor = micro_interpreter->input(0);
  output_tensor = micro_interpreter->output(0);

  if (input_tensor->bytes / sizeof(int8_t) != kVwwInputSize) {
    th_printf("Input db has %d elements, expected %d\n", input_tensor->bytes / sizeof(int8_t), kVwwInputSize);
  }
}

void th_pre() {}
void th_post() {}

/*******************************************************************************
 ***************************    DUT BASIC API   ******************************
 ******************************************************************************/

void th_command_ready(char volatile *p_command) {
  p_command = p_command;
  ee_serial_command_parser_callback((char *)p_command);
}

// th_libc implementations.
int th_strncmp(const char *str1, const char *str2, size_t n) {
  return strncmp(str1, str2, n);
}

char *th_strncpy(char *dest, const char *src, size_t n) {
  return strncpy(dest, src, n);
}

// For some reason armgcc 10 does not declare this in string.h
extern "C" size_t strnlen (const char *, size_t);

size_t th_strnlen(const char *str, size_t maxlen) {
  return strnlen(str, maxlen);
}

char *th_strcat(char *dest, const char *src) { return strcat(dest, src); }

char *th_strtok(char *str1, const char *sep) { return strtok(str1, sep); }

int th_atoi(const char *str) { return atoi(str); }

void *th_memset(void *b, int c, size_t len) { return memset(b, c, len); }

void *th_memcpy(void *dst, const void *src, size_t n) {
  return memcpy(dst, src, n);
}

/* N.B.: Many embedded *printf SDKs do not support all format specifiers. */
int th_vprintf(const char *format, va_list ap) { return vprintf(format, ap); }
void th_printf(const char *p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  (void)th_vprintf(p_fmt, args); /* ignore return */
  va_end(args);
}

char th_getchar() { return getchar(); }


/**
 * Set up a serialport at 9600 baud to use for communication to the
 * host system if it hasn't already been done, otherwise it is OK to leave this
 * blank.
 */
void th_serialport_initialize(void) {
#if EE_CFG_ENERGY_MODE == 1
  // In energy mode, we talk to the DUT through the IO Manager at 9600 baud 
  // Disable VCOM and set baudrate to 9600
  sl_board_disable_vcom();
  USART_BaudrateAsyncSet(SL_IOSTREAM_USART_VCOM_PERIPHERAL, 0, 9600, usartOVS16);
#else
  // In performance mode, we talk directly to the DUT at 115200 baud 
  // Use default settings
#endif
}

/**
 * In "energy mode", generate a falling edge. Since GPIO pin is
 * OPEN-DRAIN it is OK to float and let the pullup resistor drive. In
 * "performance mode", a timer is needed. Depending on the benchmark, the
 * resolution may be as large as 1ms (1000us). In this case, it is OK for the
 * DUT to supply a 1ms timer, just multiply by 1000. Either way, the user must
 * set the microSeconds value in "performance mode".
 *
 * NOTE: The hold time for the GPIO is 1us
 */
void th_timestamp(void) {
#if EE_CFG_ENERGY_MODE == 1
/* USER CODE 1 BEGIN */
/* Step 1. Pull pin low */
/* Step 2. Hold low for at least 1us */
/* Step 3. Release driver */
  GPIO_PinOutClear(TIMESTAMP_PORT, TIMESTAMP_PIN); 
  sl_udelay_wait(1);
  GPIO_PinOutSet(TIMESTAMP_PORT, TIMESTAMP_PIN);
/* USER CODE 1 END */
#else
  uint32_t ticks;
  uint32_t microSeconds = 0ul;
  /* USER CODE 2 BEGIN */
  ticks = sl_sleeptimer_get_tick_count();
  microSeconds = sl_sleeptimer_tick_to_ms(ticks);
  microSeconds = microSeconds * 1000;
  /* USER CODE 2 END */
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP, (uint32_t)microSeconds);
#endif
}

/**
 * PORTME: If using energy mode, set up an open-drain GPIO (if it hasn't been
 * done already by the IDE boilerplate). If using performance mode, prepare a
 * timer if necessary.
 */
void th_timestamp_initialize(void) {
  /* USER CODE 1 BEGIN */
  #if EE_CFG_ENERGY_MODE
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(TIMESTAMP_PORT, TIMESTAMP_PIN, gpioModeWiredAnd, 0);
  #endif
  /* USER CODE 1 END */

  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP_MODE);
  /* Always call the timestamp on initialize so that the open-drain output
     is set to "1" (so that we catch a falling edge) */
  th_timestamp();
}