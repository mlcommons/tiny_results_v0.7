// Copyright (C) 2022, Plumerai Ltd.
// All rights reserved.
#ifndef PLUMERAI_INFERENCE_H
#define PLUMERAI_INFERENCE_H

#include <cstdint>

#include "plumerai_tf_compat.h"

extern "C" void DebugLog(const char *s) {
  // empty, add something here in case debugging is needed
}

namespace plumerai {

class InferenceEngine {
public:
  // The lifetime of the tensor arena and optional profiler must be at least
  // as long as that of the interpreter object, since the interpreter may need
  // to access them at any time. The interpreter doesn't do any deallocation of
  // any of the pointed-to objects, ownership remains with the caller.
  InferenceEngine(std::uint8_t *tensor_arena_ptr, int tensor_arena_size,
                  ::tflite::MicroProfiler *profiler = nullptr);

  // Runs through the model and allocates all necessary input, output and
  // intermediate tensors in the tensor arena.
  TfLiteStatus AllocateTensors();

  // Run inference, assumes input data is already set
  TfLiteStatus Invoke();

  TfLiteTensor *input(int input_id);
  TfLiteTensor *output(int output_id);
  size_t inputs_size() const;
  size_t outputs_size() const;

  // For debugging only.
  // This method gives the optimal arena size. It's only available after
  // `AllocateTensors` has been called.
  size_t arena_used_bytes() const;

private:
  struct impl;
  impl *impl_;
};

} // namespace plumerai

#endif // PLUMERAI_INFERENCE_H
