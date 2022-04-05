#!/bin/bash

# Exit early whenever a sub process fails
set -e

BOARD="brd2601b"

rm -rf build
mkdir -p build/energy
mkdir -p build/performance

slc generate -p anomaly_detection/mlperf_tiny_ad.slcp -d "build/energy/ad" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:1
slc generate -p image_classification/mlperf_tiny_ic.slcp -d "build/energy/ic" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:1
slc generate -p keyword_spotting/mlperf_tiny_kws.slcp -d "build/energy/kws" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:1
slc generate -p person_detection/mlperf_tiny_vww.slcp -d "build/energy/vww" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:1

slc generate -p anomaly_detection/mlperf_tiny_ad.slcp -d "build/performance/ad" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:0
slc generate -p image_classification/mlperf_tiny_ic.slcp -d "build/performance/ic" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:0
slc generate -p keyword_spotting/mlperf_tiny_kws.slcp -d "build/performance/kws" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:0
slc generate -p person_detection/mlperf_tiny_vww.slcp -d "build/performance/vww" --with "${BOARD}" --configuration EE_CFG_ENERGY_MODE:0

make -f mlperf_tiny_ad.Makefile  -C "build/energy/ad"  -j release
make -f mlperf_tiny_ic.Makefile  -C "build/energy/ic"  -j release
make -f mlperf_tiny_kws.Makefile -C "build/energy/kws" -j release
make -f mlperf_tiny_vww.Makefile -C "build/energy/vww" -j release

cp -v build/energy/ad/build/release/mlperf_tiny_ad.s37   \
      build/energy/ic/build/release/mlperf_tiny_ic.s37   \
      build/energy/kws/build/release/mlperf_tiny_kws.s37 \
      build/energy/vww/build/release/mlperf_tiny_vww.s37 \
      build/energy/

make -f mlperf_tiny_ad.Makefile  -C "build/performance/ad"  -j release
make -f mlperf_tiny_ic.Makefile  -C "build/performance/ic"  -j release
make -f mlperf_tiny_kws.Makefile -C "build/performance/kws" -j release
make -f mlperf_tiny_vww.Makefile -C "build/performance/vww" -j release

cp -v build/performance/ad/build/release/mlperf_tiny_ad.s37   \
      build/performance/ic/build/release/mlperf_tiny_ic.s37   \
      build/performance/kws/build/release/mlperf_tiny_kws.s37 \
      build/performance/vww/build/release/mlperf_tiny_vww.s37 \
      build/performance/