ROOT_DIR=$(shell cd ..; pwd)
INCLUDE += -I${ROOT_DIR}

INCLUDE += -I../../../install_nn2/include
LDFLAGS = -L../../../install_nn2/lib

CC = riscv64-unknown-linux-gnu-gcc

CFLAGS += -O0 -g3 ${INCLUDE}

LDFLAGS += -lstdc++ -lcsi_nn2_c906 -lm -static

NET_PATH = .
DATA_PATH = .


PYTHON_FLAGS = -C -cd ${DATA_PATH}/COCO_val2014_000000343218.jpg -f ${NET_PATH}/model_vww.onnx -o . --quantization-scheme="float16" --pixel-format RGB --data-scale 0.003921569 --board c906  -in "actual_input_1" -on "output1" -is "1 3 96 96"



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
