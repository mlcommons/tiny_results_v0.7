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

#include "api/submitter_implemented.h"
#include "api/internally_implemented.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <libgen.h>
#include <unistd.h>
#include <string>


extern "C" {
  #include "io.h"
  #include "csi_ref.h"
  void *csinn_(char *params);
}


constexpr int kNumCols = 32;
constexpr int kNumRows = 32;
constexpr int kNumChannels = 3;
constexpr int kInputSize = kNumCols * kNumRows * kNumChannels;
constexpr int kCategoryCount = 10;
const float mean[] = {0.485, 0.456, 0.406};
const float stdev[] = {0.229, 0.224, 0.225};


csi_session* sess;


void th_load_tensor() {

  uint8_t data_uint8[kInputSize];
  size_t bytes = ee_get_buffer(reinterpret_cast<uint8_t *>(data_uint8),
                               kInputSize * sizeof(int8_t) );
  if (bytes / sizeof(int8_t) != kInputSize ) {
    th_printf("Input db has %d elemented, expected %d\n",
              bytes / sizeof(int8_t), kInputSize);
  }

  float finput[kInputSize];
  memset(finput, 0, kInputSize);
  uint8_t *input;

  for (int i = 0; i < kInputSize; i++) {
     finput[i] = float(data_uint8[i]) / 255.;  
  }
  
  // normalize
  for (int channel = 0; channel < 3 ; channel ++ ){
    for (int i = 0 + channel * 32 * 32 ; i <  (channel + 1) * 32 * 32; i++) {
      finput[i] = (finput[i] - mean[channel]) / stdev[channel]; 
    }
  }


  input = csi_ref_f32_to_input_dtype(0, finput, sess);
  struct csi_tensor input_tensor;
  input_tensor.data = input ;
  csi_update_input(0, &input_tensor, sess);
}

// Add to this method to return real inference results.
void th_results() {
  /**
   * The results need to be printed back in exactly this format; if easier
   * to just modify this loop than copy to results[] above, do that.
   */
  th_printf("m-results-[");
  struct csi_tensor *output = csi_alloc_tensor(NULL);
  output->data = NULL;
  csi_get_output(0, output, sess);

  struct csi_tensor *foutput = csi_ref_tensor_transform_f32(output);
  float *res = (float*)foutput->data;
  for (size_t i = 0; i < kCategoryCount; i++) {
    // Some platforms don't implement floating point formatting.
    th_printf("%0.3f", res[i]);
    if (i < (kCategoryCount - 1)) {
      th_printf(",");
    }
  }

  th_printf("]\r\n");
  csi_ref_tensor_transform_free_f32(foutput);
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() { 
  csi_session_run(sess);
}

/// \brief optional API.
void th_final_initialize(void) {
  char path[] = "./model.params"; 
  char *params = get_binary_from_file(path);
  sess = (csi_session*)csinn_(params);
}


void th_pre() {}
void th_post() {}

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

void th_serialport_initialize(void) {
// # if EE_CFG_ENERGY_MODE==1
//   pc.baud(9600);
// # else
//   pc.baud(115200);
// # endif
}

void th_timestamp(void) {
  unsigned long microSeconds = 0ul;
  /* USER CODE 2 BEGIN */
  microSeconds = clock();
  // microSeconds = csi_get_timespec() / 1000;
  /* USER CODE 2 END */
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP, microSeconds);
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
