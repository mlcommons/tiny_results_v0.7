ROOT_DIR=$(shell cd ..; pwd)
INCLUDE += -I${ROOT_DIR}
INCLUDE += -I../../../install_nn2/include
LDFLAGS = -L../../../install_nn2/lib

CC = riscv64-unknown-linux-gnu-gcc

CFLAGS += -O0 -g3 ${INCLUDE}

LDFLAGS += -lstdc++ -lcsi_nn2_c906 -lm -static

NET_PATH = .
DATA_PATH = .


PYTHON_FLAGS = -C -cd ./sample_data.npz -f ./model_ad.onnx -o . -in "actual_input_1" -on "output1" -is "1 640" --board c906 --quantization-scheme="float16"



.PHONY: clean all

all: c_runtime

main.c model.c:
	hhb $(PYTHON_FLAGS)

c_runtime: main.c model.c io.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@  $^ $(LDFLAGS)

clean:
	-rm c_runtime -rf
	-rm main.c model.c model.params process.c process.h io.c io.h -rf
	-rm *.tensor *.bin -rf
