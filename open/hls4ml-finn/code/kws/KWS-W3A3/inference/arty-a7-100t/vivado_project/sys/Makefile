MODULE = finn_kws_mlp_w3a3
PROJECT_AXIDMA = $(MODULE)_project
PROJECT_DIR_AXIDMA = $(PROJECT_AXIDMA)

PROJECT_OCM_AXIDMA = $(MODULE)_ocm_project
PROJECT_OCM_DIR_AXIDMA = $(PROJECT_OCM_AXIDMA)

help:
	@echo "INFO: make <TAB> for targets"
.PHONY: help

--wrapper:
	sed 's/TEMPLATE/$(MODULE)/g' verilog/TEMPLATE_design_wrapper.v > $(MODULE)_design_wrapper.v
.PHONY: --wrapper

sys-gui: --wrapper
	vivado -source tcl/script_axidma.tcl -mode gui -tclargs $(MODULE) 0 output_axidma_QONNX_model_Arty-A7
.PHONY: sys-gui

sys-gui-debug: --wrapper
	vivado -source tcl/script_axidma.tcl -mode gui -tclargs $(MODULE) 1 output_axidma_QONNX_model_Arty-A7
.PHONY: sys-gui-debug

sys: --wrapper
	vivado -source tcl/script_axidma.tcl -mode batch -tclargs $(MODULE) 0 output_axidma_QONNX_model_Arty-A7
.PHONY: sys

--wrapper-ocm:
	sed 's/TEMPLATE/$(MODULE)/g' verilog/TEMPLATE_ocm_design_wrapper.v > $(MODULE)_ocm_design_wrapper.v
.PHONY: --wrapper-ocm

sys-ocm-gui: --wrapper-ocm
	vivado -source tcl/script_axidma_ocm.tcl -mode gui -tclargs $(MODULE) 0 output_axidma_QONNX_model_Arty-A7
.PHONY: sys-ocm-gui

sys-ocm-gui-debug: --wrapper-ocm
	vivado -source tcl/script_axidma_ocm.tcl -mode gui -tclargs $(MODULE) 1 output_axidma_QONNX_model_Arty-A7
.PHONY: sys-ocm-gui-debug

sys-ocm: --wrapper-ocm
	vivado -source tcl/script_axidma_ocm.tcl -mode batch -tclargs $(MODULE) 0 output_axidma_QONNX_model_Arty-A7
.PHONY: sys-ocm

gui:
	vivado $(PROJECT_DIR_AXIDMA)/$(PROJECT_AXIDMA).xpr
.PHONY: gui

gui-ocm:
	vivado $(PROJECT_OCM_DIR_AXIDMA)/$(PROJECT_OCM_AXIDMA).xpr
.PHONY: gui-ocm

clean-ocm:
	rm -rf *_ocm_project
	rm -f $(MODULE)_ocm_design_wrapper.v 
.PHONY: clean-ocm

clean-ddr:
	rm -rf *_project
	rm -f $(MODULE)_design_wrapper.v 
.PHONY: clean-ddr

clean: clean-ocm clean-ddr
	rm -rf *.log *.txt *.jou *.str NA .Xil
.PHONY: clean
