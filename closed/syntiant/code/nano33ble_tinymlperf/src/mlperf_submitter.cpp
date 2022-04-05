/*
Copyright (C) EEMBC(R). All Rights Reserved

All EEMBC Benchmark Software are products of EEMBC and are provided under the
terms of the EEMBC Benchmark License Agreements. The EEMBC Benchmark Software
are proprietary intellectual properties of EEMBC and its Members and is
protected under all applicable laws, including all applicable copyright laws.

If you received this EEMBC Benchmark Software without having a currently
effective EEMBC Benchmark License Agreement, you must discontinue use.

Copyright 2020 The MLPerf Authors. All Rights Reserved.
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
==============================================================================*/
/// \file
/// \brief C++ implementations of submitter_implemented.h



#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>
#include <Wire.h>
#include <mbed.h>

#include "quantization_helpers.h"
#include "kws/kws_input_data.h"
#include "kws/kws_model_data.h"
#include "kws/kws_model_settings.h"
#include "mlperf_submitter.h"
#include "mlperf_internal.h"


using namespace mbed;


static uint32_t mbin_toggle_value = 0x80;

extern struct syntiant_ndp_device_s *ndp;

DigitalOut timestampPin(digitalPinToPinName(D7)); // we need a pin to strobe to mark time for the energy test
constexpr int kTensorArenaSize = 64 * 1024;

// TODO remove this after debugging the bytes elemented problem
extern size_t g_buff_pos;


#define DBG_LOG_LEN 4096
char g_dblog[DBG_LOG_LEN];  // print dbg text to here instead of over serial port
int g_dblg_pos = 0;
void clear_dbg_buf() {
  for(int i=0;i<DBG_LOG_LEN;i++) {
    g_dblog[i] = 0;
  }
  g_dblg_pos = 0;
}
// usage: g_dblg_pos += sprintf(g_dblog+g_dblg_pos, <format_str>, <args>);

void printrst_dbg_buf() {
  // prints g_dblog and resets the position counter
  int i;
  Serial1.println("=== DBG Log ===");
  for(i=0; i<g_dblg_pos; i++){
    if(g_dblog[i] == 0)
      Serial1.println("");
    else
      Serial1.print(g_dblog[i]);
  }
  Serial1.println("=== End DBG Log ===");
  // clear_dbg_buf(); // needed ??
  g_dblg_pos = 0;
}

// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
  int8_t input[kKwsInputSize*2];
  size_t bytes = ee_get_buffer(reinterpret_cast<uint8_t *>(input),
                               kKwsInputSize * sizeof(int8_t));
  if (bytes / sizeof(int8_t) != kKwsInputSize) {
    th_printf("Input db has %d elemented, expected %d\n", bytes / sizeof(int8_t),
              kKwsInputSize);
    th_printf("Printing input buffer before reformatting:\r\n");
    for (uint32_t j=0;j<bytes;j++)
      th_printf("%d ",input[j]);
    th_printf("\r\n");
    return;
  }

  // Correcting for 0 point and scale
  double buff[kKwsInputSize];
  for(int i = 0; i < kKwsInputSize; i++) {
      buff[i] = 0.5847029089927673 * ((double)input[i] - 83.0);
  };

  //Casting the int8_t into int16_t
  int16_t *input_16 = (int16_t *)input;
  for(int i = 0; i < kKwsInputSize; i++) {
      double temp_value = floor(buff[i] * 512 + 0.5);
      // clip values
      if (temp_value > 32767.0){
          temp_value = 32767.0;
      } else if (temp_value < -32768.0) {
          temp_value = -32768.0;
      }
      input_16[i] = (int16_t)temp_value;
  };

  // Rearrange Input Tensor
  int16_t input_16_final[kKwsInputSize];
  int elem_index = 0;
  for(int i = 0; i < kNumCols; i++) {
      for (int j = 0; j < kNumRows; j++) {
      input_16_final[elem_index++] = input_16[kNumCols*j + i];
      }
  } 
 

  syntiant_ndp120_write_block(ndp, 0, NDP120_SPI_SAMPLE, (uint8_t *)input_16_final, 980);
  delay(500); // jhdbg was 2000
  
  syntiant_ndp120_write(ndp,1, NDP120_DNN_CONFIG_DNNDATACFG0, 0x0F0F);
  syntiant_ndp120_read_block(ndp,1, 0x6002FC2C, &input_16_final, 980);  
  
}


// Add to this method to return real inference results.
void th_results() {
  /**
   * The results need to be printed back in exactly this format; if easier
   * to just modify this loop than copy to results[] above, do that.
   */
  uint32_t data;
  uint16_t softmax[12];
  int8_t activations[12];
  int8_t check_acts[20];
  float softmax_float;

  th_printf("m-results-[");
 /*syntiant_ndp120_read(ndp, 0, NDP120_SPI_MATCHSTS, &data);
  th_printf("Match: %d ", data);
  th_printf("\r\n");
 */ 
  syntiant_ndp120_read_block(ndp,1, 0x1ffff0a0, &softmax[0], 24);
  
  syntiant_ndp120_write(ndp,1, NDP120_DNN_CONFIG_DNNDATACFG0, 0x0F0F);

  syntiant_ndp120_read_block(ndp,1, 0x6002FC20, &check_acts[0], 20);  //0x300809a0
 
  size_t kCategoryCount = 12;
  for(size_t loop0=0; loop0 < kCategoryCount; loop0++){
    softmax_float = ((softmax[loop0] -1)/ pow(2, 15));
    softmax_float = (softmax_float > 0) ? softmax_float : 0;
    
    /* float converted = DequantizeInt8ToFloat(runner->GetOutput()[i], runner->output_scale(),
       runner->output_zero_point()); */
    //Some platforms don't implement floating point formatting.
    th_printf("0.%d", static_cast<int>(softmax_float*10));
    th_printf("%d", static_cast<int>(softmax_float * 100) % 10);
    th_printf("%d", static_cast<int>(softmax_float * 1000) % 10);
    
    if (loop0 < (kCategoryCount - 1)) {
      th_printf(",");
    }
  }
  th_printf("]\r\n");
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() {
  // was previously: 
  // runner->Invoke(); 
  // TODO Add code to start inference here (mailbox command)
  // and wait for mailbox out (= acknowledge)
  // and then feature interrupt (or match interrupt)

    uint32_t data, cnt = 50;
  // Use Ilib to exchange Mailbox command with DSP Code
  // -- Sent Run DNN
  // -- Wait for ACK 
  // -- Wait for Feature/Match/Watterk Interrupt

  // TODO Add code to start inference here

    // Send MBOX Command Run DNN 
    syntiant_ndp120_write(ndp, 0, NDP120_SPI_MBIN, 0x09 | mbin_toggle_value);
    mbin_toggle_value = 0x80 ^ (mbin_toggle_value & 0x80);
    
    // Wait MBOX Interrupt
    for (cnt = 0; cnt <50000; cnt++) {   // was 50
        syntiant_ndp120_read(ndp, 0, NDP120_SPI_INTSTS, &data);
        if (data & 0x4) { // got interrupt
            break;
        }
        //delay(1);  // was 10
    }

    syntiant_ndp120_write(ndp, 0, NDP120_SPI_INTSTS, data); // clear interrupt status

    if (!(data & 0x4)) {
        th_printf("error: didn't get interrupt\n");
        goto error;
    }

    syntiant_ndp120_read(ndp, 0, NDP120_SPI_MBOUT, &data); // read mbout register for ack
    if (!(data & 0x7f)) {
        ; // th_printf("got ack from dsp\n");
    } else {
        th_printf("error: error ack from dsp\n");
        goto error;
    }

    // wait for feature interrupt
    for (cnt = 0; cnt < 50000; cnt++) {  // was 100
        syntiant_ndp120_read(ndp, 0, NDP120_SPI_INTSTS, &data);
        if (data & 0x10) {
            break;
        }
        //delay(1);  // was 10
    }

    syntiant_ndp120_write(ndp, 0, NDP120_SPI_INTSTS, data); // clear interrupt status

    if (!(data & 0x10)) {
        th_printf("error: didn't get interrupt\n");
    }
    else {
        ; // th_printf("got feature interrupt\n");
    }

error:
    return;
}

/// \brief optional API.
void th_final_initialize(void) {
  // TODO Any initialization can go here
}

void th_pre() {}
void th_post() {
  uint32_t data, cnt = 50;
  // Send MBOX Command TOGGLE GPIO
  syntiant_ndp120_write(ndp, 0, NDP120_SPI_MBIN, 0x0a | mbin_toggle_value);
  mbin_toggle_value = 0x80 ^ (mbin_toggle_value & 0x80);

  // Wait MBOX Interrupt
  for (cnt = 0; cnt <50000; cnt++) {   // was 50
      syntiant_ndp120_read(ndp, 0, NDP120_SPI_INTSTS, &data);
      if (data & 0x4) { // got interrupt
          break;
      }
      //delay(1);  // was 10
  }

  syntiant_ndp120_write(ndp, 0, NDP120_SPI_INTSTS, data); // clear interrupt status

  if (!(data & 0x4)) {
      th_printf("error: didn't get interrupt\n");
      goto error;
  }

  syntiant_ndp120_read(ndp, 0, NDP120_SPI_MBOUT, &data); // read mbout register for ack
  if (!(data & 0x7f)) {
      th_printf("got ack from dsp\n");
  } else {
      th_printf("error: error ack from dsp\n");
      goto error;
  }

error:
  return;
  
}

void th_command_ready(char volatile *p_command) {
  p_command = p_command;
  // g_dblg_pos += sprintf(g_dblog+g_dblg_pos, "dbg: Cmd: %s\r\n", p_command);
  // th_printf("dbg: Cmd: %s\r\n", p_command);
  ee_serial_command_parser_callback((char *)p_command);
}

// th_libc implementations.
int th_strncmp(const char *str1, const char *str2, size_t n) {
  return strncmp(str1, str2, n);
}

char *th_strncpy(char *dest, const char *src, size_t n) {
  return strncpy(dest, src, n);
}

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
void th_printf(const char *p_fmt, ...) {
  // avoiding re-allocation only saves about 30us, but I'll leave it 
  // unless it causes a problem 5MAY2021JHH
  static char buffer[200]; 
  va_list args;
  va_start(args, p_fmt);
  vsprintf(buffer, p_fmt, args); 
  Serial1.print(buffer);
  va_end(args);
}

char th_getchar() {
  // return getchar();
  return Serial1.read();
}

void th_serialport_initialize(void) {
  // serial port setup is handled at the beginning of setup()
  // # if EE_CFG_ENERGY_MODE==1
  //   pc.baud(9600);
  // # else
  //   pc.baud(115200);
  // # endif
}

void th_timestamp(void) {
# if EE_CFG_ENERGY_MODE==1
  timestampPin = 0;
  for (int i=0; i<1000; ++i) { // was 100'000 jhdbg
    asm("nop");
  }
  timestampPin = 1;
# else
  unsigned long microSeconds = 0ul;
  /* USER CODE 2 BEGIN */
  microSeconds = micros(); 
  /* USER CODE 2 END */
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP, microSeconds);
# endif

}

void th_timestamp_initialize(void) {
  /* USER CODE 1 BEGIN */
  // Setting up BOTH perf and energy here
  /* USER CODE 1 END */
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP_MODE);
  /* Always call the timestamp on initialize so that the open-drain output
     is set to "1" (so that we catch a falling edge) */
  th_timestamp();
}
