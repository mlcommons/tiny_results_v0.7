/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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

#ifndef PLUMERAI_TENSORFLOW_COMPATIBILITY_H
#define PLUMERAI_TENSORFLOW_COMPATIBILITY_H
// This file contains only the relevant types and functions that we require from
// TF Micro
//
// C types, required to define `PlumeraiInference`
// From `tensorflow/lite/c/c_api_types.h`
//  - TfLiteStatus
//  - TfLiteType
//  - TfLiteQuantizationParams
// From `tensorflow/lite/c/common.h`
//  - TfLiteTensor
//      - TfLiteIntArray
//      - TfLitePtrUnion
//           - TfLiteComplex64
//           - TfLiteComplex128
//           - TfLiteFloat16
//      - TfLiteQuantizationType
//      - TfLiteQuantization
//      - TfLiteAllocationType
//
// C++ functions, required to access tensor data
// From `tensorflow/lite/micro/micro_error_reporter.h`
//  - ::MicroPrintf
// From `tensorflow/lite/kernels/internal/tensor_ctypes.h`
//  - tflite::GetTensorData<T>
//
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
//
// From `tensorflow/lite/c/c_api_types.h`
//
typedef enum TfLiteStatus {
  kTfLiteOk = 0,
  // Generally referring to an error in the runtime (i.e. interpreter)
  kTfLiteError = 1,
  // Generally referring to an error from a TfLiteDelegate itself.
  kTfLiteDelegateError = 2,
  // Generally referring to an error in applying a delegate due to
  // incompatibility between runtime and delegate, e.g., this error is returned
  // when trying to apply a TfLite delegate onto a model graph that's already
  // immutable.
  kTfLiteApplicationError = 3,
  // Generally referring to serialized delegate data not being found.
  // See tflite::delegates::Serialization.
  kTfLiteDelegateDataNotFound = 4,
  // Generally referring to data-writing issues in delegate serialization.
  // See tflite::delegates::Serialization.
  kTfLiteDelegateDataWriteError = 5,
} TfLiteStatus;
// Types supported by tensor
typedef enum {
  kTfLiteNoType = 0,
  kTfLiteFloat32 = 1,
  kTfLiteInt32 = 2,
  kTfLiteUInt8 = 3,
  kTfLiteInt64 = 4,
  kTfLiteString = 5,
  kTfLiteBool = 6,
  kTfLiteInt16 = 7,
  kTfLiteComplex64 = 8,
  kTfLiteInt8 = 9,
  kTfLiteFloat16 = 10,
  kTfLiteFloat64 = 11,
  kTfLiteComplex128 = 12,
  kTfLiteUInt64 = 13,
  kTfLiteResource = 14,
  kTfLiteVariant = 15,
  kTfLiteUInt32 = 16,
} TfLiteType;
typedef struct TfLiteQuantizationParams {
  float scale;
  int32_t zero_point;
} TfLiteQuantizationParams;
//
// From `tensorflow/lite/c/common.h`
//
// Fixed size list of integers. Used for dimensions and inputs/outputs tensor
// indices
typedef struct TfLiteIntArray {
  int size;
// gcc 6.1+ have a bug where flexible members aren't properly handled
// https://github.com/google/re2/commit/b94b7cd42e9f02673cd748c1ac1d16db4052514c
#if (!defined(__clang__) && defined(__GNUC__) && __GNUC__ == 6 &&              \
     __GNUC_MINOR__ >= 1) ||                                                   \
    defined(HEXAGON) ||                                                        \
    (defined(__clang__) && __clang_major__ == 7 && __clang_minor__ == 1)
  int data[0];
#else
  int data[];
#endif
} TfLiteIntArray;
// Single-precision complex data type compatible with the C99 definition.
typedef struct TfLiteComplex64 {
  float re, im; // real and imaginary parts, respectively.
} TfLiteComplex64;
// Double-precision complex data type compatible with the C99 definition.
typedef struct TfLiteComplex128 {
  double re, im; // real and imaginary parts, respectively.
} TfLiteComplex128;
// Half precision data type compatible with the C99 definition.
typedef struct TfLiteFloat16 {
  uint16_t data;
} TfLiteFloat16;
// SupportedQuantizationTypes.
typedef enum TfLiteQuantizationType {
  // No quantization.
  kTfLiteNoQuantization = 0,
  // Affine quantization (with support for per-channel quantization).
  // Corresponds to TfLiteAffineQuantization.
  kTfLiteAffineQuantization = 1,
} TfLiteQuantizationType;
// Structure specifying the quantization used by the tensor, if-any.
typedef struct TfLiteQuantization {
  // The type of quantization held by params.
  TfLiteQuantizationType type;
  // Holds an optional reference to a quantization param structure. The actual
  // type depends on the value of the `type` field (see the comment there for
  // the values and corresponding types).
  void *params;
} TfLiteQuantization;
/* A union of pointers that points to memory for a given tensor. */
typedef union TfLitePtrUnion {
  /* Do not access these members directly, if possible, use
   * GetTensorData<TYPE>(tensor) instead, otherwise only access .data, as other
   * members are deprecated. */
  int32_t *i32;
  uint32_t *u32;
  int64_t *i64;
  uint64_t *u64;
  float *f;
  TfLiteFloat16 *f16;
  double *f64;
  char *raw;
  const char *raw_const;
  uint8_t *uint8;
  bool *b;
  int16_t *i16;
  TfLiteComplex64 *c64;
  TfLiteComplex128 *c128;
  int8_t *int8;
  /* Only use this member. */
  void *data;
} TfLitePtrUnion;
// Memory allocation strategies.
//  * kTfLiteMmapRo: Read-only memory-mapped data, or data externally allocated.
//  * kTfLiteArenaRw: Arena allocated with no guarantees about persistence,
//        and available during eval.
//  * kTfLiteArenaRwPersistent: Arena allocated but persistent across eval, and
//        only available during eval.
//  * kTfLiteDynamic: Allocated during eval, or for string tensors.
//  * kTfLitePersistentRo: Allocated and populated during prepare. This is
//        useful for tensors that can be computed during prepare and treated
//        as constant inputs for downstream ops (also in prepare).
//  * kTfLiteCustom: Custom memory allocation provided by the user. See
//        TfLiteCustomAllocation below.
typedef enum TfLiteAllocationType {
  kTfLiteMemNone = 0,
  kTfLiteMmapRo,
  kTfLiteArenaRw,
  kTfLiteArenaRwPersistent,
  kTfLiteDynamic,
  kTfLitePersistentRo,
  kTfLiteCustom,
} TfLiteAllocationType;
typedef struct TfLiteTensor {
  // Quantization information. Replaces params field above.
  TfLiteQuantization quantization;
  // Quantization information.
  TfLiteQuantizationParams params;
  // A union of data pointers. The appropriate type should be used for a typed
  // tensor based on `type`.
  TfLitePtrUnion data;
  // A pointer to a structure representing the dimensionality interpretation
  // that the buffer should have. NOTE: the product of elements of `dims`
  // and the element datatype size should be equal to `bytes` below.
  TfLiteIntArray *dims;
  // The number of bytes required to store the data of this Tensor. I.e.
  // (bytes of each element) * dims[0] * ... * dims[n-1].  For example, if
  // type is kTfLiteFloat32 and dims = {3, 2} then
  // bytes = sizeof(float) * 3 * 2 = 4 * 3 * 2 = 24.
  size_t bytes;
  // The data type specification for data stored in `data`. This affects
  // what member of `data` union should be used.
  TfLiteType type;
  // How memory is mapped
  //  kTfLiteMmapRo: Memory mapped read only.
  //  i.e. weights
  //  kTfLiteArenaRw: Arena allocated read write memory
  //  (i.e. temporaries, outputs).
  TfLiteAllocationType allocation_type;
  // True if the tensor is a variable.
  bool is_variable;
} TfLiteTensor;
#ifdef __cplusplus
} // extern C
#endif
//
// From `tensorflow/lite/micro/micro_error_reporter.h`
//
void MicroPrintf(const char *format, ...);
//
// From `tensorflow/lite/kernels/internal/tensor_ctypes.h`
//
namespace tflite {
template <typename T> inline T *GetTensorData(TfLiteTensor *tensor) {
  return tensor != nullptr ? reinterpret_cast<T *>(tensor->data.raw) : nullptr;
}
template <typename T>
inline const T *GetTensorData(const TfLiteTensor *tensor) {
  return tensor != nullptr ? reinterpret_cast<const T *>(tensor->data.raw)
                           : nullptr;
}

class MicroProfiler;
} // namespace tflite
#endif // PLUMERAI_TENSORFLOW_COMPATIBILITY_H
