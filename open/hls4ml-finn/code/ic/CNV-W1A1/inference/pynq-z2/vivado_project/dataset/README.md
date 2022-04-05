# Create Data Header Files for CIFAR10

## Setup a Conda Environment

One time only:
```
./install_miniconda3.sh
./create_env.sh
```

In any new console:
```
conda activate finn-env
```

## Run

```
make run
```

Two header files get generated `src.h` and `dst.h`. Edit the Python script `generate_data_header.py` if you need more images out of CIFAR10.
