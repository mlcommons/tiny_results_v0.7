# Build project software (generate .elf file)

# Set SDK workspace
setws ./

if { $::argc == 1 } {
    set batch [lindex $::argv 0]

# Build all projects
configapp -app finn_cnv_w1a1_hlsmover_standalone compiler-misc "-DCMD_N=$batch"
#configapp -app  finn_cnv_w1a1_hlsmover_standalone 
#-info compiler-misc
projects -build

# Connect to the hw_server
connect -url tcp:127.0.0.1:3121

#
source /tools/Xilinx/SDK/2019.1/scripts/sdk/util/zynqutils.tcl

# Select a target
targets -set -nocase -filter {name =~ "ARM*#0"}

# System reset
rst -system
after 3000

fpga ./finn_cnv_w1a1_hlsmover_platform/top_wrapper.bit
loadhw ./hdf/finn_cnv_w1a1_hlsmover_design_wrapper.hdf

# PS7 initialization
namespace eval xsdb {source ./finn_cnv_w1a1_hlsmover_platform/ps7_init.tcl; ps7_init; ps7_post_config}

# Download the elf
dow ./finn_cnv_w1a1_hlsmover_standalone/Debug/finn_cnv_w1a1_hlsmover_standalone.elf


# Insert a breakpoint @ exit
bpadd -addr &exit

# Continue execution until the target is suspended
con -block -timeout 500

}
