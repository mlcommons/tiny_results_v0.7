PROJECT_AXIDMA = finn_cnv_w1a1_project

PROJECT_DIR_HLSMOVER = $(shell ls -td -- /tmp/finn_dev_$$USER/vivado_*/ | head -n 1)

help:
	@echo "INFO: make <TAB> for targets"
.PHONY: help

sys-axidma-gui:
	vivado -source tcl/axidma_script.tcl -mode gui &
.PHONY: sys-axidma-gui

sys-axidma:
	vivado -source tcl/axidma_script.tcl -mode batch
.PHONY: sys-axidma

gui-axidma:
	vivado $(PROJECT_AXIDMA)/$(PROJECT_AXIDMA).xpr
.PHONY: gui-axidma

sys-hlsmover:
	vivado -mode tcl $(PROJECT_DIR_HLSMOVER)/finn_zynq_link.xpr -source tcl/hlsmover_script.tcl -tclargs $(PROJECT_DIR_HLSMOVER) 
.PHONY: sys-hlsmover

gui-hlsmover:
	vivado $(PROJECT_DIR_HLSMOVER)/finn_zynq_link.xpr
.PHONY: gui-hlsmover

clean:
	rm -rf *.log *.txt *.jou *.str *_project NA .Xil
.PHONY: clean
