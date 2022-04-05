# Plumerai inference engine on microcontrollers

[Plumerai](https://plumerai.com/) is a company making deep learning tiny. Although specialized in Binarized Neural Networks (BNNs), Plumerai also delivers [state-of-the-art software](https://blog.plumerai.com/2021/10/cortex-m-inference-software/) to run 8-bit integer quantized neural networks on micro-controllers. The results of this submission are based on that software: the Plumerai inference engine.

The Plumerai inference engine does no pruning, quantization, or binarization. Model accuracy stays the same but inference speed goes up and memory usage goes down compared to other inference engines.


## The systems under test

We ran the Plumerai inference engine on the following microcontrollers:
* STMicroelectronics STM32L4R5 (`NUCLEO_L4R5ZI`)
* Infineon Cypress CY8CPROTO-062-4343W (`cy8cproto_062_4343w`)
* STMicroelectronics STM32F746 (`DISCO_F746NG`)

More details about the devices can be found under `systems/<device_name>.json`.

Note that the same code runs on many other devices. The example code submitted here is based on [MBED](https://os.mbed.com/), meaning only microcontrollers with MBED support will work. However, Plumerai's inference engine does not have this limitation and can work on other microcontrollers as well.


## The code

See [code/README.md](code/README.md) for details.


## The results

The logs for accuracy and performance can be found in the `results` subdirectory grouped by device and benchmark. They are the (unmodified) outputs of the official [EEMBC runner](https://github.com/eembc/energyrunner/) for accuracy and performance measurements. In summary the results are:

```
    |     latency in ms     |              |
    |  L4R5 |  CY8C |  F746 |     accuracy |
----x-------x-------x-------x--------------x
VWW | 220.4 | 200.0 |  59.4 | Top-1: 84.9% |
IC  | 184.6 | 202.8 |  65.1 | Top-1: 88.0% |
KWS |  73.5 |  63.6 |  19.5 | Top-1: 90.2% |
AD  |   5.9 |   6.8 |   2.4 |    AUC: 0.86 |
```
