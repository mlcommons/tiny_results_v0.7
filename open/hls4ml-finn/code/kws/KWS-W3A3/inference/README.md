# Building the FINN bitfiles


## Setup

1. Ensure that Xilinx Vivado 2019.1 is installed and available to the user. Other versions of Vivado are also compatible with FINN, but 2019.1 was used for this benchmark. The official Xilinx downloads page can be found [here](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html).

2. Run the `get-finn.sh` under this directory to clone FINN at the appropriate commit. Note that you may have
to do this again in the future when the `finn-examples` repo gets updated and requires FINN at a newer commit.

3. Ensure you have the [requirements](https://finn.readthedocs.io/en/latest/getting_started.html#requirements) for FINN installed, which includes
Docker community edition `docker-ce`.

4. Set up ``FINN_XILINX_PATH`` and ``FINN_XILINX_VERSION`` environment variables pointing respectively to the Xilinx tools installation directory and version (e.g. ``FINN_XILINX_PATH=/opt/Xilinx`` and ``FINN_XILINX_VERSION=2019.1``)

5. Make sure the Y2K22 patch has been applied, see: https://support.xilinx.com/s/article/76960?language=en_US

## Build bitfiles

Please see the READMEs under the respective subfolders here for instructions on how to rebuild the bitfiles.
