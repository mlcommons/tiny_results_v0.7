#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "ap_int.h"
#include "ap_fixed.h"

#include "nnet_utils/nnet_helpers.h"
//hls-fpga-machine-learning insert includes
#include "nnet_utils/nnet_activation.h"
#include "nnet_utils/nnet_activation_stream.h"
#include "nnet_utils/nnet_conv2d.h"
#include "nnet_utils/nnet_conv2d_stream.h"
#include "nnet_utils/nnet_dense.h"
#include "nnet_utils/nnet_dense_compressed.h"
#include "nnet_utils/nnet_dense_stream.h"
 
//hls-fpga-machine-learning insert weights
#include "weights/w2.h"
#include "weights/b2.h"
#include "weights/w5.h"
#include "weights/b5.h"
#include "weights/w8.h"
#include "weights/b8.h"
#include "weights/w11.h"
#include "weights/b11.h"
#include "weights/w14.h"
#include "weights/b14.h"
#include "weights/w18.h"
#include "weights/b18.h"

//hls-fpga-machine-learning insert layer-config
// q_conv2d_batchnorm
struct config2_mult : nnet::dense_config {
    static const unsigned n_in = 3;
    static const unsigned n_out = 32;
    static const unsigned reuse_factor = 1;
    static const unsigned strategy = nnet::latency;
    static const bool merged_relu = false;
    typedef ap_fixed<16,6> accum_t;
    typedef bias2_t bias_t;
    typedef weight2_t weight_t;
    typedef layer2_t:: value_type out_t;
    template<class x_T, class y_T, class res_T>
    using product = nnet::product::mult<x_T, y_T, res_T>;
};

struct config2 : nnet::conv2d_config {
    static const unsigned pad_top = 0;
    static const unsigned pad_bottom = 0;
    static const unsigned pad_left = 0;
    static const unsigned pad_right = 0;
    static const unsigned in_height = N_INPUT_1_1;
    static const unsigned in_width = N_INPUT_2_1;
    static const unsigned n_chan = N_INPUT_3_1;
    static const unsigned filt_height = 1;
    static const unsigned filt_width = 1;
    static const unsigned kernel_size = filt_height * filt_width;
    static const unsigned n_filt = N_FILT_2;
    static const unsigned stride_height = 1;
    static const unsigned stride_width = 1;
    static const unsigned out_height = OUT_HEIGHT_2;
    static const unsigned out_width = OUT_WIDTH_2;
    static const unsigned reuse_factor = 1;
    static const unsigned n_zeros = 2;
    static const bool store_weights_in_bram = false;
    static const unsigned strategy = nnet::latency;
    static const nnet::conv_implementation implementation = nnet::conv_implementation::linebuffer;
    static const unsigned min_height = N_INPUT_1_1;
    static const unsigned min_width = N_INPUT_2_1;
    static const ap_uint<filt_height * filt_width> pixels[min_height * min_width];
    typedef ap_fixed<16,6> accum_t;
    typedef bias2_t bias_t;
    typedef weight2_t weight_t;
    typedef config2_mult mult_config;
};
const ap_uint<config2::filt_height * config2::filt_width> config2::pixels[] = {0};

// q_activation
struct relu_config4 : nnet::activ_config {
    static const unsigned n_in = OUT_HEIGHT_2*OUT_WIDTH_2*N_FILT_2;
    static const unsigned table_size = 1024;
    static const unsigned io_type = nnet::io_parallel;
    static const unsigned reuse_factor = 1;
    typedef ap_fixed<18,8> table_t;
};

// q_conv2d_batchnorm_1
struct config5_mult : nnet::dense_config {
    static const unsigned n_in = 512;
    static const unsigned n_out = 4;
    static const unsigned reuse_factor = 1;
    static const unsigned strategy = nnet::latency;
    static const bool merged_relu = false;
    typedef ap_fixed<16,6> accum_t;
    typedef bias5_t bias_t;
    typedef weight5_t weight_t;
    typedef layer5_t:: value_type out_t;
    template<class x_T, class y_T, class res_T>
    using product = nnet::product::mult<x_T, y_T, res_T>;
};

struct config5 : nnet::conv2d_config {
    static const unsigned pad_top = 1;
    static const unsigned pad_bottom = 2;
    static const unsigned pad_left = 1;
    static const unsigned pad_right = 2;
    static const unsigned in_height = OUT_HEIGHT_2;
    static const unsigned in_width = OUT_WIDTH_2;
    static const unsigned n_chan = N_FILT_2;
    static const unsigned filt_height = 4;
    static const unsigned filt_width = 4;
    static const unsigned kernel_size = filt_height * filt_width;
    static const unsigned n_filt = N_FILT_5;
    static const unsigned stride_height = 1;
    static const unsigned stride_width = 1;
    static const unsigned out_height = OUT_HEIGHT_5;
    static const unsigned out_width = OUT_WIDTH_5;
    static const unsigned reuse_factor = 1;
    static const unsigned n_zeros = 372;
    static const bool store_weights_in_bram = false;
    static const unsigned strategy = nnet::latency;
    static const nnet::conv_implementation implementation = nnet::conv_implementation::linebuffer;
    static const unsigned min_height = OUT_HEIGHT_2;
    static const unsigned min_width = OUT_WIDTH_2;
    static const ap_uint<filt_height * filt_width> pixels[min_height * min_width];
    typedef ap_fixed<16,6> accum_t;
    typedef bias5_t bias_t;
    typedef weight5_t weight_t;
    typedef config5_mult mult_config;
};
const ap_uint<config5::filt_height * config5::filt_width> config5::pixels[] = {0};

// q_activation_1
struct relu_config7 : nnet::activ_config {
    static const unsigned n_in = OUT_HEIGHT_5*OUT_WIDTH_5*N_FILT_5;
    static const unsigned table_size = 1024;
    static const unsigned io_type = nnet::io_parallel;
    static const unsigned reuse_factor = 1;
    typedef ap_fixed<18,8> table_t;
};

// q_conv2d_batchnorm_2
struct config8_mult : nnet::dense_config {
    static const unsigned n_in = 64;
    static const unsigned n_out = 32;
    static const unsigned reuse_factor = 1;
    static const unsigned strategy = nnet::latency;
    static const bool merged_relu = false;
    typedef ap_fixed<16,6> accum_t;
    typedef bias8_t bias_t;
    typedef weight8_t weight_t;
    typedef layer8_t:: value_type out_t;
    template<class x_T, class y_T, class res_T>
    using product = nnet::product::mult<x_T, y_T, res_T>;
};

struct config8 : nnet::conv2d_config {
    static const unsigned pad_top = 1;
    static const unsigned pad_bottom = 2;
    static const unsigned pad_left = 1;
    static const unsigned pad_right = 2;
    static const unsigned in_height = OUT_HEIGHT_5;
    static const unsigned in_width = OUT_WIDTH_5;
    static const unsigned n_chan = N_FILT_5;
    static const unsigned filt_height = 4;
    static const unsigned filt_width = 4;
    static const unsigned kernel_size = filt_height * filt_width;
    static const unsigned n_filt = N_FILT_8;
    static const unsigned stride_height = 1;
    static const unsigned stride_width = 1;
    static const unsigned out_height = OUT_HEIGHT_8;
    static const unsigned out_width = OUT_WIDTH_8;
    static const unsigned reuse_factor = 1;
    static const unsigned n_zeros = 138;
    static const bool store_weights_in_bram = false;
    static const unsigned strategy = nnet::latency;
    static const nnet::conv_implementation implementation = nnet::conv_implementation::linebuffer;
    static const unsigned min_height = OUT_HEIGHT_5;
    static const unsigned min_width = OUT_WIDTH_5;
    static const ap_uint<filt_height * filt_width> pixels[min_height * min_width];
    typedef ap_fixed<16,6> accum_t;
    typedef bias8_t bias_t;
    typedef weight8_t weight_t;
    typedef config8_mult mult_config;
};
const ap_uint<config8::filt_height * config8::filt_width> config8::pixels[] = {0};

// q_activation_2
struct relu_config10 : nnet::activ_config {
    static const unsigned n_in = OUT_HEIGHT_8*OUT_WIDTH_8*N_FILT_8;
    static const unsigned table_size = 1024;
    static const unsigned io_type = nnet::io_parallel;
    static const unsigned reuse_factor = 1;
    typedef ap_fixed<18,8> table_t;
};

// q_conv2d_batchnorm_3
struct config11_mult : nnet::dense_config {
    static const unsigned n_in = 512;
    static const unsigned n_out = 32;
    static const unsigned reuse_factor = 1;
    static const unsigned strategy = nnet::latency;
    static const bool merged_relu = false;
    typedef ap_fixed<16,6> accum_t;
    typedef bias11_t bias_t;
    typedef weight11_t weight_t;
    typedef layer11_t:: value_type out_t;
    template<class x_T, class y_T, class res_T>
    using product = nnet::product::mult<x_T, y_T, res_T>;
};

struct config11 : nnet::conv2d_config {
    static const unsigned pad_top = 0;
    static const unsigned pad_bottom = 0;
    static const unsigned pad_left = 0;
    static const unsigned pad_right = 0;
    static const unsigned in_height = OUT_HEIGHT_8;
    static const unsigned in_width = OUT_WIDTH_8;
    static const unsigned n_chan = N_FILT_8;
    static const unsigned filt_height = 4;
    static const unsigned filt_width = 4;
    static const unsigned kernel_size = filt_height * filt_width;
    static const unsigned n_filt = N_FILT_11;
    static const unsigned stride_height = 4;
    static const unsigned stride_width = 4;
    static const unsigned out_height = OUT_HEIGHT_11;
    static const unsigned out_width = OUT_WIDTH_11;
    static const unsigned reuse_factor = 1;
    static const unsigned n_zeros = 2941;
    static const bool store_weights_in_bram = false;
    static const unsigned strategy = nnet::latency;
    static const nnet::conv_implementation implementation = nnet::conv_implementation::linebuffer;
    static const unsigned min_height = OUT_HEIGHT_8;
    static const unsigned min_width = OUT_WIDTH_8;
    static const ap_uint<filt_height * filt_width> pixels[min_height * min_width];
    typedef ap_fixed<16,6> accum_t;
    typedef bias11_t bias_t;
    typedef weight11_t weight_t;
    typedef config11_mult mult_config;
};
const ap_uint<config11::filt_height * config11::filt_width> config11::pixels[] = {0};

// q_activation_3
struct relu_config13 : nnet::activ_config {
    static const unsigned n_in = OUT_HEIGHT_11*OUT_WIDTH_11*N_FILT_11;
    static const unsigned table_size = 1024;
    static const unsigned io_type = nnet::io_parallel;
    static const unsigned reuse_factor = 1;
    typedef ap_fixed<18,8> table_t;
};

// q_conv2d_batchnorm_4
struct config14_mult : nnet::dense_config {
    static const unsigned n_in = 512;
    static const unsigned n_out = 32;
    static const unsigned reuse_factor = 1;
    static const unsigned strategy = nnet::latency;
    static const bool merged_relu = false;
    typedef ap_fixed<16,6> accum_t;
    typedef bias14_t bias_t;
    typedef weight14_t weight_t;
    typedef layer14_t:: value_type out_t;
    template<class x_T, class y_T, class res_T>
    using product = nnet::product::mult<x_T, y_T, res_T>;
};

struct config14 : nnet::conv2d_config {
    static const unsigned pad_top = 1;
    static const unsigned pad_bottom = 2;
    static const unsigned pad_left = 1;
    static const unsigned pad_right = 2;
    static const unsigned in_height = OUT_HEIGHT_11;
    static const unsigned in_width = OUT_WIDTH_11;
    static const unsigned n_chan = N_FILT_11;
    static const unsigned filt_height = 4;
    static const unsigned filt_width = 4;
    static const unsigned kernel_size = filt_height * filt_width;
    static const unsigned n_filt = N_FILT_14;
    static const unsigned stride_height = 1;
    static const unsigned stride_width = 1;
    static const unsigned out_height = OUT_HEIGHT_14;
    static const unsigned out_width = OUT_WIDTH_14;
    static const unsigned reuse_factor = 1;
    static const unsigned n_zeros = 3013;
    static const bool store_weights_in_bram = false;
    static const unsigned strategy = nnet::latency;
    static const nnet::conv_implementation implementation = nnet::conv_implementation::linebuffer;
    static const unsigned min_height = OUT_HEIGHT_11;
    static const unsigned min_width = OUT_WIDTH_11;
    static const ap_uint<filt_height * filt_width> pixels[min_height * min_width];
    typedef ap_fixed<16,6> accum_t;
    typedef bias14_t bias_t;
    typedef weight14_t weight_t;
    typedef config14_mult mult_config;
};
const ap_uint<config14::filt_height * config14::filt_width> config14::pixels[] = {0};

// q_activation_5
struct relu_config16 : nnet::activ_config {
    static const unsigned n_in = OUT_HEIGHT_14*OUT_WIDTH_14*N_FILT_14;
    static const unsigned table_size = 1024;
    static const unsigned io_type = nnet::io_parallel;
    static const unsigned reuse_factor = 1;
    typedef ap_fixed<18,8> table_t;
};

// q_dense
struct config18 : nnet::dense_config {
    static const unsigned n_in = N_SIZE_1_17;
    static const unsigned n_out = N_LAYER_18;
    static const unsigned io_type = nnet::io_parallel;
    static const unsigned strategy = nnet::latency;
    static const unsigned reuse_factor = 1;
    static const unsigned n_zeros = 7001;
    static const unsigned n_nonzeros = 13479;
    static const bool merged_relu = false;
    static const bool store_weights_in_bram = false;
    typedef ap_fixed<16,6> accum_t;
    typedef bias18_t bias_t;
    typedef weight18_t weight_t;
    typedef ap_uint<1> index_t;
    typedef layer18_t:: value_type out_t;
    template<class x_T, class y_T, class res_T>
    using product = nnet::product::mult<x_T, y_T, res_T>;
};

// softmax
struct softmax_config20 : nnet::activ_config {
    static const unsigned n_in = N_LAYER_18;
    static const unsigned table_size = 1024;
    static const unsigned io_type = nnet::io_parallel;
    static const unsigned reuse_factor = 1;
    static const unsigned axis = -1;
    static const nnet::softmax_implementation implementation = nnet::softmax_implementation::latency;
    typedef ap_fixed<18,8> exp_table_t;
    typedef ap_fixed<18,8> inv_table_t;
};


#endif
