ACCNAME=finn_kws_mlp_w3a3

help:
	@echo "INFO: make <TAB> to show targets"
.PHONY: help

--setup-hlsmover:
	xsct script_hlsmover.tcl $(ACCNAME)
.PHONY: --setup-hlsmover

sdk-hlsmover: --setup-hlsmover
	rm -f $(ACCNAME)_hlsmover_standalone/src/helloworld.c
	# cd  $(ACCNAME)_hlsmover_standalone/src && ln -s ../../common/main_hlsmover.c main.c && ln -s ../../common/src.h && ln -s ../../common/dst.h
	cd  $(ACCNAME)_hlsmover_standalone/src && cp ../../common/harness/main.c main.c && cp -r ../../common/harness/profile profile && cp -r ../../common/harness/monitor monitor
.PHONY: sdk-hlsmover

gui:
	xsdk --workspace .
.PHONY: gui

clean:
	rm -rf $(ACCNAME)*_bsp
	rm -rf $(ACCNAME)*_platform
	rm -rf $(ACCNAME)*_standalone
	rm -rf $(ACCNAME)*_fsbl
	rm -rf RemoteSystemsTempFiles
	rm -rf SDK.log
	rm -rf webtalk
	rm -rf .sdk
	rm -rf .Xil
	rm -rf .metadata
	rm -rf hdf/*.c hdf/drivers hdf/*.h hdf/*.bit hdf/*.html hdf/*.tcl
.PHONY: clean

ultraclean: clean
	rm hdf/*.hdf
.PHONY: ultraclean
