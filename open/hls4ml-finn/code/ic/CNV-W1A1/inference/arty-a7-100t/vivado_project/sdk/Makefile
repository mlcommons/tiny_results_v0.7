ACCNAME=finn_cnv_w1a1

help:
	@echo "INFO: make <TAB> to show targets"
.PHONY: help

--setup:
	xsct script.tcl $(ACCNAME)
.PHONY: --setup

sdk: --setup
	cd  $(ACCNAME)_standalone/src && cp -r ../../common/harness/* .
.PHONY: sdk

--setup-ocm:
	xsct script_ocm.tcl $(ACCNAME)_ocm
.PHONY: --setup-ocm

sdk-ocm: --setup-ocm
	rm -f $(ACCNAME)_ocm_standalone/src/helloworld.c
	cd  $(ACCNAME)_ocm_standalone/src && ln -s ../../common/main.c main.c && ln -s ../../common/src.h && ln -s ../../common/dst.h
.PHONY: sdk-ocm


gui:
	xsdk --workspace .
.PHONY: gui

#SAMPLE_COUNT=64
##SAMPLE_COUNT=166000

#data:
#	make -C ../../utils/dat2header/sim
#	../../utils/dat2header/sim/dat2header ../../hls/$(PROJECT_HLS)/tb_data/tb_input_features.dat $(PROJECT)/src/src.h src $(SAMPLE_COUNT)
#	../../utils/dat2header/sim/dat2header ../../hls/$(PROJECT_HLS)/tb_data/csim_results.log $(PROJECT)/src/dst.h dst $(SAMPLE_COUNT)
#.PHONY: data

clean:
	rm -rf $(ACCNAME)*_bsp
	rm -rf $(ACCNAME)*_platform
	rm -rf $(ACCNAME)*_standalone
	rm -rf $(ACCNAME)*_bootloader
	rm -rf RemoteSystemsTempFiles	
	rm -f *.jou
	rm -f *.log
	rm -rf webtalk
	rm -rf .sdk
	rm -rf .Xil
	rm -rf .metadata
	rm -rf hdf/*.c hdf/drivers hdf/*.h hdf/*.bit hdf/*.html hdf/*.tcl
.PHONY: clean

ultraclean: clean
	rm hdf/*.hdf
.PHONY: ultraclean
