PROJECT_DIR_HLSMOVER = $(shell ls -td -- /tmp/finn_dev_$$USER/vivado_zynq*/ | head -n 1)

help:
	@echo "INFO: make <TAB> for targets"
.PHONY: help

sys-hlsmover:
	vivado -mode tcl $(PROJECT_DIR_HLSMOVER)/finn_zynq_link.xpr -source tcl/script_hlsmover.tcl -tclargs $(PROJECT_DIR_HLSMOVER) 
.PHONY: sys-hlsmover

gui-hlsmover:
	vivado $(PROJECT_DIR_HLSMOVER)/finn_zynq_link.xpr
.PHONY: gui-hlsmover

clean:
	rm -rf *.log *.txt *.jou *.str *_project NA .Xil
.PHONY: clean
