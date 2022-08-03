#ifndef DEFINES_H_
#define DEFINES_H_

#include "ap_int.h"
#include "ap_fixed.h"
#include "nnet_utils/nnet_types.h"
#include <cstddef>
#include <cstdio>

//hls-fpga-machine-learning insert numbers
#define N_INPUT_1_1 32
#define N_INPUT_2_1 32
#define N_INPUT_3_1 3
#define OUT_HEIGHT_2 32
#define OUT_WIDTH_2 32
#define N_FILT_2 32
#define OUT_HEIGHT_5 32
#define OUT_WIDTH_5 32
#define N_FILT_5 4
#define OUT_HEIGHT_8 32
#define OUT_WIDTH_8 32
#define N_FILT_8 32
#define OUT_HEIGHT_11 8
#define OUT_WIDTH_11 8
#define N_FILT_11 32
#define OUT_HEIGHT_14 8
#define OUT_WIDTH_14 8
#define N_FILT_14 32
#define N_SIZE_1_17 2048
#define N_LAYER_18 10

//hls-fpga-machine-learning insert layer-precision
typedef ap_fixed<16,6> model_default_t;
typedef ap_fixed<16,6> input_t;
typedef ap_fixed<16,6> layer2_t;
typedef ap_fixed<8,3> weight2_t;
typedef ap_fixed<8,3> bias2_t;
typedef ap_fixed<16,6> layer4_t;
typedef ap_fixed<16,6> layer5_t;
typedef ap_fixed<8,3> weight5_t;
typedef ap_fixed<8,3> bias5_t;
typedef ap_fixed<16,6> layer7_t;
typedef ap_fixed<16,6> layer8_t;
typedef ap_fixed<8,3> weight8_t;
typedef ap_fixed<8,3> bias8_t;
typedef ap_fixed<16,6> layer10_t;
typedef ap_fixed<16,6> layer11_t;
typedef ap_fixed<8,3> weight11_t;
typedef ap_fixed<8,3> bias11_t;
typedef ap_fixed<16,6> layer13_t;
typedef ap_fixed<16,6> layer14_t;
typedef ap_fixed<8,3> weight14_t;
typedef ap_fixed<8,3> bias14_t;
typedef ap_fixed<16,6> layer16_t;
typedef ap_fixed<16,6> layer18_t;
typedef ap_fixed<8,3> weight18_t;
typedef ap_fixed<8,3> bias18_t;
typedef ap_fixed<16,6> result_t;

#endif
