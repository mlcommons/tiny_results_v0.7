setws .
if { $::argc == 1 } {
    set accname [lindex $::argv 0]
    createhw -name $accname\_axidma_platform -hwspec hdf/$accname\_axidma_design_wrapper.hdf
    createapp -name $accname\_axidma_standalone -app {Hello World} -proc ps7_cortexa9_0 -hwproject $accname\_axidma_platform -os standalone
}
