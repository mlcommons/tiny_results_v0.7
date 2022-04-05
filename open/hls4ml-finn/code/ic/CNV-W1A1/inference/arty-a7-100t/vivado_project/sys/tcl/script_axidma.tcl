# Script
set module ""
set debug 0
set stitched_ip_dir ""

set eembc_power 1

if { $argc == 3 } {
    set module [lindex $argv 0]
    set debug [lindex $argv 1]
    set stitched_ip_dir [lindex $argv 2]
} else {
    puts "Error!"
}

# Set accelerator
set top_module $module

# Set directory
set proj_dir "./$module\_project"

# Set project
set proj "$module\_project"

# Set design name
set design_name $module\_design

# Create project
create_project $proj $proj_dir -part xc7a100tcsg324-1

# Set project properties
set_property board_part digilentinc.com:arty-a7-100:part0:1.0 [current_project]

# Set IP repository paths
set_property  ip_repo_paths  $proj_dir/../../../$stitched_ip_dir/stitched_ip/ip [current_project]
update_ip_catalog -rebuild

# Create the design block
create_bd_design $design_name

# Create clock wizard
create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0
apply_board_connection -board_interface "sys_clock" -ip_intf "clk_wiz_0/clock_CLK_IN1" -diagram $design_name
set_property name clk_wizard [get_bd_cells clk_wiz_0]
set_property -dict [list CONFIG.CLKOUT2_USED {true} CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {166.667} CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {200.00} CONFIG.MMCM_CLKOUT0_DIVIDE_F {6.000} CONFIG.MMCM_CLKOUT1_DIVIDE {5} CONFIG.NUM_OUT_CLKS {2} CONFIG.CLKOUT1_JITTER {118.758} CONFIG.CLKOUT2_JITTER {114.829} CONFIG.CLKOUT2_PHASE_ERROR {98.575}] [get_bd_cells clk_wizard]

# Add clock signal for FINN IP
set_property -dict [list CONFIG.CLKOUT3_USED {true} CONFIG.MMCM_CLKOUT2_DIVIDE {10} CONFIG.NUM_OUT_CLKS {3} CONFIG.CLKOUT3_JITTER {130.958} CONFIG.CLKOUT3_PHASE_ERROR {98.575}] [get_bd_cells clk_wizard]

#set_property -dict [list CONFIG.RESET_TYPE {ACTIVE_LOW} CONFIG.RESET_PORT {resetn}] [get_bd_cells clk_wizard]

# Create MIG
create_bd_cell -type ip -vlnv xilinx.com:ip:mig_7series:4.2 mig_7series_0
apply_board_connection -board_interface "ddr3_sdram" -ip_intf "mig_7series_0/mig_ddr_interface" -diagram $design_name

# Wire MIG and clock wizard
delete_bd_objs [get_bd_nets clk_ref_i_1] [get_bd_ports clk_ref_i]
delete_bd_objs [get_bd_nets sys_clk_i_1] [get_bd_ports sys_clk_i]
connect_bd_net [get_bd_pins clk_wizard/clk_out2] [get_bd_pins mig_7series_0/clk_ref_i]
connect_bd_net [get_bd_pins clk_wizard/clk_out1] [get_bd_pins mig_7series_0/sys_clk_i]

# Setup reset
#set_property -dict [list CONFIG.RESET_BOARD_INTERFACE {reset}] [get_bd_cells clk_wizard]
apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {reset ( System Reset ) } Manual_Source {New External Port (ACTIVE_LOW)}}  [get_bd_pins mig_7series_0/sys_rst]

# Create instance of MicroBlaze
create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:11.0 microblaze_mcu
apply_bd_automation -rule xilinx.com:bd_rule:microblaze -config { \
    axi_intc {0} \
    axi_periph {Enabled} \
    cache {16KB} \
    clk {/mig_7series_0/ui_clk (83 MHz)} \
    debug_module {Debug Only} \
    ecc {None} \
    local_mem {32KB} \
    preset {None} } [get_bd_cells microblaze_mcu]

set_property -dict [list CONFIG.C_ADDR_TAG_BITS {18} CONFIG.C_CACHE_BYTE_SIZE {1024} CONFIG.C_DCACHE_ADDR_TAG {18} CONFIG.C_DCACHE_BYTE_SIZE {1024}] [get_bd_cells microblaze_mcu]

# Rename Processor System Reset for MIG
set_property name proc_sys_reset_mig [get_bd_cells rst_mig_7series_0_83M]

# Enable full FPU
set_property -dict [list CONFIG.C_USE_FPU {2}] [get_bd_cells microblaze_mcu]

# Create UART-lite interface
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uart
if { ${eembc_power} } {
    set_property -dict [list CONFIG.C_BAUDRATE {9600}] [get_bd_cells axi_uart]
} else {
    apply_board_connection -board_interface "usb_uart" -ip_intf "axi_uart/UART" -diagram ${design_name}
    set_property -dict [list CONFIG.C_BAUDRATE {115200}] [get_bd_cells axi_uart]
}
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {/mig_7series_0/ui_clk (83 MHz)} \
    Clk_slave {Auto} \
    Clk_xbar {Auto} \
    Master {/microblaze_mcu (Periph)} \
    Slave {/axi_uart/S_AXI} \
    intc_ip {New AXI Interconnect} \
    master_apm {0}} [get_bd_intf_pins axi_uart/S_AXI]

# Forward UART interface to PMOD pins
if { ${eembc_power} } {
    create_bd_port -dir O pmod_uart_txd
    create_bd_port -dir I pmod_uart_rxd
    connect_bd_net [get_bd_pins /axi_uart/tx] [get_bd_ports pmod_uart_txd]
    connect_bd_net [get_bd_pins /axi_uart/rx] [get_bd_ports pmod_uart_rxd]
    add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/uart_pmod.xdc
}

# General constraints
add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/general.xdc

# Create UART interface
#create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uart16550:2.0 axi_uart
#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/mig_7series_0/ui_clk (83 MHz)} Clk_slave {Auto} Clk_xbar {Auto} Master {/microblaze_mcu (Periph)} Slave {/axi_uart/S_AXI} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins axi_uart/S_AXI]
#apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {usb_uart ( USB UART ) } Manual_Source {Auto}}  [get_bd_intf_pins axi_uart/UART]

# Debug UART
#create_bd_port -dir O usb_uart_txd_debug
#connect_bd_net [get_bd_pins /axi_uart/sout] [get_bd_ports usb_uart_txd_debug]
#add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/uart_debug.xdc

# Wire MIG with AXI interconnect
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_mig
set_property -dict [list CONFIG.NUM_SI {2} CONFIG.NUM_MI {1}] [get_bd_cells axi_interconnect_mig]
connect_bd_intf_net [get_bd_intf_pins microblaze_mcu/M_AXI_DC] -boundary_type upper [get_bd_intf_pins axi_interconnect_mig/S00_AXI]
connect_bd_intf_net [get_bd_intf_pins microblaze_mcu/M_AXI_IC] -boundary_type upper [get_bd_intf_pins axi_interconnect_mig/S01_AXI]
connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_mig/M00_AXI] [get_bd_intf_pins mig_7series_0/S_AXI]
connect_bd_net [get_bd_pins axi_interconnect_mig/ACLK] [get_bd_pins axi_interconnect_mig/S00_ACLK] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/S00_ACLK] [get_bd_pins axi_interconnect_mig/M00_ACLK] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/M00_ACLK] [get_bd_pins axi_interconnect_mig/S01_ACLK] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/S01_ACLK] [get_bd_pins mig_7series_0/ui_clk]
connect_bd_net [get_bd_pins axi_interconnect_mig/S01_ARESETN] [get_bd_pins axi_interconnect_mig/M00_ARESETN] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/M00_ARESETN] [get_bd_pins axi_interconnect_mig/S00_ARESETN] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/S00_ARESETN] [get_bd_pins axi_interconnect_mig/ARESETN] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/ARESETN] [get_bd_pins proc_sys_reset_mig/peripheral_aresetn]

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {/mig_7series_0/ui_clk (83 MHz)} \
    Clk_slave {Auto} \
    Clk_xbar {Auto} \
    Master {/microblaze_mcu (Periph)} \
    Slave {/axi_uart/S_AXI} \
    intc_ip {New AXI Interconnect} \
    master_apm {0} } [get_bd_intf_pins axi_uart/S_AXI]

# Reset
apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {reset ( System Reset ) } Manual_Source {Auto}}  [get_bd_pins clk_wizard/reset]

# Add timer
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_mcu
set_property -dict [list CONFIG.mode_64bit {0} CONFIG.enable_timer2 {1}] [get_bd_cells axi_timer_mcu]

# Wire timer
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/mig_7series_0/ui_clk (83 MHz)} Clk_slave {Auto} Clk_xbar {/mig_7series_0/ui_clk (83 MHz)} Master {/microblaze_mcu (Periph)} Slave {/axi_timer_mcu/S_AXI} intc_ip {/microblaze_mcu_axi_periph} master_apm {0}}  [get_bd_intf_pins axi_timer_mcu/S_AXI]

# Add AXI GPIO controlled pin
if { ${eembc_power} } {
    # Add AXI GPIO IP
    create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0
    # Wire it up to a single output pin (to a PMOD)
    set_property -dict [list CONFIG.C_GPIO_WIDTH {1} CONFIG.C_ALL_OUTPUTS {1}] [get_bd_cells axi_gpio_0]
    apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
        Clk_master {/mig_7series_0/ui_clk (83 MHz)} \
        Clk_slave {Auto} \
        Clk_xbar {/mig_7series_0/ui_clk (83 MHz)} \
        Master {/microblaze_mcu (Periph)} \
        Slave {/axi_gpio_0/S_AXI} \
        intc_ip {/microblaze_mcu_axi_periph} \
        master_apm {0}} [get_bd_intf_pins axi_gpio_0/S_AXI]
    create_bd_port -dir O pmod_pin
    connect_bd_net [get_bd_ports pmod_pin] [get_bd_pins axi_gpio_0/gpio_io_o]

    add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/pin_pmod.xdc
}

# Add Quad SPI for cold boot
if { ${eembc_power} } {
    create_bd_cell -type ip -vlnv xilinx.com:ip:axi_quad_spi:3.2 axi_quad_spi_0
    set_property -dict [list CONFIG.C_SPI_MEMORY {3}] [get_bd_cells axi_quad_spi_0]
    apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {qspi_flash ( Quad SPI Flash ) } Manual_Source {Auto}}  [get_bd_intf_pins axi_quad_spi_0/SPI_0]
    apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/mig_7series_0/ui_clk (83 MHz)} Clk_slave {Auto} Clk_xbar {/mig_7series_0/ui_clk (83 MHz)} Master {/microblaze_mcu (Periph)} Slave {/axi_quad_spi_0/AXI_LITE} intc_ip {/microblaze_mcu_axi_periph} master_apm {0}}  [get_bd_intf_pins axi_quad_spi_0/AXI_LITE]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wizard/clk_out1 (166 MHz)" }  [get_bd_pins axi_quad_spi_0/ext_spi_clk]

    # BUG FIX
    disconnect_bd_net /clk_wizard_clk_out1 [get_bd_pins axi_quad_spi_0/ext_spi_clk]
    connect_bd_net [get_bd_pins axi_quad_spi_0/ext_spi_clk] [get_bd_pins mig_7series_0/ui_clk]

#    set_property -dict [list CONFIG.CLKOUT3_USED {true} CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {50} CONFIG.MMCM_CLKOUT2_DIVIDE {20} CONFIG.NUM_OUT_CLKS {3} CONFIG.CLKOUT3_JITTER {151.636} CONFIG.CLKOUT3_PHASE_ERROR {98.575}] [get_bd_cells clk_wizard]
#    connect_bd_net [get_bd_pins clk_wizard/clk_out3] [get_bd_pins axi_quad_spi_0/ext_spi_clk]
#    set_property -dict [list CONFIG.C_SPI_MEMORY {3}] [get_bd_cells axi_quad_spi_0]
    add_files -fileset constrs_1 -norecurse $proj_dir/../xdc/qspi.xdc
}

# Add FINN IP(s)
create_bd_cell -type ip -vlnv xilinx_finn:finn:finn_design:1.0 finn_design

# Add reset IP for FINN IP
create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_finn

# Wire FINN IP(s)
connect_bd_net [get_bd_pins clk_wizard/clk_out3] [get_bd_pins proc_sys_reset_finn/slowest_sync_clk]
connect_bd_net [get_bd_pins clk_wizard/clk_out3] [get_bd_pins finn_design/ap_clk]
connect_bd_net [get_bd_pins proc_sys_reset_finn/peripheral_aresetn] [get_bd_pins finn_design/ap_rst_n]
connect_bd_net [get_bd_pins proc_sys_reset_finn/ext_reset_in] [get_bd_pins mig_7series_0/ui_clk_sync_rst]
connect_bd_net [get_bd_pins proc_sys_reset_finn/mb_debug_sys_rst] [get_bd_pins mdm_1/Debug_SYS_Rst]
connect_bd_net [get_bd_pins proc_sys_reset_finn/dcm_locked] [get_bd_pins mig_7series_0/mmcm_locked]
# Add AXI DMA IP
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma
set_property -dict [list CONFIG.c_include_sg {0} CONFIG.c_sg_include_stscntrl_strm {0} CONFIG.c_include_mm2s {1}] [get_bd_cells axi_dma]
set_property -dict [list CONFIG.c_m_axi_mm2s_data_width {32} CONFIG.c_m_axis_mm2s_tdata_width {8} CONFIG.c_mm2s_burst_size {16}] [get_bd_cells axi_dma]
set_property -dict [list CONFIG.c_include_mm2s_dre {1} CONFIG.c_include_s2mm_dre {1}] [get_bd_cells axi_dma]

# Wire AXI DMA IP
connect_bd_intf_net [get_bd_intf_pins axi_dma/M_AXIS_MM2S] [get_bd_intf_pins finn_design/s_axis_0]
connect_bd_intf_net [get_bd_intf_pins finn_design/m_axis_0] [get_bd_intf_pins axi_dma/S_AXIS_S2MM]
connect_bd_net [get_bd_pins axi_dma/m_axi_mm2s_aclk] [get_bd_pins clk_wizard/clk_out3]
connect_bd_net [get_bd_pins axi_dma/m_axi_s2mm_aclk] [get_bd_pins clk_wizard/clk_out3]

# Wire AXI-lite for AXI DMA IP (automation)
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {/mig_7series_0/ui_clk (83 MHz)} \
    Clk_slave {Auto} \
    Clk_xbar {/mig_7series_0/ui_clk (83 MHz)} \
    Master {/microblaze_mcu (Periph)} \
    Slave {/axi_dma/S_AXI_LITE} \
    intc_ip {/microblaze_mcu_axi_periph} \
    master_apm {0}} [get_bd_intf_pins axi_dma/S_AXI_LITE]

# Wire Master AXI for AXI DMA IP
set_property -dict [list CONFIG.NUM_SI {4} CONFIG.NUM_MI {1}] [get_bd_cells axi_interconnect_mig]
connect_bd_intf_net [get_bd_intf_pins axi_dma/M_AXI_MM2S] -boundary_type upper [get_bd_intf_pins axi_interconnect_mig/S02_AXI]
connect_bd_intf_net [get_bd_intf_pins axi_dma/M_AXI_S2MM] -boundary_type upper [get_bd_intf_pins axi_interconnect_mig/S03_AXI]
connect_bd_net [get_bd_pins axi_interconnect_mig/S02_ACLK] [get_bd_pins axi_interconnect_mig/S03_ACLK] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/S02_ARESETN] [get_bd_pins axi_interconnect_mig/S03_ARESETN] -boundary_type upper
connect_bd_net [get_bd_pins axi_interconnect_mig/S02_ACLK] [get_bd_pins clk_wizard/clk_out3]
connect_bd_net [get_bd_pins proc_sys_reset_finn/interconnect_aresetn] [get_bd_pins axi_interconnect_mig/S03_ARESETN]

connect_bd_net [get_bd_pins mig_7series_0/aresetn] [get_bd_pins proc_sys_reset_mig/peripheral_aresetn]

# Address Editor
assign_bd_address [get_bd_addr_segs {mig_7series_0/memmap/memaddr }]

# Validate the design block we created
validate_bd_design

# Save design
save_bd_design

# Create verilog top
#make_wrapper -files [get_files $proj_dir/$proj.srcs/sources_1/bd/$design_name/$design_name.bd] -top
#add_files -norecurse $proj_dir/$proj.srcs/sources_1/bd/$design_name/hdl/$design_name\_wrapper.v
add_files -norecurse $proj_dir/../$design_name\_wrapper.v

# In the Verilog wrapper, enable configuration for the EEMBC power setup
if { ${eembc_power} } {
    set_property verilog_define EEMBC_POWER=1 [current_fileset]
}

update_compile_order -fileset sources_1

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

