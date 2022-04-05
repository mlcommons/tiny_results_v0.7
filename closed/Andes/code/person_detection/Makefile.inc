EXAMPLE_NAME:=mlperf_libnn

MICROLITE_LIBS += $(BSP_LIB_DIR)/libae350.a

MLPERF_LIBNN_TEST_SRCS := \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/$(EXAMPLE_NAME)_test.cc \

OUTPUT_HANDLER_TEST_SRCS := \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/output_handler_test.cc \

MLPERF_LIBNN_SRCS := \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/api/internally_implemented.cc \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/submitter_implemented.cc \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/main.cc \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/vww/vww_inputs.cc \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/vww/vww_model_data.cc \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/vww/vww_model_settings.cc \

MLPERF_LIBNN_HDRS := \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/api/submitter_implemented.h \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/api/internally_implemented.h \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/util/tf_micro_model_runner.h \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/util/quantization_helpers.h \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/vww/vww_inputs.h \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/vww/vww_model_data.h \
tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/vww/vww_model_settings.h \

MLPERF_LIBNN_GENERATED_SRCS := \

MLPERF_LIBNN_GENERATED_HDRS := \

#Find any platform - specific rules for this example.
include $(wildcard tensorflow/lite/micro/examples/$(EXAMPLE_NAME)/*/Makefile.inc)

# Tests loading and running the sine model.
$(eval $(call microlite_test,$(EXAMPLE_NAME)_test,\
$(MLPERF_LIBNN_TEST_SRCS),,$(MLPERF_LIBNN_GENERATOR_INPUTS)))

# Tests producing an output.
$(eval $(call microlite_test,mlperf_libnn_output_handler_test,\
$(OUTPUT_HANDLER_TEST_SRCS),$(OUTPUT_HANDLER_TEST_HDRS)))

# Builds a standalone binary.
$(eval $(call microlite_test,$(EXAMPLE_NAME),\
$(MLPERF_LIBNN_SRCS),$(MLPERF_LIBNN_HDRS),$(MLPERF_LIBNN_GENERATOR_INPUTS)))

# Add sources and headers generated from $(MLPERF_LIBNN_GENERATOR_INPUTS).
MLPERF_LIBNN_SRCS += $(MLPERF_LIBNN_GENERATED_SRCS)
MLPERF_LIBNN_HDRS += $(MLPERF_LIBNN_GENERATED_HDRS)

list_$(EXAMPLE_NAME)_example_sources:
	@echo $(MLPERF_LIBNN_SRCS)

list_$(EXAMPLE_NAME)_example_headers:
	@echo $(MLPERF_LIBNN_HDRS)
