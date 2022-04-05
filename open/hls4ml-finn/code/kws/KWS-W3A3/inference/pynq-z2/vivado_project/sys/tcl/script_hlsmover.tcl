if { $::argc == 1 } {
    set base_dir [lindex $::argv 0]
    set proj finn_zynq_link
    set accname finn_kws_mlp_w3a3

    ###
    ### Add an ILA for debug (disabled)
    ###

    #open_bd_design {$base_dir/$proj.srcs/sources_1/bd/top/top.bd}
    #set_property name idma0_ctrl [get_bd_intf_nets axi_interconnect_0_M00_AXI]
    #set_property name odma0_ctrl [get_bd_intf_nets axi_interconnect_0_M01_AXI]
    #set_property name idma0_maxi [get_bd_intf_nets idma0_m_axi_gmem0]
    #set_property name odma0_maxi [get_bd_intf_nets odma0_m_axi_gmem0]
    #set_property name idma0_axis [get_bd_intf_nets idma0_m_axis_0]
    #set_property name odma0_axis [get_bd_intf_nets StreamingDataflowPartition_1_m_axis_0]
    #set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {idma0_axis odma0_ctrl odma0_axis idma0_ctrl idma0_maxi odma0_maxi}]
    #apply_bd_automation -rule xilinx.com:bd_rule:debug -dict [list \
    #	[get_bd_intf_nets odma0_ctrl] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    #    [get_bd_intf_nets idma0_ctrl] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    #    [get_bd_intf_nets idma0_maxi] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    #    [get_bd_intf_nets odma0_axis] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    #    [get_bd_intf_nets odma0_maxi] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    #    [get_bd_intf_nets idma0_axis] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    #]
    #set_property -dict [list CONFIG.C_BRAM_CNT {24} CONFIG.C_DATA_DEPTH {1024}] [get_bd_cells system_ila_0]

    ## Run the synthesis step
    #launch_runs synth_1
    #wait_on_run -timeout 360 synth_1

    ## Run the implementation step
    #puts "INFO: Run bistream generation..."
    #launch_runs impl_1 -to_step write_bitstream -jobs 18
    #wait_on_run -timeout 360 impl_1

    #if {[get_property PROGRESS [get_runs impl_1]] != "100%"} {
    #    error "ERROR: bitstream generation failed!"
    #}

    ###
    ### Export the bitstream and the hardware for the SDK, FINN has already did everything for us :-)
    ###
    puts "INFO: Export hardware..."
    file copy -force $base_dir/$proj.runs/impl_1/top_wrapper.sysdef ../sdk/hdf/$accname\_hlsmover_design_wrapper.hdf
}
exit
