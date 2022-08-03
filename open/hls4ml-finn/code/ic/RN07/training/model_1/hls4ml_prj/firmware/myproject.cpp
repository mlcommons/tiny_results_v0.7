//
//    rfnoc-hls-neuralnet: Vivado HLS code for neural-net building blocks
//
//    Copyright (C) 2017 EJ Kreinar
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <iostream>

#include "myproject.h"
#include "parameters.h"

void myproject(
    input_t input_1[N_INPUT_1_1*N_INPUT_2_1*N_INPUT_3_1],
    result_t layer20_out[N_LAYER_18],
    unsigned short &const_size_in_1,
    unsigned short &const_size_out_1
) {

    //hls-fpga-machine-learning insert IO
    #pragma HLS ARRAY_RESHAPE variable=input_1 complete dim=0
    #pragma HLS ARRAY_PARTITION variable=layer20_out complete dim=0
    #pragma HLS INTERFACE ap_vld port=input_1,layer20_out 
    #pragma HLS PIPELINE 

    const_size_in_1 = N_INPUT_1_1*N_INPUT_2_1*N_INPUT_3_1;
    const_size_out_1 = N_LAYER_18;

#ifndef __SYNTHESIS__
    static bool loaded_weights = false;
    if (!loaded_weights) {
        //hls-fpga-machine-learning insert load weights
        nnet::load_weights_from_txt<weight2_t, 96>(w2, "w2.txt");
        nnet::load_weights_from_txt<bias2_t, 32>(b2, "b2.txt");
        nnet::load_weights_from_txt<weight5_t, 2048>(w5, "w5.txt");
        nnet::load_weights_from_txt<bias5_t, 4>(b5, "b5.txt");
        nnet::load_weights_from_txt<weight8_t, 2048>(w8, "w8.txt");
        nnet::load_weights_from_txt<bias8_t, 32>(b8, "b8.txt");
        nnet::load_weights_from_txt<weight11_t, 16384>(w11, "w11.txt");
        nnet::load_weights_from_txt<bias11_t, 32>(b11, "b11.txt");
        nnet::load_weights_from_txt<weight14_t, 16384>(w14, "w14.txt");
        nnet::load_weights_from_txt<bias14_t, 32>(b14, "b14.txt");
        nnet::load_weights_from_txt<weight18_t, 20480>(w18, "w18.txt");
        nnet::load_weights_from_txt<bias18_t, 10>(b18, "b18.txt");
        loaded_weights = true;
    }
#endif

    // ****************************************
    // NETWORK INSTANTIATION
    // ****************************************

    //hls-fpga-machine-learning insert layers

    layer2_t layer2_out[OUT_HEIGHT_2*OUT_WIDTH_2*N_FILT_2];
    #pragma HLS ARRAY_PARTITION variable=layer2_out complete dim=0
    nnet::conv_2d_cl<input_t, layer2_t, config2>(input_1, layer2_out, w2, b2); // q_conv2d_batchnorm

    layer4_t layer4_out[OUT_HEIGHT_2*OUT_WIDTH_2*N_FILT_2];
    #pragma HLS ARRAY_PARTITION variable=layer4_out complete dim=0
    nnet::relu<layer2_t, layer4_t, relu_config4>(layer2_out, layer4_out); // q_activation

    layer5_t layer5_out[OUT_HEIGHT_5*OUT_WIDTH_5*N_FILT_5];
    #pragma HLS ARRAY_PARTITION variable=layer5_out complete dim=0
    nnet::conv_2d_cl<layer4_t, layer5_t, config5>(layer4_out, layer5_out, w5, b5); // q_conv2d_batchnorm_1

    layer7_t layer7_out[OUT_HEIGHT_5*OUT_WIDTH_5*N_FILT_5];
    #pragma HLS ARRAY_PARTITION variable=layer7_out complete dim=0
    nnet::relu<layer5_t, layer7_t, relu_config7>(layer5_out, layer7_out); // q_activation_1

    layer8_t layer8_out[OUT_HEIGHT_8*OUT_WIDTH_8*N_FILT_8];
    #pragma HLS ARRAY_PARTITION variable=layer8_out complete dim=0
    nnet::conv_2d_cl<layer7_t, layer8_t, config8>(layer7_out, layer8_out, w8, b8); // q_conv2d_batchnorm_2

    layer10_t layer10_out[OUT_HEIGHT_8*OUT_WIDTH_8*N_FILT_8];
    #pragma HLS ARRAY_PARTITION variable=layer10_out complete dim=0
    nnet::relu<layer8_t, layer10_t, relu_config10>(layer8_out, layer10_out); // q_activation_2

    layer11_t layer11_out[OUT_HEIGHT_11*OUT_WIDTH_11*N_FILT_11];
    #pragma HLS ARRAY_PARTITION variable=layer11_out complete dim=0
    nnet::conv_2d_cl<layer10_t, layer11_t, config11>(layer10_out, layer11_out, w11, b11); // q_conv2d_batchnorm_3

    layer13_t layer13_out[OUT_HEIGHT_11*OUT_WIDTH_11*N_FILT_11];
    #pragma HLS ARRAY_PARTITION variable=layer13_out complete dim=0
    nnet::relu<layer11_t, layer13_t, relu_config13>(layer11_out, layer13_out); // q_activation_3

    layer14_t layer14_out[OUT_HEIGHT_14*OUT_WIDTH_14*N_FILT_14];
    #pragma HLS ARRAY_PARTITION variable=layer14_out complete dim=0
    nnet::conv_2d_cl<layer13_t, layer14_t, config14>(layer13_out, layer14_out, w14, b14); // q_conv2d_batchnorm_4

    layer16_t layer16_out[OUT_HEIGHT_14*OUT_WIDTH_14*N_FILT_14];
    #pragma HLS ARRAY_PARTITION variable=layer16_out complete dim=0
    nnet::relu<layer14_t, layer16_t, relu_config16>(layer14_out, layer16_out); // q_activation_5

    layer18_t layer18_out[N_LAYER_18];
    #pragma HLS ARRAY_PARTITION variable=layer18_out complete dim=0
    nnet::dense<layer16_t, layer18_t, config18>(layer16_out, layer18_out, w18, b18); // q_dense

    nnet::softmax<layer18_t, result_t, softmax_config20>(layer18_out, layer20_out); // softmax

}
