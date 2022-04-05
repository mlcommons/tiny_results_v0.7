/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. 
*
* Updated for EEMBC inference.
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#include <math.h>

#include <tensorflow/lite/micro/examples/main_functions.h>
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"

char* model_input_ptr;
uint32_t model_input_size;
float EEMBC_reslut[20];

#ifdef EEMBCKWS
	char input_buffer[1*1024];
#endif
#ifdef EEMBCVWW
	char input_buffer[27*1024];
#endif
#ifdef EEMBCAD
	char input_buffer[100*1024];
#endif
#ifdef EEMBCIC
	char input_buffer[3*1024];
#endif

extern const unsigned char tflite_model[];

namespace
{
    tflite::ErrorReporter* error_reporter = nullptr;
    const tflite::Model* model = nullptr;
    TfLiteTensor* model_input = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;

    // Create an area of memory to use for input, output, and intermediate arrays.
    // The size of this will depend on the model you're using, and may need to be
    // determined by experimentation.
#ifdef EEMBCKWS
    const int tensor_arena_size = 31 * 1024;
#endif
#ifdef EEMBCVWW
    const int tensor_arena_size = 99 * 1024;
#endif
#ifdef EEMBCAD
    const int tensor_arena_size = 3 * 1024;
#endif
#ifdef EEMBCIC
    const int tensor_arena_size = 54 * 1024;
#endif
    uint8_t tensor_arena[tensor_arena_size];
    TfLiteStatus invoke_status;
}  // namespace

// The function to initialize the model
uint8_t Setup()
{
      static tflite::MicroErrorReporter micro_error_reporter;  // NOLINT
      error_reporter = &micro_error_reporter;

      // Map the model into a usable data structure. This doesn't involve any
      // copying or parsing, it's a very lightweight operation.
      model = tflite::GetModel(tflite_model);

      if (model->version() != TFLITE_SCHEMA_VERSION)
      {
          error_reporter->Report("Model provided is schema version %d not equal to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
          return 0;
      }

      // Pull in only the operation implementations we need.
      // This relies on a complete list of all the ops needed by this graph.
      // An easier approach is to just use the AllOpsResolver, but this will
      // incur some penalty in code space for op implementations that are not
      // needed by this graph.
      //tflite::AllOpsResolver resolver;

      static tflite::MicroMutableOpResolver<9> micro_op_resolver;  // NOLINT
      micro_op_resolver.AddAdd();
      micro_op_resolver.AddAveragePool2D();
      micro_op_resolver.AddConv2D();
      micro_op_resolver.AddDepthwiseConv2D();
      micro_op_resolver.AddFullyConnected();
      micro_op_resolver.AddRelu();
      micro_op_resolver.AddRelu6();
      micro_op_resolver.AddReshape();
      micro_op_resolver.AddSoftmax();


      // Build an interpreter to run the model with.
      static tflite::MicroInterpreter static_interpreter(
              model, micro_op_resolver, tensor_arena, tensor_arena_size, error_reporter);
      interpreter = &static_interpreter;

       // Allocate memory from the tensor_arena for the model's tensors

      TfLiteStatus allocate_status =  interpreter->AllocateTensors();

      if (allocate_status != kTfLiteOk)
      {
          error_reporter->Report("Allocation failed");
          return 0;
      }

      // Get information about the memory area to use for the model's input.
      model_input = interpreter->input(0);

      model_input_ptr = input_buffer;
      model_input_size = sizeof(input_buffer);

      return 1;
}

int EEMBC_LoadTensor(void)
{
	for (size_t i = 0; i < model_input->bytes; ++i)
    {
#if defined(EEMBCAD)
		int8_t result;
		int32_t	 input_data;
		float* input_float = (float *)input_buffer;

		input_data = round(input_float[i]/model_input->params.scale)+model_input->params.zero_point;

		  if (input_data < INT8_MIN) {
			  input_data = INT8_MIN;
		  }
		  if (input_data > INT8_MAX) {
			  input_data = INT8_MAX;
		  }

		  model_input->data.int8[i] = ((int8_t)input_data);

#elif defined(EEMBCKWS)
	   model_input->data.int8[i] = ((int8_t)input_buffer[i]);
#else
       model_input->data.int8[i] = ((int8_t)input_buffer[i]) - 128;
#endif
    }

	return model_input->bytes;
}

int EEMBC_LoadTensor_PreInf(const int8_t *const_pnSample)
{
	for (size_t i = 0; i < model_input->bytes; ++i)
    {
       model_input->data.int8[i] = const_pnSample[i];
    }

	return model_input->bytes;
}

int EEMBC_Inference(void)
{
    TfLiteStatus invoke_status = interpreter->Invoke();
    return (int)invoke_status;
}

int EEMBC_Result(void)
{
    // Get predicted class
    TfLiteTensor* output = interpreter->output(0);
#ifdef EEMBCAD
    float diffsum = 0;
	float* input_float = (float *)input_buffer;


    for(int nClassID = 0; nClassID<output->bytes; nClassID++)
    {
        float converted = (float)((int32_t)output->data.int8[nClassID]-output->params.zero_point)*output->params.scale;
        float diff = converted - input_float[nClassID];
        diffsum += diff * diff;
    }
    diffsum /= output->bytes;

    EEMBC_reslut[0] = diffsum;

    return 1;

#else
    for(int nClassID = 0; nClassID<output->bytes; nClassID++)
    {
        EEMBC_reslut[nClassID] = (float)((int32_t)output->data.int8[nClassID]-output->params.zero_point)*output->params.scale;
    }

    return output->bytes;
#endif
}

int EEMBC_Result_PreInf(void)
{
    // Get predicted class
    TfLiteTensor* output = interpreter->output(0);

    for(int nClassID = 0; nClassID<output->bytes; nClassID++)
    {
        EEMBC_reslut[nClassID] = (float)output->data.int8[nClassID];
    }

    return output->bytes;
}
