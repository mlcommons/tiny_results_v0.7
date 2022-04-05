# Script
set module ""
set debug 0
set stiched_ip_dir ""

if { $argc == 3 } {
    set module [lindex $argv 0]
    set debug [lindex $argv 1]
    set stiched_ip_dir [lindex $argv 2]
} else {
    puts "Error!"
}

# Set accelerator
set top_module $module

# Set directory
set proj_dir "./$module\_ocm_project"

# Set project
set proj "$module\_ocm_project"

# Set design name
set design_name $module\_ocm_design

# Create project
create_project $proj $proj_dir -part xc7a100tcsg324-1

# Set project properties
set_property board_part digilentinc.com:arty-a7-100:part0:1.0 [current_project]

# Set IP repository paths
set_property  ip_repo_paths  $proj_dir/../../../$stiched_ip_dir/stitched_ip/ip [current_project]
update_ip_catalog -rebuild

# Create the design block
create_bd_design $design_name

# Create clock wizard
create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wizard
set_property -dict [list CONFIG.RESET_TYPE {ACTIVE_LOW} CONFIG.RESET_PORT {resetn}] [get_bd_cells clk_wizard]
apply_board_connection -board_interface "sys_clock" -ip_intf "clk_wizard/clock_CLK_IN1" -diagram $design_name

# Connect system reset
apply_board_connection -board_interface "reset" -ip_intf "/clk_wizard/resetn" -diagram "finn_kws_mlp_w3a3_ocm_design" 

# Create instance of MicroBlaze
create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:11.0 microblaze_mcu
set_property -dict [list \
    CONFIG.C_AREA_OPTIMIZED {1} \
    CONFIG.C_D_AXI {1} \
    CONFIG.G_TEMPLATE_LIST {1} \
    CONFIG.C_USE_REORDER_INSTR {0} \
    CONFIG.C_USE_FPU {2} \
    CONFIG.C_DEBUG_ENABLED {1} \
    CONFIG.C_NUMBER_OF_PC_BRK {0} \
    CONFIG.C_USE_ICACHE {0} \
    CONFIG.C_ADDR_TAG_BITS {0} \
    CONFIG.C_CACHE_BYTE_SIZE {4096} \
    CONFIG.C_USE_DCACHE {0} \
    CONFIG.C_DCACHE_ADDR_TAG {0} \
    CONFIG.C_DCACHE_BYTE_SIZE {4096} \
    CONFIG.C_MMU_DTLB_SIZE {2} \
    CONFIG.C_MMU_ITLB_SIZE {1} \
    CONFIG.C_MMU_ZONES {2} \
    ] [get_bd_cells microblaze_mcu]

apply_bd_automation -rule xilinx.com:bd_rule:microblaze -config { \
    axi_intc {0} \
    axi_periph {Enabled} \
    cache {None} \
    clk {/clk_wizard/clk_out1 (100 MHz)} \
    debug_module {Debug Only} \
    ecc {None} \
    local_mem {128KB} \
    preset {None} } [get_bd_cells microblaze_mcu]

# Renaming
set_property name mdm [get_bd_cells mdm_1]
set_property name proc_sys_reset_finn [get_bd_cells rst_clk_wizard_100M]

apply_bd_automation -rule xilinx.com:bd_rule:board -config { \
    Board_Interface {reset ( System Reset ) } \
    Manual_Source {Auto}} [get_bd_pins proc_sys_reset_finn/ext_reset_in]

# Edit local memory
delete_bd_objs \
    [get_bd_intf_nets microblaze_mcu_local_memory/microblaze_mcu_ilmb_bus] \
    [get_bd_intf_nets microblaze_mcu_local_memory/microblaze_mcu_ilmb_cntlr] \
    [get_bd_cells microblaze_mcu_local_memory/ilmb_bram_if_cntlr]
set_property -dict [list CONFIG.C_NUM_LMB {2}] [get_bd_cells microblaze_mcu_local_memory/dlmb_bram_if_cntlr]
connect_bd_intf_net [get_bd_intf_pins microblaze_mcu_local_memory/ilmb_v10/LMB_Sl_0] [get_bd_intf_pins microblaze_mcu_local_memory/dlmb_bram_if_cntlr/SLMB1]
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0
move_bd_cells [get_bd_cells microblaze_mcu_local_memory] [get_bd_cells axi_bram_ctrl_0]
set_property name axi_bram_ctrl [get_bd_cells microblaze_mcu_local_memory/axi_bram_ctrl_0]
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells microblaze_mcu_local_memory/axi_bram_ctrl]
connect_bd_intf_net [get_bd_intf_pins microblaze_mcu_local_memory/axi_bram_ctrl/BRAM_PORTA] [get_bd_intf_pins microblaze_mcu_local_memory/lmb_bram/BRAM_PORTB]
connect_bd_net [get_bd_pins microblaze_mcu_local_memory/LMB_Clk] [get_bd_pins microblaze_mcu_local_memory/axi_bram_ctrl/s_axi_aclk]
create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_0
set_property -dict [list CONFIG.C_OPERATION {not} CONFIG.LOGO_FILE {data/sym_notgate.png}] [get_bd_cells util_vector_logic_0]
set_property name resetn [get_bd_cells util_vector_logic_0]
move_bd_cells [get_bd_cells microblaze_mcu_local_memory] [get_bd_cells resetn]
connect_bd_net [get_bd_pins microblaze_mcu_local_memory/resetn/Res] [get_bd_pins microblaze_mcu_local_memory/axi_bram_ctrl/s_axi_aresetn]
connect_bd_net [get_bd_pins microblaze_mcu_local_memory/SYS_Rst] [get_bd_pins microblaze_mcu_local_memory/resetn/Op1]

# Create UART-lite interface
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uart
apply_board_connection -board_interface "usb_uart" -ip_intf "axi_uart/UART" -diagram $design_name
set_property -dict [list CONFIG.C_BAUDRATE {115200}] [get_bd_cells axi_uart]
#set_property -dict [list CONFIG.C_BAUDRATE {9600}] [get_bd_cells axi_uart]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {/mig_7series_0/ui_clk (83 MHz)} \
    Clk_slave {Auto} \
    Clk_xbar {Auto} \
    Master {/microblaze_mcu (Periph)} \
    Slave {/axi_uart/S_AXI} \
    intc_ip {New AXI Interconnect} \
    master_apm {0}} [get_bd_intf_pins axi_uart/S_AXI]

# Debug UART-lite
create_bd_port -dir O usb_uart_txd_debug
connect_bd_net [get_bd_pins /axi_uart/tx] [get_bd_ports usb_uart_txd_debug]
add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/uart_debug.xdc

# General constraints
add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/general.xdc

## Create UART interface
#create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uart16550:2.0 axi_uart
#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/mig_7series_0/ui_clk (83 MHz)} Clk_slave {Auto} Clk_xbar {Auto} Master {/microblaze_mcu (Periph)} Slave {/axi_uart/S_AXI} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins axi_uart/S_AXI]
#apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {usb_uart ( USB UART ) } Manual_Source {Auto}}  [get_bd_intf_pins axi_uart/UART]

# Debug UART
#create_bd_port -dir O usb_uart_txd_debug
#connect_bd_net [get_bd_pins /axi_uart/sout] [get_bd_ports usb_uart_txd_debug]
#add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/uart_debug.xdc

# Add timer
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_mcu
set_property -dict [list CONFIG.enable_timer2 {0}] [get_bd_cells axi_timer_mcu]
set_property -dict [list CONFIG.mode_64bit {0} CONFIG.enable_timer2 {0}] [get_bd_cells axi_timer_mcu]
#set_property -dict [list CONFIG.mode_64bit {1} CONFIG.enable_timer2 {0}] [get_bd_cells axi_timer_mcu]
#set_property -dict [list CONFIG.mode_64bit {1}] [get_bd_cells axi_timer_mcu]

# Wire timer
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {/clk_wizard/clk_out1 (100 MHz)} \
    Clk_slave {Auto} \
    Clk_xbar {/clk_wizard/clk_out1 (100 MHz)} \
    Master {/microblaze_mcu (Periph)} \
    Slave {/axi_timer_mcu/S_AXI} \
    intc_ip {/microblaze_mcu_axi_periph} \
    master_apm {0}} [get_bd_intf_pins axi_timer_mcu/S_AXI]

## Add Quad SPI
#create_bd_cell -type ip -vlnv xilinx.com:ip:axi_quad_spi:3.2 axi_quad_spi_0
#set_property -dict [list CONFIG.C_SPI_MEMORY {3} CONFIG.C_SPI_MODE {2} CONFIG.C_SCK_RATIO {2}] [get_bd_cells axi_quad_spi_0]
#apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {qspi_flash ( Quad SPI Flash ) } Manual_Source {Auto}}  [get_bd_intf_pins axi_quad_spi_0/SPI_0]
#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/mig_7series_0/ui_clk (83 MHz)} Clk_slave {Auto} Clk_xbar {/mig_7series_0/ui_clk (83 MHz)} Master {/microblaze_mcu (Periph)} Slave {/axi_quad_spi_0/AXI_LITE} intc_ip {/microblaze_mcu_axi_periph} master_apm {0}}  [get_bd_intf_pins axi_quad_spi_0/AXI_LITE]
#set_property -dict [list CONFIG.CLKOUT3_USED {true} CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {50} CONFIG.MMCM_CLKOUT2_DIVIDE {20} CONFIG.NUM_OUT_CLKS {3} CONFIG.CLKOUT3_JITTER {151.636} CONFIG.CLKOUT3_PHASE_ERROR {98.575}] [get_bd_cells clk_wizard]
#connect_bd_net [get_bd_pins clk_wizard/clk_out3] [get_bd_pins axi_quad_spi_0/ext_spi_clk]

# Add FINN IP(s)
create_bd_cell -type ip -vlnv xilinx_finn:finn:finn_design:1.0 finn_design

# Add data width converter for AXI-stream
create_bd_cell -type ip -vlnv xilinx.com:ip:axis_dwidth_converter:1.1 finn_design_axis_dwidth_converter
set_property -dict [list CONFIG.S_TDATA_NUM_BYTES.VALUE_SRC USER] [get_bd_cells finn_design_axis_dwidth_converter]
set_property -dict [list CONFIG.M_TDATA_NUM_BYTES {10}] [get_bd_cells finn_design_axis_dwidth_converter]

# Connect dwidth converter to FINN IP(s)
connect_bd_intf_net [get_bd_intf_pins finn_design_axis_dwidth_converter/M_AXIS] [get_bd_intf_pins finn_design/s_axis_0]

# Wire FINN IP(s)
connect_bd_net [get_bd_pins finn_design_axis_dwidth_converter/aclk] [get_bd_pins clk_wizard/clk_out1]
disconnect_bd_net /proc_sys_reset_finn_peripheral_aresetn [get_bd_pins proc_sys_reset_finn/peripheral_aresetn]
connect_bd_net [get_bd_pins finn_design_axis_dwidth_converter/aresetn] [get_bd_pins proc_sys_reset_finn/peripheral_aresetn]
connect_bd_net [get_bd_pins proc_sys_reset_finn/interconnect_aresetn] [get_bd_pins microblaze_mcu_axi_periph/S00_ARESETN]
connect_bd_net [get_bd_pins finn_design/ap_clk] [get_bd_pins clk_wizard/clk_out1]
connect_bd_net [get_bd_pins finn_design/ap_rst_n] [get_bd_pins proc_sys_reset_finn/peripheral_aresetn]

# Add AXI DMA IP
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma
set_property -dict [list CONFIG.c_include_sg {0} CONFIG.c_sg_include_stscntrl_strm {0} CONFIG.c_include_mm2s {1}] [get_bd_cells axi_dma]
set_property -dict [list CONFIG.c_m_axi_mm2s_data_width {32} CONFIG.c_m_axis_mm2s_tdata_width {8} CONFIG.c_mm2s_burst_size {16}] [get_bd_cells axi_dma]
set_property -dict [list CONFIG.c_include_mm2s_dre {1} CONFIG.c_include_s2mm_dre {1}] [get_bd_cells axi_dma]

# Wire AXI DMA IP
connect_bd_intf_net [get_bd_intf_pins axi_dma/M_AXIS_MM2S] [get_bd_intf_pins finn_design_axis_dwidth_converter/S_AXIS]
connect_bd_intf_net [get_bd_intf_pins finn_design/m_axis_0] [get_bd_intf_pins axi_dma/S_AXIS_S2MM]

# Add Smart AXI Interconnect
create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 axi_smartconnect
connect_bd_intf_net [get_bd_intf_pins axi_smartconnect/M00_AXI] [get_bd_intf_pins microblaze_mcu_local_memory/axi_bram_ctrl/S_AXI]
set_property name S_AXI_DMA [get_bd_intf_pins microblaze_mcu_local_memory/S_AXI]
connect_bd_intf_net [get_bd_intf_pins axi_smartconnect/S00_AXI] [get_bd_intf_pins axi_dma/M_AXI_MM2S]
connect_bd_intf_net [get_bd_intf_pins axi_smartconnect/S01_AXI] [get_bd_intf_pins axi_dma/M_AXI_S2MM]
connect_bd_net [get_bd_pins axi_smartconnect/aclk] [get_bd_pins clk_wizard/clk_out1]
connect_bd_net [get_bd_pins axi_smartconnect/aresetn] [get_bd_pins proc_sys_reset_finn/interconnect_aresetn]

# Wire AXI-lite for AXI DMA IP (automation)
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {/clk_wizard/clk_out1 (100 MHz)} \
    Clk_slave {Auto} \
    Clk_xbar {/clk_wizard/clk_out1 (100 MHz)} \
    Master {/microblaze_mcu (Periph)} \
    Slave {/axi_dma/S_AXI_LITE} \
    intc_ip {/microblaze_mcu_axi_periph} \
    master_apm {0}} [get_bd_intf_pins axi_dma/S_AXI_LITE]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wizard/clk_out1 (100 MHz)" } [get_bd_pins axi_dma/m_axi_mm2s_aclk]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wizard/clk_out1 (100 MHz)" } [get_bd_pins axi_dma/m_axi_s2mm_aclk]

# Address Editor
assign_bd_address [get_bd_addr_segs {microblaze_mcu_local_memory/axi_bram_ctrl/S_AXI/Mem0 }]
assign_bd_address [get_bd_addr_segs {microblaze_mcu_local_memory/dlmb_bram_if_cntlr/SLMB1/Mem }]
set_property range 4K [get_bd_addr_segs {microblaze_mcu/Data/SEG_axi_uart_Reg}]
set_property range 4K [get_bd_addr_segs {microblaze_mcu/Data/SEG_axi_timer_mcu_Reg}]
set_property range 4K [get_bd_addr_segs {microblaze_mcu/Data/SEG_axi_dma_Reg}]

# ILAs

# Validate the design block we created
validate_bd_design

# Save design
save_bd_design

# Create verilog top
#make_wrapper -files [get_files $proj_dir/$proj.srcs/sources_1/bd/$design_name/$design_name.bd] -top
#add_files -norecurse $proj_dir/$proj.srcs/sources_1/bd/$design_name/hdl/$design_name\_wrapper.v
add_files -norecurse $proj_dir/../$design_name\_wrapper.v

#update_compile_order -fileset sources_1

if { $debug == 0 } {
    # Run the synthesis step
    launch_runs synth_1 -j 16
    wait_on_run -timeout 360 synth_1

    # Run the implementation step
    puts "INFO: Run bistream generation..."
    launch_runs impl_1 -to_step write_bitstream
    wait_on_run -timeout 360 impl_1

    if {[get_property PROGRESS [get_runs impl_1]] != "100%"} {
        error "ERROR: bitstream generation failed!"
    }

    # Export the bitstream and the hardware for the SDK
    puts "INFO: Export hardware..."
    file copy -force $proj_dir/$proj.runs/impl_1/$design_name\_wrapper.sysdef \
        ../sdk/hdf/$design_name\_wrapper.hdf
}

