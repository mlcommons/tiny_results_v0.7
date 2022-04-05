DESIGN := design_1

help:
	@echo "INFO: make <TAB> to show targets"
.PHONY: help

--setup:
	xsct ./setup.tcl $(DESIGN)
.PHONY: --setup

sdk: --setup
	cd  $(DESIGN)_standalone/src && cp -r ../../common/harness/* .
.PHONY: sdk

gui:
	xsdk --workspace . &
.PHONY: gui

clean:
	rm -rf $(DESIGN)_platform
	rm -rf $(DESIGN)_standalone
	rm -rf $(DESIGN)_standalone_bsp
	rm -rf $(DESIGN)*_fsbl
	rm -rf RemoteSystemsTempFiles
	rm -rf .Xil
	rm -rf .metadata
	rm -f *.log
.PHONY: clean

ultraclean: clean
	rm -rf hdf/*.hdf
.PHONY: ultraclean
